#include "common.h"

void	deinit(t_graphic *graphic)
{
	SDL_DestroyWindow(graphic->win);
	SDL_DestroyRenderer(graphic->renderer);
	free(graphic);
	SDL_Quit();
}

t_graphic	*init(void)
{
	t_graphic	*graphic = NULL;

	graphic = (t_graphic *)calloc(sizeof(t_graphic), 1);
	if (graphic == NULL) {
		return (NULL);
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("SDL_Init: %s\n", SDL_GetError());
		free(graphic);
		return (NULL);
	}

	graphic->win = SDL_CreateWindow("", 0, 0, 1280, 720, 0);
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

