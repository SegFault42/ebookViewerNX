#include "common.h"

extern t_graphic	*graphic;

void	deinit(void)
{
	SDL_DestroyWindow(graphic->win);
	SDL_DestroyRenderer(graphic->renderer);

	free(graphic);

	SDL_Quit();
	log_info("deinit() [Success]");
}

t_graphic	*init(void)
{
	graphic = (t_graphic *)calloc(sizeof(t_graphic), 1);
	if (graphic == NULL) {
		return (NULL);
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("SDL_Init: %s\n", SDL_GetError());
		free(graphic);
		return (NULL);
	}

	graphic->win = SDL_CreateWindow("", 0, 0, WIN_WIDTH, WIN_HEIGHT, 0);
	if (graphic->win == NULL) {
		SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
		SDL_Quit();
		free(graphic);
		return (NULL);
	}

	graphic->renderer = SDL_CreateRenderer(graphic->win, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (graphic->renderer == NULL) {
		SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(graphic->win);
		SDL_Quit();
		free(graphic);
		return (NULL);
	}

	return (graphic);
}

void	draw_ppm(fz_pixmap *ppm)
{
	// Convert pix array to surface
	SDL_Surface *image = SDL_CreateRGBSurfaceFrom(ppm->samples,
 			ppm->w, ppm->h, ppm->n * 8, ppm->w * ppm->n,
 			0x000000FF, 0x0000FF00, 0x00FF0000, 0);

	// Convert surface to texture
	SDL_Texture	*texture = SDL_CreateTextureFromSurface(graphic->renderer, image);
	// Free surface
	SDL_FreeSurface(image);

	// Create rect and move it in the middle in X axis
	SDL_Rect dstrect = {(WIN_WIDTH - ppm->w) / 2, 0, ppm->w, ppm->h};

	// clear window
	SDL_RenderClear(graphic->renderer);

	SDL_RenderCopyEx(graphic->renderer, texture, NULL, &dstrect, 0, NULL, SDL_FLIP_NONE);
	// Free texture
	SDL_DestroyTexture(texture);
	// Render
	SDL_RenderPresent(graphic->renderer);

	log_info("draw_ppm() [Success]");
}
