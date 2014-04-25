#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_terminal.h>

void key_press(SDL_keysym *keysym)
{
	switch (keysym->sym) {
		case SDLK_ESCAPE:
			SDL_Quit();
			exit(EXIT_SUCCESS);
			break;

		default:
			break;
	}
}

int main(int argc, char **argv)
{
	SDL_Surface *screen, *image;
	Uint32 last_tick = 0;
	int fps = 100;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
		SDL_Quit();

		return(1);
	}

	if(!(image = IMG_Load("./resource/wide.png"))) {
		if(!(image = IMG_Load("/usr/share/pilogin/resource/wide.png"))) {
			fprintf(stderr, "Cannot load image\n");
			SDL_Quit();

			return(1);
		}
	}


	screen = SDL_SetVideoMode(800, 600, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);

	SDL_WM_SetCaption("SDL Terminal without OpenGL", NULL);

	SDL_Terminal *terminal = SDL_CreateTerminal();

	if(!(-1 == SDL_TerminalSetFont(terminal, "./resource/monofont.ttf", 16))) {
		if(!(-1 == SDL_TerminalSetFont(terminal, "/usr/share/pilogin/resource/monofont.ttf", 16))) {
			fprintf(stderr, "Cannot load image\n");

			SDL_DestroyTerminal(terminal);
			SDL_Quit();

			return(1);
		}
	}




	SDL_TerminalSetSize(terminal, 80, 25);
	SDL_TerminalSetPosition(terminal, 0, 0);


	SDL_TerminalSetColor(terminal, 0, 0, 0, 0); /* background? */
	SDL_TerminalSetBorderColor(terminal, 0x28, 0xF7, 0x81, 255);
	SDL_TerminalSetForeground(terminal, 0, 255, 0, 255);
	SDL_TerminalSetBackground(terminal, 0, 0, 0, 0); /* No background since alpha=0 */

	SDL_TerminalClear(terminal);
	SDL_TerminalPrint(terminal, "Terminal initialized\n");
	SDL_TerminalPrint(terminal, "Using font VeraMono, %d\n", terminal->font_size);
	SDL_TerminalPrint(terminal, "Terminal geometry: %d x %d\n\n", terminal->size.column, terminal->size.row);
	SDL_TerminalPrint(terminal, "\033[1mBold on\033[22m - Bold off\n");
	SDL_TerminalPrint(terminal, "\033[3mItalic on\033[23m - Italic off\n");
	SDL_TerminalPrint(terminal, "\033[4mUnderline on\033[24m - Underline off\n");

	SDL_TerminalPrint(terminal,
			  "\033[30mBlack"
			  "\033[31m - Red"
			  "\033[32m - Green"
			  "\033[33m - Yellow"
			  "\033[34m - Blue"
			  "\033[35m - Magenta"
			  "\033[36m - Cyan"
			  "\033[37m - White\033[30m\n");
	SDL_TerminalReset(terminal);
	SDL_TerminalPrint(terminal,
			  "\033[40;37mBlack\033[30m"
			  "\033[49m - \033[41mRed"
			  "\033[49m - \033[42mGreen"
			  "\033[49m - \033[43mYellow"
			  "\033[49m - \033[44mBlue"
			  "\033[49m - \033[45mMagenta"
			  "\033[49m - \033[46mCyan"
			  "\033[49m - \033[47mWhite\033[49m\n");

	int done = 0;
	SDL_Event event;
	last_tick = SDL_GetTicks();

	while (!done) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				key_press(&event.key.keysym);
				break;
			case SDL_QUIT:
				done = 1;
				break;
			case SDL_TERMINALEVENT:
				printf("Terminal event: %s\n", (char *)event.user.data2);
				break;
			default:
				break;
			}
		}

		Uint32 wait = (Uint32)(1000.0f / fps);
		Uint32 new_tick = SDL_GetTicks();
		if ((new_tick - last_tick) < wait)
			SDL_Delay(wait - (new_tick - last_tick));
		last_tick = SDL_GetTicks();

		SDL_FillRect(screen, 0, 0);
		SDL_Rect dst = { (800 - image->w) / 2, (600 - image->h) / 2, 0, 0 };
		SDL_BlitSurface(image, NULL, screen, &dst);
		SDL_TerminalBlit(terminal);

		SDL_Flip(screen);
	}

	SDL_Quit();

	return 0;
}
