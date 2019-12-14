#include "common.h"

extern t_graphic	*graphic;
extern t_transform	*trans;
extern t_ebook		*ebook;
extern t_layout		*layout;
extern t_cbr		*cbr;

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
		if (ebook->layout_orientation == PORTRAIT) {
			SDL_RenderCopyEx(graphic->renderer, texture, NULL, &(layout->cover), 90, NULL, SDL_FLIP_NONE);
		} else {
			SDL_RenderCopyEx(graphic->renderer, texture, NULL, &(layout->cover), 0, NULL, SDL_FLIP_NONE);
		}
	} else {
		SDL_RenderCopyEx(graphic->renderer, texture, NULL, &(layout->page), 0, NULL, SDL_FLIP_NONE);
	}
	// Free texture
	SDL_DestroyTexture(texture);

	log_info("draw_ppm() [Success]");
}

static void	draw_text(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Color color, int angle)
{
	SDL_Surface	*surface_message = NULL;
	SDL_Texture	*message = NULL;
	SDL_Rect	rect = {0};
	SDL_Point	point = {0};

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

	SDL_RenderCopyEx(renderer, message, NULL, &rect, angle, &point, SDL_FLIP_NONE);

	// free texture
	SDL_DestroyTexture(message);
	message = NULL;
	log_info("draw_text() [Success]");
}

void	draw_cover_cbr(char *book)
{
	SDL_Surface	*image = NULL;
	SDL_Texture	*texture = NULL;
	char		cbr_path[PATH_MAX] = {0};

	// get path
	sprintf(cbr_path, EBOOK_PATH"%s", book);

	// save image cover in /tmp
	if (extract_cbr(cbr_path, 0) == false) {
		log_warn("extract_cbr() [Failure]");
		return ;
	}

	image = IMG_Load(cbr->path);
	if (image == NULL) {
		log_warn("%s", IMG_GetError());
		return ;
	}

	texture = SDL_CreateTextureFromSurface(graphic->renderer, image);
	if (texture == NULL) {
		log_warn("%s", IMG_GetError());
		return ;
	}

	SDL_FreeSurface(image);

	// Render cover
	SDL_RenderCopy(graphic->renderer, texture, NULL, &layout->cover);
	SDL_DestroyTexture(texture);

	// Draw rect around cover
	SDL_SetRenderDrawColor(graphic->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(graphic->renderer, &layout->cover);

	remove(cbr->path);

	free(cbr->path);
	cbr->path = NULL;

	log_info("print_cover_cbr() [Success]");
}

bool	load_from_cache(char *book)
{
	SDL_Surface	*image = NULL;
	SDL_Texture	*texture = NULL;
	char		path[PATH_MAX] = {0};

	sprintf(path, "/switch/ebookViewerNX/cache/%s", book);

	if (isFileExist(path) == false) {
		return (false);
	}

	printf("%s\n", path);
	image = IMG_Load(cbr->path);
	if (image == NULL) {
		log_warn("%s", IMG_GetError());
		return (false);
	}
	printf("%s\n", path);

	texture = SDL_CreateTextureFromSurface(graphic->renderer, image);
	if (texture == NULL) {
		log_warn("%s", IMG_GetError());
		return (false);
	}
	printf("%s\n", path);

	SDL_FreeSurface(image);

	// Render cover
	SDL_RenderCopy(graphic->renderer, texture, NULL, &layout->cover);
	SDL_DestroyTexture(texture);

	log_info("load_from_cache() [Success]");
	return (true);
}

void	save_in_cache(fz_pixmap *ppm, char *book)
{
	char	*image = NULL;
	int		len = 0;
	char	path[PATH_MAX] = {0};

	sprintf(path, "/switch/ebookViewerNX/cache/%s", book);

	printf("len = %ld\n", strlen(ppm->samples));
	image = (char *)calloc(sizeof(char), 1000000);
	if (image == NULL) {
		return ;
	}

	len += sprintf(image, "P3\n%d %d\n255\n", ppm->w, ppm->h);
	for (int y = 0; y < ppm->h; ++y)
	{
		unsigned char *p = &ppm->samples[y * ppm->stride];
		for (int x = 0; x < ppm->w; ++x)
		{
			if (x > 0)
				len += sprintf(&image[len], "  ");
			len += sprintf(&image[len], "%3d %3d %3d", p[0], p[1], p[2]);
			p += ppm->n;
		}
		len += sprintf(&image[len], "\n");
		printf("len = %d\n", len);
	}

	int fd = open(path, O_CREAT | O_RDWR, 0777);
	if (fd == -1) {
		free(image);
		image = NULL;
		return ;
	}

	printf("hello\n");
	write(fd, image, len);

	close(fd);
	free(image);
	image = NULL;

	log_info("save_in_cache() [Success]");
}

static bool	draw_cover(char *book)
{
	char	path[PATH_MAX] = {0};

	sprintf(path, "/switch/ebookViewerNX/%s", book);

	if (load_from_cache(book) == false) {
		init_mupdf();

		if (open_ebook(path) == false) {
			return (false);
		}

		get_page_info(0);

		//scale to fit 100%
		trans->zoom = 100 / (trans->bounds.y1 / layout->cover.h);

		// set zoom and rotation
		trans->ctm = fz_scale(trans->zoom / 100, trans->zoom / 100);
		trans->ctm = fz_pre_rotate(trans->ctm, 0);

		if (convert_page_to_ppm(0) == false) {
			return (false);
		}

		draw_ppm(ebook->ppm, COVER);

		save_in_cache(ebook->ppm, book);

		fz_drop_pixmap(ebook->ctx, ebook->ppm);
	}

	// Draw rect around cover
	SDL_SetRenderDrawColor(graphic->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawRect(graphic->renderer, &layout->cover);

	log_info("draw_cover() [Success]");
	return (true);
}

void	draw_title(char *book)
{
	SDL_Color	color = {255, 255, 255, 255};
	char		*title = NULL;
	int			w = 0;
	int			h = 0;

	// remove file extension
	title = strrchr(book, '.');
	if (title != NULL) {
		title[0] = '\0';
	}

	TTF_SizeText(graphic->ttf->font_medium, book, &w, &h);
	if (ebook->layout_orientation == LANDSCAPE) {
		draw_text(graphic->renderer, (WIN_WIDTH / 2) - (w / 2), 80, book, graphic->ttf->font_medium, color, 0);
	} else {
		draw_text(graphic->renderer, 1200, ((WIN_HEIGHT / 2) - (h / 2)) - (w / 2), book, graphic->ttf->font_medium, color, 90);
	}

	// put back file ext
	title[strlen(title)] = '.';
	log_info("draw_title() [Success]");
}

void	draw_line(void)
{
	// draw background bar
	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, 255);
	SDL_RenderFillRect(graphic->renderer, &layout->bar.back_bar);

	// draw green line
	SDL_SetRenderDrawColor(graphic->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(graphic->renderer, &layout->bar.line);

	log_info("draw_line() [Success]");
}

void	draw_app_name(void)
{
	SDL_Color	color = {255, 255, 255, 255};

	// in home menu
	if (ebook->layout_orientation == LANDSCAPE) {
		draw_text(graphic->renderer, layout->app_title.x, layout->app_title.y, APP_NAME, graphic->ttf->font_large, color, 0);
	} else {
		draw_text(graphic->renderer, layout->app_title.x, layout->app_title.y, APP_NAME, graphic->ttf->font_medium, color, 90);
	}

	log_info("draw_app_name() [Success]");
}

void	draw_button(SDL_Rect *rect, char *text, SDL_Color button_color, SDL_Color text_color, int angle)
{
	int	rect_text_w = 0;
	int	rect_text_h = 0;
	int	text_x = 0;
	int	text_y = 0;

	TTF_SizeText(graphic->ttf->font_small, text, &rect_text_w, &rect_text_h);

	/*if (angle == 90) {*/
		/*if (rect->w < rect_text_w) {*/
			/*rect->h = rect_text_w + 8;*/
		/*}*/
		/*if (rect->h < rect_text_h) {*/
			/*rect->w = rect_text_h + 4;*/
		/*}*/
	/*}*/

	SDL_SetRenderDrawColor(graphic->renderer, button_color.r, button_color.g, button_color.b, button_color.a);
	SDL_RenderDrawRect(graphic->renderer, rect);

	if (angle == 90) {
		text_x = rect->x + ((rect->w) - ((rect->w - rect_text_h) / 2));
		text_y = rect->y + ((rect->h - rect_text_w) / 2);
	} else {
		text_x = ((rect->w / 2) - (rect_text_w / 2)) + rect->x;
		text_y = ((rect->h / 2) - (rect_text_h / 2)) + rect->y;
	}

	draw_text(graphic->renderer, text_x, text_y, text, graphic->ttf->font_small, text_color, angle);

	log_info("draw_button() [Success]");
}

static void	draw_exit_button(void)
{
	SDL_Color	background_color = {0, 255, 0, 255};
	SDL_Color	text_color = {255, 255, 255, 255};

	if (ebook->layout_orientation == PORTRAIT) {
		draw_button(&layout->exit_button, "Exit", background_color, text_color, 90);
	} else {
		draw_button(&layout->exit_button, "Exit", background_color, text_color, 0);
	}

	log_info("draw_exit_button() [Success]");
}

static void	draw_help_button(void)
{
	SDL_Color background_color = {0, 255, 0, 255};
	SDL_Color text_color = {255, 255, 255, 255};

	if (ebook->layout_orientation == PORTRAIT) {
		draw_button(&layout->help_button, "Help", background_color, text_color, 90);
	} else {
		draw_button(&layout->help_button, "Help", background_color, text_color, 0);
	}

	log_info("draw_help_button() [Success]");
}

static void	draw_rotate_button(void)
{
	SDL_Color background_color = {0, 255, 0, 255};
	SDL_Color text_color = {255, 255, 255, 255};

	if (ebook->layout_orientation == PORTRAIT) {
		draw_button(&layout->rotate_button, "Rotate", background_color, text_color, 90);
	} else {
		draw_button(&layout->rotate_button, "Rotate", background_color, text_color, 0);
	}

	log_info("draw_rotate_button() [Success]");
}

void	draw_bar(void)
{
	draw_line();
	if (ebook->read_mode == false) {
		draw_app_name();
	} else {
		draw_page_number(DEFAULT);
		draw_rotate_button();
	}
	draw_exit_button();
	draw_help_button();

	log_info("draw_bar() [Success]");
}

void	draw_page_number(int type)
{
	SDL_Color	color = {255, 255, 255, 255};
	char		page_number[20] = {0};
	int			x = 0;
	int			w = 0;
	int			h = 0;
	float		percentage = 0;

	// Draw page number
	if (type == CBR) {
		sprintf(page_number, "%d/%d", ebook->last_page + 1, cbr->total_page);
	} else {
		sprintf(page_number, "%d/%d", ebook->last_page + 1, ebook->total_page);
	}

	TTF_SizeText(graphic->ttf->font_medium, page_number, &w, &h);
	x = (WIN_WIDTH / 2) - (w / 2);

	if (ebook->read_mode == false && ebook->layout_orientation == LANDSCAPE) {
		draw_text(graphic->renderer, x, 660, page_number, graphic->ttf->font_medium, color, 0);
	} else if (ebook->layout_orientation == LANDSCAPE && ebook->read_mode == true) {
		draw_text(graphic->renderer, x, 10, page_number, graphic->ttf->font_medium, color, 0);
	} else if (ebook->layout_orientation == PORTRAIT && ebook->read_mode == true) {
		draw_text(graphic->renderer, 1280, (WIN_HEIGHT / 2) - (w  /2), page_number, graphic->ttf->font_medium, color, 90);
	}

	// Background
	SDL_SetRenderDrawColor(graphic->renderer, 0, 150, 0, 255);
	SDL_RenderFillRect(graphic->renderer, &layout->progress_bar);

	//Foreground
	if (ebook->layout_orientation == PORTRAIT && ebook->read_mode == true) {
		percentage = round(((float)layout->progress_bar.h / (float)ebook->total_page) * (float)ebook->last_page);
		layout->progress_bar.h = (int)percentage;
	} else {
		percentage = round(((float)layout->progress_bar.w / (float)ebook->total_page) * (float)ebook->last_page);
		layout->progress_bar.w = (int)percentage;
	}

	SDL_SetRenderDrawColor(graphic->renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(graphic->renderer, &layout->progress_bar);

	log_info("draw_page_number() [Success]");
}

void	print_help(void)
{
	SDL_Surface	*image = NULL;

	if (ebook->read_mode == false) {
		image = IMG_Load("romfs:/help_button_landscape.png");
	} else if (ebook->read_mode == true && ebook->layout_orientation == LANDSCAPE) {
		image = IMG_Load("romfs:/help_read_landscape.png");
	} else if (ebook->read_mode == true && ebook->layout_orientation == PORTRAIT) {
		image = IMG_Load("romfs:/help_read_portrait.png");
	}

	if (image == NULL) {
		log_warn("%s", IMG_GetError());
	}
	SDL_Texture	*texture = SDL_CreateTextureFromSurface(graphic->renderer, image);
	if (texture == NULL) {
		log_warn("%s", SDL_GetError());
	}
	SDL_RenderCopy(graphic->renderer, texture, NULL, NULL);
	SDL_DestroyTexture(texture);

	log_info("print_help() [Success]");
}

void	draw_home_menu(char *book)
{
	char	*ext = NULL;

	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, 255);
	SDL_RenderClear(graphic->renderer);

	// Draw bar
	draw_bar();

	// Draw title
	draw_title(book);

	// Draw Cover
	ext = get_file_extension(book);
	if (ext == NULL) {
		return ;
	}

	if (!strcmp(ext, ".cbr")) {
		draw_cover_cbr(book);
		draw_page_number(CBR);
	} else {
		if (draw_cover(book) == false) {
			deinit_mupdf();
			return ;
		}
		// Draw Page number
		count_page_number();
		draw_page_number(DEFAULT);

		deinit_mupdf();
	}

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

// TODO: refactor this func
void	draw_message_box(char *msg)
{
	SDL_Color	color = {255, 255, 255, 255};
	SDL_Rect	rect = {
		(WIN_WIDTH / 2) - 300,
		(WIN_HEIGHT / 2) - 150,
		600,
		300
	};
	int			w = 0;
	int			h = 0;

	SDL_SetRenderDrawColor(graphic->renderer, 60, 60, 60, SDL_ALPHA_OPAQUE);
	/*SDL_RenderClear(graphic->renderer);*/

	SDL_RenderFillRect(graphic->renderer, &rect);
	TTF_SizeText(graphic->ttf->font_small, msg, &w, &h);

	draw_text(graphic->renderer, (WIN_WIDTH / 2) - (w / 2), (WIN_HEIGHT / 2) - (h / 2), msg, graphic->ttf->font_small, color, 0);

	while (appletMainLoop()) {
		hidScanInput();

		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_PLUS) {
			break ;
		}

		SDL_RenderPresent(graphic->renderer);
	}
	
}

void	draw_error(char *msg)
{
	SDL_Color	color = {255, 255, 255, 255};
	int			w = 0;
	int			h = 0;

	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(graphic->renderer);

	TTF_SizeText(graphic->ttf->font_medium, msg, &w, &h);

	draw_text(graphic->renderer, (WIN_WIDTH / 2) - (w / 2), (WIN_HEIGHT / 2) - (h / 2), msg, graphic->ttf->font_medium, color, 0);

	while (appletMainLoop()) {
		hidScanInput();

		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		if (kDown & KEY_PLUS) {
			break ;
		}

		SDL_RenderPresent(graphic->renderer);
	}
}
