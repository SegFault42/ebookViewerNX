#include "common.h"

extern t_graphic	*graphic;
extern t_ebook		*ebook;
extern t_transform	*trans;

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

static void	render_page(char *book, int current_page)
{
	char	*path = NULL;

	path = (char *)calloc(sizeof(char) , strlen("/switch/ebookReaderNX/") + strlen(book) + 1);
	if (path == NULL) {
		log_fatal("calloc [Failure]");
		return ;
	}

	strcat(path, "/switch/ebookReaderNX/");
	strcat(path, book);

	init_mupdf();
	if (open_ebook(path) == false) {
		deinit_mupdf();
		free(path);
		path = NULL;
		return ;
	}

	free(path);
	path = NULL;

	if (count_page_number() == false) {
		deinit_mupdf();
		return ;
	}
	// check out of range index
	if (ebook->total_page < 0 || current_page >= ebook->total_page) {
		log_fatal("page number out of range: %d (page count %d)\n", ebook->total_page + 1, current_page);
		deinit_mupdf();
		return ;
	}

	get_page_info(current_page);

	if (ebook->layout_orientation == PORTRAIT) {
		portrait_default();
	}
	else if (ebook->layout_orientation == LANDSCAPE) {
		landscape_default();
	}

	if (convert_page_to_ppm(current_page) == false) {
		deinit_mupdf();
		return ;
	}

	SDL_RenderClear(graphic->renderer);
	draw_ppm(ebook->ppm);

	fz_drop_pixmap(ebook->ctx, ebook->ppm);
	printf("%d/%d\n", current_page+1, ebook->total_page);
	deinit_mupdf();

	log_info("render_page() [Success]");
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

	fd_tmp = open("/tmp", O_CREAT | O_RDWR | O_TRUNC, 0777);
	if (fd_tmp == -1) {
		log_warn("open : %s", strerror(errno));
		return ;
	}

	while (get_next_line(fd, &line) > 0) {
		tmp = strdup(line);
		token = strtok(tmp, "=");
		if (!strcmp(token, book)) {
			dprintf(fd_tmp, "%s=%d\n", token, ebook->last_page);
			new_book = false;
		} else {
			dprintf(fd_tmp, "%s\n", line);
		}

		free(tmp);
		tmp = NULL;
		free(line);
		line = NULL;
	}
	if (new_book == true) {
		dprintf(fd_tmp, "%s=%d\n", book, ebook->last_page);
	}
	free(line);
	line = NULL;

	close(fd);
	close(fd_tmp);

	if (remove(CONFIG_PATH) == -1) {
		log_warn("%s", strerror(errno));
	}
	if (rename("/tmp", CONFIG_PATH) == -1) {
		log_warn("%s", strerror(errno));
	}

	log_info("save_last_page() [Success]");
}

void	load_last_page(char *book)
{
	int		fd = 0;
	char	*line = NULL;
	char	*token = NULL;
	char	*tmp = NULL;

	fd = open(CONFIG_PATH, O_RDONLY);
	if (fd == -1) {
		log_warn("open : %s", strerror(errno));
		return ;
	}

	while (get_next_line(fd, &line) > 0) {
		tmp = strdup(line);
		token = strtok(tmp, "=");
		if (!strcmp(token, book)) {
			token = strtok(NULL, "=");
			ebook->last_page = atoi(token);
			break ;
		}

		free(tmp);
		tmp = NULL;
		free(line);
		line = NULL;
	}
	free(line);
	line = NULL;

	close(fd);
	log_info("load_last_page() [Success]");
}

void	ebook_reader(char *book)
{
	bool	refresh = true;

	load_last_page(book);
	while (appletMainLoop()) {
		hidScanInput();

		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		// input
		if (kDown & KEY_DRIGHT) {
			ebook->last_page++;
			refresh = true;
		}
		if (kDown & KEY_DLEFT) {
			ebook->last_page--;
			refresh = true;
		}
		if (kDown & KEY_R) {
			ebook->last_page += 10;
			refresh = true;
		}
		if (kDown & KEY_L) {
			ebook->last_page -= 10;
			refresh = true;
		}
		if (kDown & KEY_ZR) {
			ebook->layout_orientation = !ebook->layout_orientation;
			refresh = true;
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
			render_page(book, ebook->last_page);
			SDL_RenderPresent(graphic->renderer);
			refresh = false;
		}

		if (kDown & KEY_PLUS) {
			save_last_page(book, ebook->last_page);
			break;
		}
	}

	log_info("ebook_reader() [Success]");
}
