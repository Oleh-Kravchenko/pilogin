noinst_PROGRAMS = glcube terminal2D terminal3D pyterm

glcube_SOURCES  = glcube.c
glcube_CPPFLAGS = -I$(top_srcdir)/include -Wall -ansi -D_GNU_SOURCE -fms-extensions
glcube_LDADD    = ../src/libSDL_terminal.la @GL_LIBS@ -lGLU

terminal2D_SOURCES  = terminal2D.c	
terminal2D_CPPFLAGS = -I$(top_srcdir)/include -Wall -ansi -D_GNU_SOURCE -fms-extensions
terminal2D_LDADD    = ../src/libSDL_terminal.la

terminal3D_SOURCES  = terminal3D.c	
terminal3D_CPPFLAGS = -I$(top_srcdir)/include -Wall -ansi -D_GNU_SOURCE -fms-extensions
terminal3D_LDADD    = ../src/libSDL_terminal.la @GL_LIBS@ -lGLU

if PYTHON_DEVEL
pyterm_SOURCES  = pyterm.cc
pyterm_CPPFLAGS = -I$(top_srcdir)/include -Wall -ansi -O3 -D_GNU_SOURCE -fms-extensions @PYTHON_CPPFLAGS@
pyterm_LDADD    = ../src/libSDL_terminal.la @GL_LIBS@ -lGLU @PYTHON_LDFLAGS@ -L@PYTHON_EXTRA_LIBS@
endif

EXTRA_DIST = \
	VeraMono.ttf	\
	gnu-tux.bmp		\
	SDLTerminal.py
