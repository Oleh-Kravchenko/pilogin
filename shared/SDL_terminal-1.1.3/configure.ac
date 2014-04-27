AC_INIT(README)
AC_CONFIG_AUX_DIR(config)

dnl Set various version strings - taken gratefully from the GTk sources
MAJOR_VERSION=1
MINOR_VERSION=1
MICRO_VERSION=3
VERSION=$MAJOR_VERSION.$MINOR_VERSION.$MICRO_VERSION

AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(MICRO_VERSION)
AC_SUBST(VERSION)

# libtool versioning
LT_RELEASE=$MAJOR_VERSION.$MINOR_VERSION
AC_SUBST(LT_RELEASE)

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Setup for automake
AM_INIT_AUTOMAKE(SDL_terminal, $VERSION)
AC_CONFIG_SRCDIR([src/SDL_terminal.c])
AC_CONFIG_HEADERS([config/config.h:config/config.h.in])
AM_MAINTAINER_MODE

dnl Check for tools
AC_PROG_LIBTOOL
AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_INSTALL
AC_FUNC_ALLOCA


# Use the macro SDL provides to check the installed version of the SDL
# development environment.  Abort the configuration process if the
# minimum version we require isn't available.
SDL_VERSION=1.2.4
AM_PATH_SDL($SDL_VERSION,
            :,
      AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
CFLAGS="$CFLAGS $SDL_CFLAGS"
CPPFLAGS="$CFLAGS $SDL_CFLAGS"

AC_CHECK_LIB( SDL_ttf, 
              TTF_Init,,
    	      AC_MSG_ERROR([[ 
SDL_Terminal requires the SDL_ttf library. If you have a distribution supplied 
SDL_ttf.RPM installed, perhaps the SDL_ttf-devel.RPM is missing. 
If your distribution does not include the SDL_ttf library, you can get it 
from http://www.libsdl.org/projects/SDL_ttf/ ]]))

# Add the SDL preprocessor flags and libraries to the build process
LIBS="$LIBS $SDL_LIBS -lm -lSDL_ttf"


dnl Check for OpenGL (code taken from SDL_ttf)
case "$target" in
    *-*-cygwin* | *-*-mingw32*)
        MATHLIB=""
        SYS_GL_LIBS="-lopengl32"
        ;;
    *-*-beos*)
        MATHLIB=""
        SYS_GL_LIBS="-lGL"
        ;;
    *-*-darwin*)
        MATHLIB=""
        SYS_GL_LIBS=""
        ;;
    *-*-aix*)
        if test x$ac_cv_prog_gcc = xyes; then
            CFLAGS="-mthreads"
	fi
        SYS_GL_LIBS=""
        ;;
    *)
        MATHLIB="-lm"
        AC_PATH_X
        AC_PATH_XTRA
        if test x$have_x = xyes; then
            CFLAGS="$CFLAGS $X_CFLAGS"
            SYS_GL_LIBS="$X_LIBS -lGL"
        else
            SYS_GL_LIBS="-lGL"
        fi
        ;;
esac
AC_MSG_CHECKING(for OpenGL support)
have_opengl=no
AC_TRY_COMPILE([
 #include <SDL/SDL_opengl.h>
],[
 GLuint texture;
],[
have_opengl=yes
])
AC_MSG_RESULT($have_opengl)
if test x$have_opengl = xyes; then
    CFLAGS="$CFLAGS -DHAVE_OPENGL"
    CPPFLAGS="$CPPFLAGS -DHAVE_OPENGL"
    GL_LIBS="$SYS_GL_LIBS"
else
    GL_LIBS=""
fi
AC_SUBST(GL_LIBS)

AC_PYTHON_DEVEL

# Finally create all the generated files
# The configure script takes "file.in" and substitutes variables to produce
# "file".  In this case we are just generating the Makefiles, but this could
# be used to generate any number of automatically generated files.
AC_OUTPUT([
Makefile
src/Makefile
include/Makefile
examples/Makefile
])
