CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(SDL_terminal)

INCLUDE_DIRECTORIES(include)

ADD_LIBRARY(SDL_terminal
	src/SDL_terminal.c
	src/SDL_terminal_private.h
	include/SDL_terminal.h
)

TARGET_LINK_LIBRARIES(SDL_terminal
	SDL_ttf
)
