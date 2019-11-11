#include "common.h"

extern t_graphic	*graphic;
extern t_transform	*trans;
extern t_ebook		*ebook;
extern t_layout		*layout;

bool	init_ttf(void)
{
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

	log_info("deinit_ttf() [Success]");
}

bool	init_graphic(void)
{
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

	SDL_Quit();

	log_info("deinit_graphic() [Success]");
}

void	draw_ppm(fz_pixmap *ppm, bool cover)
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

	if (cover == 0) {
		SDL_RenderCopyEx(graphic->renderer, texture, NULL, &(layout->cover_pos), 0, NULL, SDL_FLIP_NONE);
	} else {
		SDL_RenderCopyEx(graphic->renderer, texture, NULL, &(trans->dstrect), 0, NULL, SDL_FLIP_NONE);
	}
	// Free texture
	SDL_DestroyTexture(texture);

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

static bool	draw_cover(char *book)
{
	char		path[PATH_MAX] = {0};
	SDL_Rect	rect = {
		layout->cover_pos.x,
		layout->cover_pos.y,
		layout->cover_pos.w,
		layout->cover_pos.h
	};

	sprintf(path, "/switch/ebookReaderNX/%s", book);

	init_mupdf();

	if (open_ebook(path) == false) {
		return (false);
	}

	get_page_info(0);

	//scale to fit 100%
	trans->zoom = 100 / (trans->bounds.y1 / 500);

	// set zoom and rotation
	trans->ctm = fz_scale(trans->zoom / 100, trans->zoom / 100);
	trans->ctm = fz_pre_rotate(trans->ctm, 0);

	if (convert_page_to_ppm(0) == false) {
		return (false);
	}

	draw_ppm(ebook->ppm, COVER);

	fz_drop_pixmap(ebook->ctx, ebook->ppm);

	// Draw rect around cover
	SDL_SetRenderDrawColor(graphic->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(graphic->renderer, &rect);
	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, SDL_ALPHA_OPAQUE);

	log_info("draw_cover() [Success]");
	return (true);
}

void	draw_title(char *book)
{
	char		*title = NULL;
	int			title_x = 0;
	SDL_Color	color = {255, 255, 255, 255};

	// remove file extension
	title = strrchr(book, '.');
	if (title != NULL) {
		title[0] = '\0';
		title_x = ((WIN_WIDTH / 2) - ((CHAR_WIDTH_MEDIUM * strlen(book)) / 2));
	}

	draw_text(graphic->renderer, title_x, 80, book, graphic->ttf->font_medium, color);

	title[strlen(title)] = '.';
	log_info("draw_title() [Success]");
}

void	draw_line(void)
{
	#define		POINTS_COUNT	4
	const		SDL_Point points[POINTS_COUNT] = {
		{40, 55},
		{1240, 55},
		{1240, 56},
		{40, 56}
	};

	SDL_SetRenderDrawColor(graphic->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawLines(graphic->renderer, points, POINTS_COUNT);
	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, SDL_ALPHA_OPAQUE);
	log_info("draw_line() [Success]");
}

void	draw_app_name(void)
{
	SDL_Color	color = {255, 255, 255, 255};

	draw_text(graphic->renderer, (WIN_WIDTH / 2) - ((CHAR_WIDTH_LARGE * sizeof(APP_NAME)) / 2), 5, APP_NAME, graphic->ttf->font_large, color);
	log_info("draw_app_name() [Success]");
}

void	draw_page_number(void)
{
	int			progression_x = 0;
	char		page_number[20] = {0};
	SDL_Color	color = {255, 255, 255, 255};

	count_page_number();

	sprintf(page_number, "%d/%d", ebook->last_page + 1, ebook->total_page);
	progression_x = ((WIN_WIDTH / 2) - ((CHAR_WIDTH_MEDIUM * strlen(page_number)) / 2));
	draw_text(graphic->renderer, progression_x, 660, page_number, graphic->ttf->font_medium, color);
	log_info("draw_page_number() [Success]");
}

void	draw_ui(char *book)
{
	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, 255);
	SDL_RenderClear(graphic->renderer);

	// Draw app name
	draw_app_name();

	// Draw line
	draw_line();

	// Draw title
	draw_title(book);

	// Draw Cover
	if (draw_cover(book) == false) {
		deinit_mupdf();
		return ;
	}

	// Draw Page number
	draw_page_number();

	deinit_mupdf();
	log_info("draw_ui() [Success]");
}
