#include "common.h"

extern t_graphic	*graphic;
extern t_ebook		*ebook;

bool	init_mupdf(void)
{
	ebook = (t_ebook *)calloc(sizeof(t_ebook), 1);
	if (ebook == NULL) {
		log_fatal("init_mupdf() : calloc failure");
		return (false);
	}

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
	free(ebook);
	ebook = NULL;
	log_info("deinit_mupdf() [Success]");
}


static fz_document	*open_ebook(char *path, fz_context *ctx)
{
	fz_document	*doc = NULL;

	/* Open the document. */
	fz_try(ctx) {
		doc = fz_open_document(ctx, path);
	}
	fz_catch(ctx) {
		log_fatal("cannot open document: %s\n", fz_caught_message(ctx));
		fz_drop_context(ctx);
		return (NULL);
	}

	log_info("open_ebook() [Success]");
	return (doc);
}

static int	count_page_number(t_ebook *ebook)
{
	int	page_count = 0;

	/* Count the number of pages. */
	fz_try(ebook->ctx) {
		page_count = fz_count_pages(ebook->ctx, ebook->doc);
	}
	fz_catch(ebook->ctx) {
		log_fatal("cannot count number of pages: %s\n", fz_caught_message(ebook->ctx));
		return (-1);
	}

	log_info("count_page_number() [Success]");
	return (page_count);
}

// TODO: create struct for math
static fz_pixmap	*convert_page_to_ppm(t_ebook *ebook, int current_page)
{
	fz_matrix	ctm;
	fz_pixmap	*ppm = NULL;
	fz_page		*page = NULL;
	fz_rect		bounds;
	float		zoom = 0, rotate = 0;

	// get size of page
    page = fz_load_page(ebook->ctx, ebook->doc, current_page);
	bounds = fz_bound_page(ebook->ctx, page);
	fz_drop_page(ebook->ctx, page);

	// calculate to fit in Y (Default zoom)
	zoom = (WIN_HEIGHT * 100) / bounds.y1;

	// set zoom and rotation
	ctm = fz_scale(zoom / 100, zoom / 100);
	ctm = fz_pre_rotate(ctm, rotate);

	/* Render page to an RGB pixmap. */
	fz_try(ebook->ctx)
		ppm = fz_new_pixmap_from_page_number(ebook->ctx, ebook->doc, current_page, ctm, fz_device_rgb(ebook->ctx), 0);
	fz_catch(ebook->ctx) {
		log_fatal("cannot render page: %s\n", fz_caught_message(ebook->ctx));
		return (NULL);
	}

	log_info("convert_page_to_ppm() [Success]");
	return (ppm);
}

void	ebook_reader(char *path, int page_index)
{
	ebook->doc = open_ebook(path, ebook->ctx);
	if (ebook->doc == NULL) {
		return ;
	}

	ebook->total_page = count_page_number(ebook);
	if (ebook->total_page == -1) {
		return ;
	}

	// check out of range index
	if (ebook->total_page < 0 || page_index >= ebook->total_page) {
		log_fatal("page number out of range: %d (page count %d)\n", ebook->total_page + 1, page_index);
		return ;
	}

	// loop here to naviguate in pdf

	ebook->ppm = convert_page_to_ppm(ebook, page_index);
	if (ebook->ppm == NULL) {
		return ;
	}

	draw_ppm(ebook->ppm);
	// free ppm
	fz_drop_pixmap(ebook->ctx, ebook->ppm);
	// end of loop
}
