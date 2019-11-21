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

	SDL_SetRenderDrawBlendMode(graphic->renderer, SDL_BLENDMODE_BLEND);

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

static void	draw_text(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Color color, int angle)
{
	SDL_Surface	*surface_message = NULL;
	SDL_Texture	*message = NULL;
	SDL_Rect	rect;

	surface_message = TTF_RenderUTF8_Blended(font, text, color);
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

	SDL_RenderCopyEx(renderer, message, NULL, &rect, angle, NULL, SDL_FLIP_NONE);

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

	sprintf(path, "/switch/ebookViewerNX/%s", book);

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

	draw_text(graphic->renderer, title_x, 80, book, graphic->ttf->font_medium, color, 0);

	title[strlen(title)] = '.';
	log_info("draw_title() [Success]");
}

void	draw_line(void)
{
	SDL_Rect	rect = {0};
	
	if (ebook->read_mode == false || (ebook->layout_orientation == LANDSCAPE && ebook->read_mode == true)) {
		layout->line.x = WIN_WIDTH / 32;
		layout->line.y = WIN_HEIGHT / 12;
		layout->line.w = WIN_WIDTH - (layout->line.x * 2);
		layout->line.h = 2;

		rect.x = 0;
		rect.h = layout->line.y;

		// coord to touch bar
		layout->bar.x = 0;
		layout->bar.y = 0;
		layout->bar.w = WIN_WIDTH;
		layout->bar.h = layout->line.y;
	} else if (ebook->layout_orientation == PORTRAIT && ebook->read_mode == true) {
		layout->line.x = WIN_WIDTH - (WIN_WIDTH / 28);
		layout->line.y = WIN_HEIGHT / 20;
		layout->line.w = 2;
		layout->line.h = WIN_HEIGHT - (layout->line.y * 2);

		rect.x = layout->line.x;
		rect.h = WIN_HEIGHT;

		// coord to touch bar
		layout->bar.x = layout->line.x;
		layout->bar.y = 0;
		layout->bar.w = WIN_WIDTH - layout->line.x;
		layout->bar.h = WIN_HEIGHT;
	}

	rect.y = 0;
	rect.w = WIN_WIDTH;

	// draw background bar
	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, 255);
	SDL_RenderFillRect(graphic->renderer, &rect);

	SDL_SetRenderDrawColor(graphic->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(graphic->renderer, &layout->line);

	log_info("draw_line() [Success]");
}

void	draw_app_name(void)
{
	SDL_Color	color = {255, 255, 255, 255};
	int			w = 0;
	int			h = 0;

	// in home menu
	if (ebook->read_mode == false || (ebook->layout_orientation == LANDSCAPE && ebook->read_mode == true)) {
		TTF_SizeText(graphic->ttf->font_large, APP_NAME, &w, &h);
		layout->app_title.x = (WIN_WIDTH / 2) - (w / 2);
		layout->app_title.y = WIN_HEIGHT / 90;

		draw_text(graphic->renderer, layout->app_title.x, layout->app_title.y, APP_NAME, graphic->ttf->font_large, color, 0);
	} else if (ebook->read_mode == true && ebook->layout_orientation == PORTRAIT) {
		TTF_SizeText(graphic->ttf->font_medium, APP_NAME, &w, &h);
		layout->app_title.x = 1090;
		layout->app_title.y = (WIN_HEIGHT / 2);

		draw_text(graphic->renderer, layout->app_title.x, layout->app_title.y, APP_NAME, graphic->ttf->font_medium, color, 90);
	}

	log_info("draw_app_name() [Success]");
}

void	draw_button(SDL_Rect rect, char *text, uint8_t prop, SDL_Color button_color, SDL_Color text_color, int angle)
{
	int	rect_text_w = 0;
	int	rect_text_y = 0;
	int	text_x = 0;
	int	text_y = 0;

	TTF_SizeText(graphic->ttf->font_small, text, &rect_text_w, &rect_text_y);

	if (angle == 90 && rect.w < rect_text_w) {
		if (rect.w < rect_text_w) {
			rect.h = rect_text_w + 8;
		}
		if (rect.h < rect_text_y) {
			rect.w = rect_text_y + 4;
		}
	}

	SDL_SetRenderDrawColor(graphic->renderer, button_color.r, button_color.g, button_color.b, button_color.a);
	SDL_RenderDrawRect(graphic->renderer, &rect);

	text_x = ((rect.w / 2) - (rect_text_w / 2)) + rect.x;
	text_y = ((rect.h / 2) - (rect_text_y / 2)) + rect.y;

	draw_text(graphic->renderer, text_x, text_y, text, graphic->ttf->font_small, text_color, angle);

	log_info("draw_button() [Success]");
}

static void	draw_exit_button(void)
{
	SDL_Color	background_color = {0, 255, 0, 255};
	SDL_Color	text_color = {255, 255, 255, 255};

	if (ebook->layout_orientation == PORTRAIT && ebook->read_mode == true) {
		layout->exit_home.x = 1240;
		layout->exit_home.y = 615;
		layout->exit_home.w = 34;
		layout->exit_home.h = 58;

		draw_button(layout->exit_home, "Exit", 0, background_color, text_color, 90);
	} else {
		layout->exit_home.w = WIN_WIDTH / 14;
		layout->exit_home.h = layout->line.y / 1.30;
		layout->exit_home.x = 0.8984375 * WIN_WIDTH;
		layout->exit_home.y = (layout->line.y - layout->exit_home.h) / 2;

		draw_button(layout->exit_home, "Exit", 0, background_color, text_color, 0);
	}

	log_info("draw_exit_button() [Success]");
}

static void	draw_help_button(void)
{
	SDL_Color background_color = {0, 255, 0, 255};
	SDL_Color text_color = {255, 255, 255, 255};

	if (ebook->layout_orientation == PORTRAIT && ebook->read_mode == true) {
		layout->help_home.x = 1240;
		layout->help_home.y = 550;
		layout->help_home.w = 34;
		layout->help_home.h = 58;

		draw_button(layout->help_home, "Help", 0, background_color, text_color, 90);
	} else {
		layout->help_home.w = WIN_WIDTH / 14;
		layout->help_home.h = layout->line.y / 1.30;
		layout->help_home.x = 0.8203125 * WIN_WIDTH;
		layout->help_home.y = (layout->line.y - layout->help_home.h) / 2;

		draw_button(layout->help_home, "Help", 0, background_color, text_color, 0);
	}

	log_info("draw_help_button() [Success]");
}

static void	draw_rotate_button(void)
{
	SDL_Color background_color = {0, 255, 0, 255};
	SDL_Color text_color = {255, 255, 255, 255};

	if (ebook->layout_orientation == PORTRAIT && ebook->read_mode == true) {
		layout->rotate_button.x = 1240;
		layout->rotate_button.y = 120;
		layout->rotate_button.w = 34;
		layout->rotate_button.h = 58;

		draw_button(layout->rotate_button, "Rotate", 0, background_color, text_color, 90);
	} else {
		layout->rotate_button.w = WIN_WIDTH / 14;
		layout->rotate_button.h = layout->line.y / 1.30;
		layout->rotate_button.x = 0.739125 * WIN_WIDTH;
		layout->rotate_button.y = (layout->line.y - layout->rotate_button.h) / 2;

		draw_button(layout->rotate_button, "Rotate", 0, background_color, text_color, 0);
	}

	log_info("draw_rotate_button() [Success]");
}

void	draw_bar(void)
{
	draw_line();
	draw_app_name();
	draw_exit_button();
	draw_help_button();
	if (ebook->read_mode == true) {
		draw_rotate_button();
	}

	log_info("draw_bar() [Success]");
}

void	draw_page_number(void)
{
	int			progression_x = 0;
	char		page_number[20] = {0};
	SDL_Color	color = {255, 255, 255, 255};

	count_page_number();

	sprintf(page_number, "%d/%d", ebook->last_page + 1, ebook->total_page);
	progression_x = ((WIN_WIDTH / 2) - ((CHAR_WIDTH_MEDIUM * strlen(page_number)) / 2));
	draw_text(graphic->renderer, progression_x, 660, page_number, graphic->ttf->font_medium, color, 0);
	log_info("draw_page_number() [Success]");
}

void	print_help(void)
{
	SDL_Rect	rect = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	SDL_Color	color = {255, 255, 255, 255};
	int			w, h;

	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, 200);

	// draw background
	SDL_RenderFillRect(graphic->renderer, &rect);

	SDL_SetRenderDrawColor(graphic->renderer, 255, 255, 255, 255);
	// draw left lines
	SDL_RenderDrawLine(graphic->renderer, layout->cover.x -1, layout->line.y, layout->cover.x -1, WIN_HEIGHT);
	SDL_RenderDrawLine(graphic->renderer, layout->cover.x, layout->line.y, layout->cover.x, WIN_HEIGHT);
	// draw right lines
	SDL_RenderDrawLine(graphic->renderer, layout->cover.x + layout->cover.w + 1, layout->line.y, layout->cover.x + layout->cover.w + 1, WIN_HEIGHT);
	SDL_RenderDrawLine(graphic->renderer, layout->cover.x + layout->cover.w, layout->line.y, layout->cover.x + layout->cover.w, WIN_HEIGHT);
	// Draw Horizontal line
	SDL_RenderDrawLine(graphic->renderer, 0, layout->line.y, WIN_WIDTH, layout->line.y);
	SDL_RenderDrawLine(graphic->renderer, 0, layout->line.y + 1, WIN_WIDTH, layout->line.y + 1);

	// draw prev book
	TTF_SizeText(graphic->ttf->font_medium, "Previous book", &w, &h);
	draw_text(graphic->renderer, (layout->cover.x / 2) - (w / 2), (WIN_HEIGHT / 2) - (h / 2), "Previous book", graphic->ttf->font_medium, color, 0);

	// draw next book
	TTF_SizeText(graphic->ttf->font_medium, "Next book", &w, &h);
	draw_text(graphic->renderer, layout->cover.x + layout->cover.w + ((layout->cover.x / 2) - (w / 2)), (WIN_HEIGHT / 2) - (h / 2), "Next book", graphic->ttf->font_medium, color, 0);
	// draw launch book
	TTF_SizeText(graphic->ttf->font_medium, "Launch book", &w, &h);
	draw_text(graphic->renderer, (layout->cover.x + (layout->cover.w / 2)) - (w / 2), (WIN_HEIGHT / 2) - (h / 2), "Open book", graphic->ttf->font_medium, color, 0);

	log_info("print_help() [Success]");
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

	deinit_mupdf();
	log_info("draw_home_menu() [Success]");
}

void	draw_loading(void)
{
	SDL_Color	color = {255, 255, 255, 255};

	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(graphic->renderer);

	if (ebook->layout_orientation == PORTRAIT && ebook->read_mode == true) {
		draw_text(graphic->renderer, -50, 600, "Loading ...", graphic->ttf->font_medium, color, 90);
	} else {
		draw_text(graphic->renderer, 1100, 680, "Loading ...", graphic->ttf->font_medium, color, 0);
	}

	SDL_RenderPresent(graphic->renderer);

	log_info("draw_loading() [Success]");
}
