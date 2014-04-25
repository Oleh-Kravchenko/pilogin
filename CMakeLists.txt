CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ping)

ADD_DEFINITIONS(-pedantic -std=gnu99 -Wall -Werror -g3)

ADD_EXECUTABLE(pilogin
	main.c
)

TARGET_LINK_LIBRARIES(pilogin SDL SDL_terminal SDL_image)