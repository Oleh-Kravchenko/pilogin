#ifndef __SDL_TOOL_H
#define __SDL_TOOL_H

#include <SDL/SDL.h>

/**
 * @brief resize surface
 * @param [in,out] img surface
 * @param [in] w new width
 * @param [in] h new height
 * @return 0, or -1 if there was an error
 *
 * If function failed, @p img is unchanged.
 */
int resizeSurface(SDL_Surface** img, int w, int h);

#endif /* __SDL_TOOL_H */
