CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(pilogin)

INCLUDE_DIRECTORIES(include)

include(FindSDL)

ADD_EXECUTABLE(pilogin
	main.c
	sdltool.c
	strtool.c
	shared/player2.c
)

TARGET_LINK_LIBRARIES(pilogin
	m
	SDL
	SDL_gfx
	SDL_image
	SDL_terminal

	avutil
	avformat
	avcodec
	swscale
	z
)

ADD_SUBDIRECTORY(shared/SDL_terminal-1.1.3 SDL_terminal)
