#include "common.h"

extern t_graphic	*graphic;
extern t_ebook		*ebook;
extern t_transform	*trans;
extern t_controller	*controller;
extern t_layout		*layout;
extern t_cbr		*cbr;

bool	init_mupdf(void)
{
	/* Create a context to hold the exception stack and various caches. */
	ebook->ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ebook->ctx) {
		log_fatal("cannot create mupdf context");
		return (false);
	}

	/* Register the default file types to handle. */
	fz_try(ebook->ctx) {
		fz_register_document_handlers(ebook->ctx);
	}
	fz_catch(ebook->ctx) {
		log_fatal("cannot register document handlers: %s\n", fz_caught_message(ebook->ctx));
		fz_drop_context(ebook->ctx);
		return (false);
	}

	log_info("init_mupdf() [Success]");
	return (true);
}

void	deinit_mupdf(void)
{
	fz_drop_document(ebook->ctx, ebook->doc);
	fz_drop_context(ebook->ctx);
	log_info("deinit_mupdf() [Success]");
}

bool	open_ebook(char *path)
{
	/* Open the document. */
	fz_try(ebook->ctx) {
		ebook->doc = fz_open_document(ebook->ctx, path);
	}
	fz_catch(ebook->ctx) {
		log_fatal("open_ebook(): %s\n", fz_caught_message(ebook->ctx));
		return (false);
	}

	log_info("open_ebook() [Success]");
	return (true);
}

bool	count_page_number(void)
{
	/* Count the number of pages. */
	fz_try(ebook->ctx) {
		ebook->total_page = fz_count_pages(ebook->ctx, ebook->doc);
	}
	fz_catch(ebook->ctx) {
		log_fatal("cannot count number of pages: %s\n", fz_caught_message(ebook->ctx));
		return (false);
	}

	log_info("count_page_number() [Success]");
	return (true);
}

bool	convert_page_to_ppm(int current_page)
{
	/* Render page to an RGB pixmap. */
	fz_try(ebook->ctx)
		ebook->ppm = fz_new_pixmap_from_page_number(ebook->ctx, ebook->doc, current_page, trans->ctm, fz_device_rgb(ebook->ctx), 0);
	fz_catch(ebook->ctx) {
		log_fatal("cannot render page: %s\n", fz_caught_message(ebook->ctx));
		return (false);
	}

	log_info("convert_page_to_ppm() [Success]");
	return (true);
}

void	get_page_info(int current_page)
{
	// get size of page
	trans->page = fz_load_page(ebook->ctx, ebook->doc, current_page);
	trans->bounds = fz_bound_page(ebook->ctx, trans->page);
	fz_drop_page(ebook->ctx, trans->page);

	log_info("get_page_info() [Success]");
}

static bool	render_page(char *book, int current_page)
{
	char	*path = NULL;

	path = (char *)calloc(sizeof(char) , strlen(EBOOK_PATH) + strlen(book) + 1);
	if (path == NULL) {
		log_fatal("calloc [Failure]");
		return (false);
	}

	strcat(path, EBOOK_PATH);
	strcat(path, book);

	init_mupdf();
	if (open_ebook(path) == false) {
		deinit_mupdf();
		free(path);
		path = NULL;
		return (false);
	}

	free(path);
	path = NULL;

	// check out of range index
	if (ebook->total_page < 0 || current_page >= ebook->total_page) {
		log_fatal("page number out of range: %d (page count %d)\n", ebook->total_page + 1, current_page);
		deinit_mupdf();
		return (false);
	}

	get_page_info(current_page);

	if (ebook->layout_orientation == PORTRAIT) {
		portrait_default();
	} else if (ebook->layout_orientation == LANDSCAPE) {
		landscape_default();
	}

	if (convert_page_to_ppm(current_page) == false) {
		deinit_mupdf();
		return (false);
	}

	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(graphic->renderer);
	draw_ppm(ebook->ppm, READ);

	fz_drop_pixmap(ebook->ctx, ebook->ppm);
	deinit_mupdf();

	log_info("render_page() [Success]");
	return (true);
}

void	save_last_page(char *book, int current_page)
{
	int		fd = 0;
	int		fd_tmp = 0;
	char	*line = NULL;
	bool	new_book = true;
	char	*token = NULL;
	char	*tmp = NULL;

	ebook->last_page = current_page;

	fd = open(CONFIG_PATH, O_RDONLY);
	if (fd == -1) {
		log_warn("open : %s", strerror(errno));
		return ;
	}

	fd_tmp = open(EBOOK_PATH"tmp", O_CREAT | O_RDWR | O_TRUNC, 0777);
	if (fd_tmp == -1) {
		log_warn("open : %s", strerror(errno));
		close(fd);
		return ;
	}

	while (get_next_line(fd, &line) > 0) {
		tmp = strdup(line);
		token = strtok(tmp, "=");

		if (!strcmp(token, book)) {
			dprintf(fd_tmp, "%s=%d\n", book, ebook->last_page);
			new_book = false;
		} else {
			dprintf(fd_tmp, "%s\n", line);
		}

		free(tmp);
		free(line);
		tmp = NULL;
		line = NULL;
	}
	if (new_book == true) {
		dprintf(fd_tmp, "%s=%d\n", book, ebook->last_page);
	}

	close(fd);
	close(fd_tmp);

	if (remove(CONFIG_PATH) == -1) {
		log_warn("%s", strerror(errno));
	} else {
		if (rename(EBOOK_PATH"tmp", CONFIG_PATH) == -1) {
			log_warn("%s", strerror(errno));
		}
	}

	log_info("save_last_page() [Success]");
}

void	render_cbr_page(char *book, int last_page)
{
	SDL_Surface	*image = NULL;
	SDL_Texture	*texture = NULL;
	char		cbr_path[PATH_MAX] = {0};
	SDL_Rect	dstrect = {
		(WIN_WIDTH / 2) - (COVER_WIDTH / 2),
		(WIN_HEIGHT / 2) - (COVER_HEIGHT / 2),
		COVER_WIDTH,
		COVER_HEIGHT
	};

	// get path
	sprintf(cbr_path, EBOOK_PATH"%s", book);

	if (extract_cbr(cbr_path, last_page) == false) {
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

	SDL_SetRenderDrawColor(graphic->renderer, 40, 40, 40, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(graphic->renderer);

	// Render cover
	SDL_RenderCopy(graphic->renderer, texture, NULL, &dstrect);
	SDL_DestroyTexture(texture);

	remove(cbr->path);

	free(cbr->path);
	cbr->path = NULL;
}

void	ebook_reader(char *book)
{
	bool	refresh = true;
	bool	help = false;

	ebook->read_mode = true;
	while (appletMainLoop()) {
		hidScanInput();

		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		touchPosition touch = {0};

		hidTouchRead(&touch, 0);

		if (kDown & controller->quit || (layout->show_bar == true && touch_button(touch, e_exit) == true)) {
			ebook->read_mode = false;
			break;
		} else if (kDown & controller->help || (layout->show_bar == true && touch_button(touch, e_help) == true)) {
			help = help == true ? false : true;
			refresh = true;
		} else if (kDown & controller->layout || (layout->show_bar == true && touch_button(touch, e_rotate) == true)) {
			ebook->layout_orientation = !ebook->layout_orientation;
			refresh = true;
			help = false;
		} else if (kDown & KEY_A || (touch_button(touch, e_bar) == true)) {
			layout->show_bar = !layout->show_bar;
			refresh = true;
			help = false;
		} else if (kDown & controller->next_page || touch_next_page_read(touch)) {
			ebook->last_page++;
			refresh = true;
			help = false;
		} else if (kDown & controller->prev_page || touch_prev_page_read(touch)) {
			ebook->last_page--;
			refresh = true;
			help = false;
		} else if (kDown & controller->next_multiple_page) {
			ebook->last_page += 10;
			refresh = true;
			help = false;
		} else if (kDown & controller->prev_multiple_page) {
			ebook->last_page -= 10;
			refresh = true;
			help = false;
		}

		// Overflow
		if (ebook->last_page >= ebook->total_page) {
			ebook->last_page = 0;
		}
		if (ebook->last_page < 0) {
			ebook->last_page = ebook->total_page -1;
		}

		// printing
		if (refresh == true) {
			/*draw_loading();*/
			char *ext = get_file_extension(book);
			if (!strcmp(ext, ".cbr")) {
				render_cbr_page(book, ebook->last_page);
			} else {
				if (render_page(book, ebook->last_page) == false) {
					break ;
				}
			}
			if (layout->show_bar == true) {
				draw_bar();
			}
			if (help == true) {
				print_help();
			}
			SDL_RenderPresent(graphic->renderer);
			save_last_page(book, ebook->last_page);
			refresh = false;
		}
	}

	log_info("ebook_reader() [Success]");
}
