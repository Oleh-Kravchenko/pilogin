# Makefile.am for the SDL terminal

lib_LTLIBRARIES = libSDL_terminal.la
libSDL_terminalincludedir = $(includedir)/SDL
libSDL_terminal_la_SOURCES =	SDL_terminal.c SDL_terminal_private.h

libSDL_terminal_la_CFLAGS = 	\
	-I$(top_srcdir)/include		\
	-Wall -ansi	-D_GNU_SOURCE	\
	-fms-extensions

libSDL_terminal_la_LDFLAGS = 	\
	-no-undefined				\
	-release $(LT_RELEASE)

libSDL_terminal_la_LIBADD = @GL_LIBS@
