#include <stdlib.h>
#include <assert.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>

#include "shared/SDL_terminal-1.1.3/include/SDL_terminal.h"

#include "global.h"
#include "strtool.h"
#include "sdltool.h"

/*------------------------------------------------------------------------*/

static int terminate = 0;

/*------------------------------------------------------------------------*/

static void terminal_random(SDL_Terminal* terminal)
{
	char s1[13], s2[13];
	Uint32 p = 0x8000f000;
	int i;

	SDL_TerminalClear(terminal);
	SDL_TerminalPrint(terminal, "ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL\n");
	SDL_TerminalPrint(terminal, "ENTER PASSWORD NOW\n\n");

	for(i = 0; i < 21; ++ i) {
		strrand(s1, sizeof(s1));
		strrand(s2, sizeof(s2));

		SDL_TerminalPrint(terminal, "0x%04X %s 0x%04X %s\n", p, s1, p + 12, s2);
		p += 24;
	}
}

/*------------------------------------------------------------------------*/

static void terminal_attempt(SDL_Terminal* terminal)
{
	terminal->cpos.row = 52;
	terminal->cpos.column = 9;
	SDL_TerminalPrint(terminal, "> \033[31mPassword was wrong!!\n");

	terminal->cpos.row = 52;
	terminal->cpos.column = 10;
	SDL_TerminalSetForeground(terminal, 0x2e, 0xfc, 0x8f, 255);
	SDL_TerminalPrint(terminal, ">\n");
}

/*------------------------------------------------------------------------*/

static void terminal_password(SDL_Terminal* terminal)
{
	terminal->cpos.row = 52;
	terminal->cpos.column = 12;
	SDL_TerminalPrint(terminal, "> Enter password\n\n");

	terminal->cpos.row = 52;
	terminal->cpos.column = 13;
	SDL_TerminalPrint(terminal, "> ");
}

/*------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	SDL_Surface *screen, *image;
	Uint32 last_tick = 0;
	int fps = 24;

char password[0x100];
char video_file[0x100];

if(argc != 3) {
	printf("%s 1234 ./resource/test.mp4\n", argv[0]);

	return(1);
}

strncpy(password, argv[1], sizeof(password));
strncpy(video_file, argv[2], sizeof(video_file));

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());

		SDL_Quit();

		return(1);
	}

	if (!(image = IMG_Load("./resource/" BACKGROUND))) {
		if(!(image = IMG_Load("/usr/share/pilogin/resource/" BACKGROUND))) {
			fprintf(stderr, "Cannot load image\n");

			SDL_Quit();

			return(1);
		}
	}

	if(resizeSurface(&image, WIDTH, HEIGHT)) {
		fprintf(stderr, "Cannot resize image\n");

		SDL_FreeSurface(image);
		SDL_Quit();

		return(1);
	}

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_FULLSCREEN | SDL_DOUBLEBUF);
	assert(screen);
	SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);

	SDL_Terminal *terminal;

	terminal = SDL_CreateTerminal();

	if (!(-1 == SDL_TerminalSetFont(terminal, "./resource/" FONT, 16))) {
		if (!(-1 == SDL_TerminalSetFont(terminal, "/usr/share/pilogin/resource/" FONT, 16))) {
			fprintf(stderr, "Cannot load image\n");

			SDL_DestroyTerminal(terminal);
			SDL_Quit();

			return(1);
		}
	}

	SDL_TerminalSetSize(terminal, 80, 25);
	SDL_TerminalSetPosition(terminal, (screen->w - terminal->psize.w) / 2, (screen->h - terminal->psize.h) / 2);
	SDL_TerminalSetPosition(terminal, 0, 0);
	SDL_TerminalSetBorderColor(terminal, 0x00, 0xf0, 0x00, 0x00);
	SDL_TerminalSetColor(terminal, 0, 0, 0, 0); /* background? */
	SDL_TerminalSetForeground(terminal, 0x2e, 0xfc, 0x8f, 255); /* text */
	SDL_TerminalSetCursorColor(terminal, 0x2e, 0xfc, 0x8f, 255);
	SDL_TerminalClear(terminal);

	terminal->password_char = '*';
	terminal->active = 1;

	SDL_Event event;
	last_tick = SDL_GetTicks();

	SDL_ShowCursor(0);
	terminal_random(terminal);
	terminal_password(terminal);

int wrong = 0;
int prompt = 0;
int attempt = 0;

	while (!terminate) {

		if(wrong > SDL_GetTicks()) {
			terminal_random(terminal);
		} else if(!prompt) {
			terminal_random(terminal);

			if(attempt) {
				terminal_attempt(terminal);
			}

			terminal_password(terminal);

			prompt = 1;
		}


		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN: {
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						terminate = 1;

						break;
					}

					break;
				}

				case SDL_QUIT:
					terminate = 1;
					break;

				case SDL_TERMINALEVENT:
					if(!strcmp(PASSWORD_EXIT, event.user.data2)) {
						terminate = 1;
						break;
					}

					if(!strcmp(password, event.user.data2)) {
						int player_main(const char* filename, SDL_Surface* display);
						player_main(video_file, screen);

						SDL_FreeSurface(screen);
						screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE | SDL_FULLSCREEN | SDL_DOUBLEBUF);
						assert(screen);
						SDL_WarpMouse(WIDTH / 2, HEIGHT / 2);


						terminal->active = 1;
						SDL_ShowCursor(0);
						terminal_random(terminal);
						terminal_password(terminal);

						prompt = 1;
						break;
					} else {
						printf("Terminal event: %s\n", (char *)event.user.data2);

						wrong = SDL_GetTicks() + LOGIN_DELAY;
						prompt = 0;
						attempt = 1;
					}

					break;

				default:
					break;
			}
		}

		Uint32 wait = (Uint32)(1000.0f / fps);
		Uint32 new_tick = SDL_GetTicks();

		if ((new_tick - last_tick) < wait) {
			SDL_Delay(wait - (new_tick - last_tick));
		}
		last_tick = SDL_GetTicks();

		SDL_FillRect(screen, NULL, 0);

		SDL_Rect dst = { (screen->w - image->w) / 2, (screen->h - image->h) / 2, 0, 0 };

		SDL_BlitSurface(image, NULL, screen, &dst);
		SDL_TerminalBlit(terminal);

		SDL_Flip(screen);
	}

	SDL_Quit();

	return 0;
}
