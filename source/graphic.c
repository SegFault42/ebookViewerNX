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
		SDL_RenderCopyEx(graphic->renderer, texture, NULL, &(layout->cover), 0, NULL, SDL_FLIP_NONE);
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
		layout->cover.x,
		layout->cover.y,
		layout->cover.w,
		layout->cover.h
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

void	draw_line()
{
	SDL_SetRenderDrawColor(graphic->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	// if reading and landscape
	if (/*ebook->layout_orientation == LANDSCAPE &&*/ ebook->read_mode == false) {
		SDL_RenderFillRect(graphic->renderer, &layout->line);
	}

	log_info("draw_line() [Success]");
}

void	draw_app_name(void)
{
	SDL_Color	color = {255, 255, 255, 255};

	// if reading and landscape 
	if (/*ebook->layout_orientation == LANDSCAPE &&*/ ebook->read_mode == false) {
		draw_text(graphic->renderer, layout->app_title.x, layout->app_title.y, APP_NAME, graphic->ttf->font_large, color);
	}

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

void	draw_button(SDL_Rect rect, char *text, uint8_t prop, SDL_Color button_color, SDL_Color text_color)
{
	int	rect_text_w = 0;
	int	rect_text_y = 0;
	int	text_x = 0;
	int	text_y = 0;

	TTF_SizeText(graphic->ttf->font_small, text, &rect_text_w, &rect_text_y);

	SDL_SetRenderDrawColor(graphic->renderer, button_color.r, button_color.g, button_color.b, button_color.a);
	SDL_RenderDrawRect(graphic->renderer, &rect);

	text_x = ((rect.w / 2) - (rect_text_w / 2)) + rect.x;
	text_y = ((rect.h / 2) - (rect_text_y / 2)) + rect.y;

	draw_text(graphic->renderer, text_x, text_y, text, graphic->ttf->font_small, text_color);
}

static void	draw_exit_button(void)
{
	layout->exit_home.w = WIN_WIDTH / 14;
	layout->exit_home.h = layout->line.y / 1.30;
	layout->exit_home.x = 0.8984375 * WIN_WIDTH;
	layout->exit_home.y = (layout->line.y - layout->exit_home.h) / 2;

	SDL_Color background_color = {0, 255, 0, 255};
	SDL_Color text_color = {255, 255, 255, 255};

	draw_button(layout->exit_home, "Exit", 0, background_color, text_color);
}

static void	draw_help_button(void)
{
	layout->help_home.w = WIN_WIDTH / 14;
	layout->help_home.h = layout->line.y / 1.30;
	layout->help_home.x = 0.8203125 * WIN_WIDTH;
	layout->help_home.y = (layout->line.y - layout->help_home.h) / 2;

	SDL_Color background_color = {0, 255, 0, 255};
	SDL_Color text_color = {255, 255, 255, 255};

	draw_button(layout->help_home, "Help", 0, background_color, text_color);
}

void	draw_bar(void)
{
	// Draw app name
	draw_app_name();

	// Draw line
	draw_line();
}
void	draw_home_menu(char *book)
{
	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, 255);
	SDL_RenderClear(graphic->renderer);

	// Draw bar
	draw_bar();

	// Draw title
	draw_title(book);

	// Draw Cover
	if (draw_cover(book) == false) {
		deinit_mupdf();
		return ;
	}

	// Draw Page number
	draw_page_number();

	// Draw exit button
	draw_exit_button();

	draw_help_button();

	deinit_mupdf();
	log_info("draw_home_menu() [Success]");
}
