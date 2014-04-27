/* 
 * Copyright (c) 2005-2006 Nicolas P. Rougier
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this software; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <Python.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_thread.h>
#include <stdlib.h>
#include "SDL_terminal.h"

#define TERMINAL_W	768
#define TERMINAL_H	368
#define FIFO_SIZE	1024

/*
 *
 */
void			init_gl (void);
SDL_Surface *	init_sdl (int &flags);
void			init_python (void);
static PyObject *stdout_write (PyObject *self, PyObject *args);
static PyObject *stderr_write (PyObject *self, PyObject *args);
static PyObject *stdin_readline (PyObject *self, PyObject *args);
float			wait_frame (void);
void			gl_scene (float dt);
void			gl_cube (float dt);


/*
 *
 */

static char *fifo[FIFO_SIZE];
static int fifo_head = 0;
static int fifo_tail = 0;

static char *input = 0;
static int input_flag = 0;
static SDL_sem *lock;

SDL_Thread *thread = 0;



static SDL_Terminal *terminal = 0;
static PyMethodDef stdout_methods[] = {
	{"write",	stdout_write,	1},
	{NULL,		NULL}};
static PyMethodDef stderr_methods[] = {
	{"write",	stderr_write,	1},
	{NULL,		NULL}};
static PyMethodDef stdin_methods[] = {
	{"readline", stdin_readline,	1},
	{NULL,		 NULL}};



/*
 *
 */
void key_press (SDL_keysym *keysym)
{
    switch (keysym->sym) {
	case SDLK_ESCAPE:
		SDL_Quit ();
		exit (EXIT_SUCCESS);
	    break;
	default:
	    break;
	}
}

/*
 *
 */
void resize (int width, int height) {
    GLfloat ratio;
    if (height == 0)
        height = 1;
    ratio = (GLfloat)width / (GLfloat) height;
    glViewport (0, 0, (GLint) width, (GLint) height);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity ();
    gluPerspective (30.0f, ratio, 0.1f, 100.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}

int thread_func (void *data) {
    PyRun_SimpleString ((char*) data);
    // fprintf (stderr, "1. SemPost, pre: %d\n", SDL_SemValue(lock));
    SDL_SemPost (lock);
    // fprintf (stderr, "1. SemPost, post: %d\n", SDL_SemValue(lock));
    return 0;
}


/*
 *
 */
int main (int argc, char **argv)
{
    int i;
    int video_flags;
    SDL_Surface *surface;

    surface = init_sdl (video_flags);
    SDL_WM_SetCaption ("SDL Terminal emulating a python shell", NULL);

    terminal =  SDL_CreateTerminal ();
    SDL_TerminalSetFont (terminal, "./VeraMono.ttf", 12);
    SDL_TerminalSetSize (terminal, 80, 24);
    SDL_TerminalSetPosition (terminal, 0, 0);
    SDL_TerminalSetColor (terminal, 255,255,221,255);
    SDL_TerminalSetBorderColor (terminal, 255,255,221,255);
    SDL_TerminalSetForeground (terminal, 0,0,0,255);
    SDL_TerminalSetBackground (terminal, 0,0,0,0);
    SDL_TerminalClear (terminal);

    init_gl ();
    surface = SDL_SetVideoMode (terminal->psize.w,terminal->psize.h, 32, video_flags);
    resize (terminal->psize.w,terminal->psize.h);    

    init_python();


    /* Python banner */
    PyRun_SimpleString("print 'Python %s on %s' % (sys.version, sys.platform) ");
    PyRun_SimpleString ("print 'Type \"help\", \"copyright\", \"credits\" or \"license\" for more information.'");
    PyRun_SimpleString ("console.push(\"\")");

    int done = 0;
    SDL_Event event;
    char buffer[SDL_TERMINAL_MAX_LINE_SIZE+24];
    while (!done) {
	    while (SDL_PollEvent(&event)) {
            switch (event.type) {	
            case SDL_VIDEORESIZE:
                surface = SDL_SetVideoMode( event.resize.w, event.resize.h, 32, video_flags);
                if (!surface) {
                    fprintf( stderr, "Could not get a surface after resize: %s\n", SDL_GetError( ) );
                    SDL_Quit();
                    exit (EXIT_FAILURE);
                }
                resize (event.resize.w, event.resize.h);
                break;
			case SDL_KEYDOWN:
                if (event.key.keysym.mod & KMOD_CTRL) {
                    if (event.key.keysym.sym == SDLK_d)
                        done = 1;
                    else if (event.key.keysym.sym == SDLK_c) {
                        if (input_flag) {
                            input_flag = 2;
                            // fprintf (stderr, "2. SemPost, pre: %d\n", SDL_SemValue(lock));
                            SDL_SemPost (lock);
                            // fprintf (stderr, "2. SemPost, post: %d\n", SDL_SemValue(lock));
                        }
                        else {
                            PyRun_SimpleString("raise KeyboardInterrupt\n");
                            PyErr_Print();
                            PyRun_SimpleString("console.push(\"\")");
                        }
                    }
                }
                else
                    key_press (&event.key.keysym);
			    break;
			case SDL_QUIT:
			    done = 1;
			    break;
            case SDL_TERMINALEVENT:
                if (input_flag) {
                    input = (char *) event.user.data2;
                    // fprintf (stderr, "3. SemPost, pre: %d\n", SDL_SemValue(lock));
                    SDL_SemPost (lock);
                    // fprintf (stderr, "3. SemPost, post: %d\n", SDL_SemValue(lock));
                }
                else {
                    sprintf (buffer, "console.push(\"\"\"%s\"\"\")", (char *)event.user.data2);
                    // fprintf (stderr, "4. SemWait, pre: %d\n", SDL_SemValue(lock));
                    SDL_SemWait (lock);
                    // fprintf (stderr, "4. SemWait, post: %d\n", SDL_SemValue(lock));

                    if (thread)
                        SDL_WaitThread(thread, 0);
                    thread = SDL_CreateThread (thread_func, buffer);
                }
                break;
			default:
			    break;
			}
		}

        for (i=0; i<10; i++)
            if (fifo_head != fifo_tail) {
                SDL_TerminalPrint (terminal, fifo[fifo_head]);
                //free (fifo[fifo_head]);
                fifo_head++;
                if (fifo_head >= FIFO_SIZE)
                    fifo_head = 0;
            }

        float dt = wait_frame ();
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity ();
        SDL_TerminalBlit (terminal);
        gl_scene (dt);
        SDL_GL_SwapBuffers ();
    }
	
    Py_Finalize();
    SDL_Quit ();
    exit (EXIT_SUCCESS);
	return 0;
}



/*
 * Initalization of OpenGL stuff
 */
void 
init_gl (void)
{
    GLfloat light_ambient[]  = { 0.25f, 0.25f, 0.25f, 1.0f };
    GLfloat light_diffuse[]  = { 0.75f, 0.75f, 0.75f, 1.0f };
    GLfloat light_position[] = { 0.0f, 0.0f, 2.0f, 1.0f };

    glDisable (GL_TEXTURE_2D);
    glShadeModel (GL_SMOOTH);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth (1.0f);
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
}

/*
 * Intialization of SDL stuff
 */
SDL_Surface *
init_sdl (int &flags)
{
    SDL_Surface *surface;
    const SDL_VideoInfo *info;


    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
	    fprintf (stderr, "Video initialization failed: %s\n", SDL_GetError( ));
		SDL_Quit();
		exit (EXIT_FAILURE);
	}

    info = SDL_GetVideoInfo ();
    if (!info) {
	    fprintf (stderr, "Video query failed: %s\n", SDL_GetError());
		SDL_Quit();
		exit (EXIT_FAILURE);
	}

    flags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
    flags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
    flags |= SDL_HWPALETTE;       /* Store the palette in hardware */
    //videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */
    if (info->hw_available )
        flags |= SDL_HWSURFACE;
    else
        flags |= SDL_SWSURFACE;

    if (info->blit_hw )
        flags |= SDL_HWACCEL;
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

    surface = SDL_SetVideoMode (TERMINAL_W, TERMINAL_H, 32, flags);

    if (!surface) {
	    fprintf (stderr,  "Video mode set failed: %s\n", SDL_GetError( ));
		SDL_Quit();
		exit (EXIT_FAILURE);
	}

    lock = SDL_CreateSemaphore (1);
    // fprintf (stderr, "initial: %d\n", SDL_SemValue(lock));
    return surface;
}

static PyObject *
stdout_write (PyObject *self, PyObject *args) {
    char *text;
	PyArg_ParseTuple (args, "s", &text);
    fifo[fifo_tail++] = strdup (text);
    if (fifo_tail >= FIFO_SIZE)
        fifo_tail = 0;

	Py_INCREF (Py_None);
	return Py_None;
}

static PyObject *
stderr_write (PyObject *self, PyObject *args) {
    char *text;
    static char *red   = "\033[31m";
    static char *black = "\033[30m";

    fifo[fifo_tail++] = strdup(red);
    if (fifo_tail >= FIFO_SIZE)
        fifo_tail = 0;

	PyArg_ParseTuple (args, "s", &text);
    fifo[fifo_tail++] = strdup (text);
    if (fifo_tail >= FIFO_SIZE)
        fifo_tail = 0;

    fifo[fifo_tail++] = strdup(black);
    if (fifo_tail >= FIFO_SIZE)
        fifo_tail = 0;

	Py_INCREF (Py_None);
	return Py_None;
}

static PyObject *
stdin_readline (PyObject *self, PyObject *args)
{
    char *empty="\n";
    input_flag = 1;

    // fprintf (stderr, "5. SemWait, pre: %d\n", SDL_SemValue(lock));
    SDL_SemWait (lock);
    // fprintf (stderr, "5. SemWait, post: %d\n", SDL_SemValue(lock));

    if (input_flag == 1) {
        input_flag = 0;
        if (strlen ((char *) input))
            return Py_BuildValue ("s", (char *) input);
    }
    else if (input_flag == 2) {
        PyErr_SetNone(PyExc_KeyboardInterrupt);
        return NULL;
    }
    
    return Py_BuildValue ("s", (char *) empty);
}

/*
 * Initalization of python stuff
 */
void
init_python (void) 
{
    Py_Initialize();
    PyObject *module;

    /* stdout redirection */
    module = Py_InitModule ("stdout", stdout_methods);
	PySys_SetObject("stdout", module);

    /* stderr redirection */
    module = Py_InitModule ("stderr", stderr_methods);
	PySys_SetObject("stderr", module);

    /* stdin redirection */
    module = Py_InitModule ("stdin", stdin_methods);
	PySys_SetObject("stdin", module);

    /* prompt 1 */
    PyObject *ps1 = PySys_GetObject("ps1");
	if (ps1 == NULL) {
		PySys_SetObject("ps1", ps1 = PyString_FromString(">>> "));
		Py_XDECREF(ps1);
	}

    /* prompt 2 */
    PyObject *ps2 = PySys_GetObject("ps2");
	if (ps2 == NULL) {
		PySys_SetObject("ps2", ps2 = PyString_FromString("... "));
		Py_XDECREF(ps2);
	}

    char *InitString =
        "import sys\n"
        "import SDLTerminal\n"
        "console = SDLTerminal.InteractiveConsole()\n";    
    PyRun_SimpleString (InitString);
}


/*
 *
 */
float
wait_frame () 
{
    int fps = 100;
    
    static Uint32 last_tick = SDL_GetTicks();
    Uint32 wait = (Uint32)(1000.0f/fps);
    Uint32 new_tick  = SDL_GetTicks();
    if ((new_tick - last_tick) < wait)
        SDL_Delay (wait - (new_tick-last_tick));
    float dt = (SDL_GetTicks() - last_tick)/1000.0f;
    last_tick = SDL_GetTicks();
    return dt;
}

/*
 *
 */
void gl_scene(float dt)
{
    glDisable(GL_BLEND);

    glPolygonOffset (1.0f, 1.0f);
    glEnable (GL_POLYGON_OFFSET_FILL);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    float color[] = {131/256.0, 187/256.0, 239/256.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glLoadIdentity ();
    glTranslatef (4.5, -2.0, -18);
    gl_cube (2*dt);
    glPopMatrix();


    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(0.5f);

    glDisable (GL_POLYGON_OFFSET_FILL);
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
    float black[] = {0,0,0,1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
    glPushMatrix ();
    glLoadIdentity ();
    glTranslatef (4.5, -2.0, -18);
    gl_cube (0);
    glPopMatrix();

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
}


void gl_cube (float dt) {
    static float xrot = 180;
    static float yrot = 180;
    xrot += dt*17;
    yrot += dt*13;
    glRotatef (xrot, 1.0f, 0.0f, 0.0f);
    glRotatef (yrot, 0.0f, 1.0f, 0.0f);

    glBegin(GL_QUADS); {
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( -1.0f, -1.0f,  1.0f );
        glVertex3f(  1.0f, -1.0f,  1.0f );
        glVertex3f(  1.0f,  1.0f,  1.0f );
        glVertex3f( -1.0f,  1.0f,  1.0f );
        
        glNormal3f( 0.0f, 0.0f, -1.0f);
        glVertex3f( -1.0f, -1.0f, -1.0f );
        glVertex3f( -1.0f,  1.0f, -1.0f );
        glVertex3f(  1.0f,  1.0f, -1.0f );
        glVertex3f(  1.0f, -1.0f, -1.0f );
        
        glNormal3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( -1.0f,  1.0f, -1.0f );
        glVertex3f( -1.0f,  1.0f,  1.0f );
        glVertex3f(  1.0f,  1.0f,  1.0f );
        glVertex3f(  1.0f,  1.0f, -1.0f );
        
        glNormal3f( 0.0f, -1.0f, 0.0f );
        glVertex3f( -1.0f, -1.0f, -1.0f );
        glVertex3f(  1.0f, -1.0f, -1.0f );
        glVertex3f(  1.0f, -1.0f,  1.0f );
        glVertex3f( -1.0f, -1.0f,  1.0f );
        
        glNormal3f( 1.0f, 0.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f, -1.0f );
        glVertex3f( 1.0f,  1.0f, -1.0f );
        glVertex3f( 1.0f,  1.0f,  1.0f );
        glVertex3f( 1.0f, -1.0f,  1.0f );
        
        glNormal3f( -1.0f, 0.0f, 0.0f );
        glVertex3f( -1.0f, -1.0f, -1.0f );
        glVertex3f( -1.0f, -1.0f,  1.0f );
        glVertex3f( -1.0f,  1.0f,  1.0f );
        glVertex3f( -1.0f,  1.0f, -1.0f );
    } glEnd();
}
