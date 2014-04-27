#include <SDL/SDL.h>
#include <SDL/SDL_rotozoom.h>

/*------------------------------------------------------------------------*/

int resizeSurface(SDL_Surface** img, int w, int h)
{
	SDL_Surface* ret;

	double zoomx = w / (double)(*img)->w;
	double zoomy = h / (double)(*img)->h;

	if(!(ret = zoomSurface(*img, zoomx, zoomy, SMOOTHING_ON))) {
		return(-1);
	}

	if((*img)->flags & SDL_SRCCOLORKEY) {
		if(-1 == SDL_SetColorKey(ret, SDL_SRCCOLORKEY, (*img)->format->colorkey)) {
			SDL_FreeSurface(ret);

			return(-1);
		}
	}

	SDL_FreeSurface(*img);
	*img = ret;

	return(0);
}
