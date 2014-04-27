/*
    SDL Terminal - Simple DirectMedia Layer Terminal
    Copyright (C) 2005 Nicolas P. Rougier

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Nicolas P. Rougier
    rougier@loria.fr
*/

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_ttf.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define		SDL_TERMINAL_MASK			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#else
#define		SDL_TERMINAL_MASK			0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#endif


/* Maximum line size */
#define SDL_TERMINAL_MAX_LINE_SIZE		1024

/* Events generated by SDL Terminal */
#define SDL_TERMINALEVENT				(SDL_USEREVENT)


/*
 * Useful structure to hold a couple of int
 */
typedef struct Vec2i {
    union {int x; int w; int width;  int row;};
    union {int y; int h; int height; int column;};
} Vec2i;
    

/*
 * SDL Terminal structure
 */
typedef struct SDL_Terminal {
    int				status;					/* Terminal status */
    int				visible;				/* Terminal visibility status */
    int				active;					/* Terminal activity status (mouse pointer in or out)*/

    Vec2i			position;				/* Terminal position */
    Vec2i			size;					/* Terminal size (row, colum) */
    Vec2i			psize;					/* Terminal pixel size */
    Vec2i			cpos;					/* Cursor position (row, colum) */
    
    TTF_Font *		font;					/* Font */
    char *			font_filename;			/* Font filename */
    int				font_size;				/* Font size */
    Vec2i			glyph_size;				/* Font glyph size */
    
    SDL_Color		color;					/* Terminal background color */
    SDL_Color		br_color;				/* Terminal border color */
    SDL_Color		fg_color;				/* Current text foreground color */
    SDL_Color		default_fg_color;		/* Default text foreground color */
    SDL_Color		bg_color;				/* Current text background color */
    SDL_Color		default_bg_color;		/* Default text background color */
    SDL_Color		cursor_color;			/* Terminal cursor color */

    int				br_size;				/* Border size around text */
    int				tabsize;				/* Tab size */

    char			password_char;			/* Password symbol, used for input */

    char *			line;					/* Edited line */
    Vec2i			line_start;				/* Edited line start position */
    int				line_pos;				/* Position within edited line */

    char **			history;				/* History buffer, newest command at last index */
    int				history_size;			/* History buffer size */
    int				history_current;		/* Current yanked history */

    char **			buffer;					/* Text buffer, newest line at last index */
    int				buffer_size;			/* Text buffer size */

    SDL_Surface *	surface;				/* Terminal surface */
    unsigned int	texture;				/* Terminal surface GL texture */
    Vec2i			texture_size;			/* Terminal surface GL texture size */
    SDL_EventFilter event_filter;			/* Event filter */
    SDL_Event		event;					/* Entry event */
} SDL_Terminal;



/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


SDL_Terminal *SDL_CreateTerminal (void);
void SDL_DestroyTerminal (SDL_Terminal *terminal);

int SDL_TerminalBlit (SDL_Terminal *terminal);
int SDL_TerminalRefresh (SDL_Terminal *terminal);

int SDL_TerminalClear (SDL_Terminal *terminal);
int SDL_TerminalPrint (SDL_Terminal *terminal, char *text, ...);
int SDL_TerminalPrintAt (SDL_Terminal *terminal, int row, int column, char *text, ...);
int SDL_TerminalReset (SDL_Terminal *terminal);

int SDL_TerminalSetFont (SDL_Terminal *terminal, char *font_filename, int size);
int SDL_TerminalSetSize (SDL_Terminal *terminal, int row, int column);
int SDL_TerminalSetPosition (SDL_Terminal *terminal, int x, int y);
int SDL_TerminalSetColor (SDL_Terminal *terminal, int red, int green, int blue, int alpha);
int SDL_TerminalSetBorderColor (SDL_Terminal *terminal, int red, int green, int blue, int alpha);
int SDL_TerminalSetBorderSize (SDL_Terminal *terminal, int size);
int SDL_TerminalSetDefaultForeground (SDL_Terminal *terminal, int red, int green, int blue, int alpha);
int SDL_TerminalSetDefaultBackground (SDL_Terminal *terminal, int red, int green, int blue, int alpha);
int SDL_TerminalSetForeground (SDL_Terminal *terminal, int red, int green, int blue, int alpha);
int SDL_TerminalSetBackground (SDL_Terminal *terminal, int red, int green, int blue, int alpha);
int SDL_TerminalSetCursorColor (SDL_Terminal *terminal, int red, int green, int blue, int alpha);
int SDL_TerminalEnableBold (SDL_Terminal *terminal);
int SDL_TerminalEnableUnderline (SDL_Terminal *terminal);
int SDL_TerminalEnableItalic (SDL_Terminal *terminal);
int SDL_TerminalDisableBold (SDL_Terminal *terminal);
int SDL_TerminalDisableUnderline (SDL_Terminal *terminal);
int SDL_TerminalDisableItalic (SDL_Terminal *terminal);

int SDL_TerminalProcessEvent (SDL_Terminal *terminal, const SDL_Event *event);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

