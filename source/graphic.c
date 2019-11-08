#include "common.h"

extern t_graphic	*graphic;
extern t_transform	*trans;
extern t_ebook		*ebook;

bool	init_ttf(void)
{
	graphic->ttf = (t_ttf *)calloc(sizeof(t_ttf), 1);
	if (graphic->ttf == NULL) {
		log_fatal("init_ttf() : calloc [failure]");
		return (false);
	}

	if (TTF_Init() == -1) {
		log_fatal("TTF_Init(): %s\n", TTF_GetError());
		free(graphic->ttf);
		graphic->ttf = NULL;
		return (false);
	}

	graphic->ttf->font_small = TTF_OpenFont("romfs:/fonts/NintendoStandard.ttf", 24);
	graphic->ttf->font_medium = TTF_OpenFont("romfs:/fonts/NintendoStandard.ttf", 31);
	graphic->ttf->font_large = TTF_OpenFont("romfs:/fonts/NintendoStandard.ttf", 42);
	if (graphic->ttf->font_small == NULL || graphic->ttf->font_medium == NULL || graphic->ttf->font_large == NULL) {
		log_fatal("TTF_OpenFont(): %s\n", TTF_GetError());
		free(graphic->ttf);
		graphic->ttf = NULL;
		TTF_Quit();
		return (false);
	}

	log_info("init_ttf() [Success]");
	return (true);
}

void	deinit_ttf(void)
{
	TTF_CloseFont(graphic->ttf->font_small);
	TTF_CloseFont(graphic->ttf->font_medium);
	TTF_CloseFont(graphic->ttf->font_large);

	TTF_Quit();

	free(graphic->ttf);
	graphic->ttf = NULL;

	log_info("init_ttf() [Failure]");
}

bool	init_graphic(void)
{
	graphic = (t_graphic *)calloc(sizeof(t_graphic), 1);
	if (graphic == NULL) {
		log_fatal("init_graphic() : calloc [Failure]");
		return (false);
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_fatal("SDL_Init(): %s\n", SDL_GetError());
		free(graphic);
		return (false);
	}

	graphic->win = SDL_CreateWindow("", 0, 0, WIN_WIDTH, WIN_HEIGHT, 0);
	if (graphic->win == NULL) {
		log_fatal("SDL_CreateWindow(): %s\n", SDL_GetError());
		SDL_Quit();
		free(graphic);
		return (false);
	}

	graphic->renderer = SDL_CreateRenderer(graphic->win, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (graphic->renderer == NULL) {
		log_fatal("SDL_CreateRenderer(): %s\n", SDL_GetError());
		SDL_DestroyWindow(graphic->win);
		SDL_Quit();
		free(graphic);
		return (false);
	}

	log_info("init_graphic() [Success]");
	return (true);
}

void	deinit_graphic(void)
{
	SDL_DestroyWindow(graphic->win);
	SDL_DestroyRenderer(graphic->renderer);

	free(graphic);
	graphic = NULL;

	SDL_Quit();

	log_info("deinit_graphic() [Success]");
}

void	draw_ppm(fz_pixmap *ppm)
{
	SDL_Surface	*image = NULL;
	SDL_Texture	*texture = NULL;

	// Convert pix array to surface
	image = SDL_CreateRGBSurfaceFrom(ppm->samples,
			ppm->w, ppm->h, ppm->n * 8, ppm->w * ppm->n,
			0x000000FF, 0x0000FF00, 0x00FF0000, 0);

	// Convert surface to texture
	texture = SDL_CreateTextureFromSurface(graphic->renderer, image);

	// Free surface
	SDL_FreeSurface(image);

	// clear window
	SDL_RenderClear(graphic->renderer);

	SDL_RenderCopyEx(graphic->renderer, texture, NULL, &(trans->dstrect), 0, NULL, SDL_FLIP_NONE);
	// Free texture
	SDL_DestroyTexture(texture);
	// Render
	SDL_RenderPresent(graphic->renderer);

	log_info("draw_ppm() [Success]");
}


static void	draw_text(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Color color)
{
	SDL_Surface	*surface_message = NULL;
	SDL_Texture	*message = NULL;
	SDL_Rect	rect;

	surface_message = TTF_RenderText_Blended(font, text, color);
	if (surface_message == NULL) {
		log_fatal("%s", TTF_GetError());
		return ;
	}

	// Set rectangle size
	rect.x = x;
	rect.y = y;
	rect.w = surface_message->w;
	rect.h = surface_message->h;

	// Convert as texture
	message = SDL_CreateTextureFromSurface(renderer, surface_message);
	SDL_FreeSurface(surface_message);
	surface_message = NULL;
	if (message == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
		return ;
	}

	SDL_RenderCopy(renderer, message, NULL, &rect);

	// free texture
	SDL_DestroyTexture(message);
	message = NULL;
	log_info("draw_text() [Success]");
}

void	draw_ui(char *book)
{
	SDL_Surface	*image = NULL;
	SDL_Rect	cover = {(WIN_WIDTH / 2) - (350 /2) , (WIN_HEIGHT / 2) - (500 / 2), 350, 500};
	SDL_Color	color = {255, 255, 255, 255};
	int			title_x = ((WIN_WIDTH / 2) - ((CHAR_WIDTH * strlen(book)) / 2));
	char		page_number[10] = {0};

	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, 255);
	SDL_RenderClear(graphic->renderer);

	SDL_SetRenderDrawColor(graphic->renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderDrawRect(graphic->renderer, &cover);

	// Title
	draw_text(graphic->renderer, title_x, 30, book, graphic->ttf->font_large, color);
	// Page number
	draw_text(graphic->renderer, 850, 130, "Pages number :", graphic->ttf->font_medium, color);
	sprintf(page_number, "%d", ebook->total_page);
	draw_text(graphic->renderer, 1100, 130, page_number, graphic->ttf->font_medium, color);

	SDL_RenderPresent(graphic->renderer);

	log_info("draw_ui() [Success]");
}
