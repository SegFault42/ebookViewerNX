#include "common.h"

extern t_graphic	*graphic;

static fz_context	*init_mupdf(void)
{
	fz_context	*ctx = NULL;

	/* Create a context to hold the exception stack and various caches. */
	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx) {
		log_fatal("cannot create mupdf context");
		return (NULL);
	}

	/* Register the default file types to handle. */
	fz_try(ctx) {
		fz_register_document_handlers(ctx);
	}
	fz_catch(ctx) {
		log_fatal("cannot register document handlers: %s\n", fz_caught_message(ctx));
		fz_drop_context(ctx);
		return (NULL);
	}

	log_info("init_mupdf() [Success]");
	return (ctx);
}

static void	deinit_mupdf(fz_context *ctx, fz_document *doc)
{
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
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

static int	count_page_number(fz_context *ctx, fz_document *doc)
{
	int	page_count = 0;

	/* Count the number of pages. */
	fz_try(ctx) {
		page_count = fz_count_pages(ctx, doc);
	}
	fz_catch(ctx) {
		log_fatal("cannot count number of pages: %s\n", fz_caught_message(ctx));
		deinit_mupdf(ctx, doc);
		return (-1);
	}

	log_info("count_page_number() [Success]");
	return (page_count);
}

static fz_pixmap	*convert_page_to_ppm(fz_context *ctx, fz_document *doc, int current_page)
{
	fz_matrix	ctm;
	fz_pixmap	*ppm = NULL;
	float zoom = 100, rotate = 0;

	ctm = fz_scale(zoom / 100, zoom / 100);
	ctm = fz_pre_rotate(ctm, rotate);

	/* Render page to an RGB pixmap. */
	fz_try(ctx)
		ppm = fz_new_pixmap_from_page_number(ctx, doc, current_page, ctm, fz_device_rgb(ctx), 0);
	fz_catch(ctx)
	{
		log_fatal("cannot render page: %s\n", fz_caught_message(ctx));
		deinit_mupdf(ctx, doc);
		return (NULL);
	}

	log_info("convert_page_to_ppm() [Success]");
	return (ppm);
}

void	ebook(char *path, int page_index)
{
	fz_context	*ctx = NULL;
	fz_document	*doc = NULL;
	fz_pixmap	*ppm = NULL;
	int			total_page = 0;

	ctx = init_mupdf();
	if (ctx == NULL) {
		return ;
	}

	doc = open_ebook(path, ctx);
	if (doc == NULL) {
		return ;
	}

	total_page = count_page_number(ctx, doc);
	if (total_page == -1) {
		return ;
	}

	// check out of range index
	if (total_page < 0 || page_index >= total_page) {
		log_fatal("page number out of range: %d (page count %d)\n", total_page + 1, page_index);
		deinit_mupdf(ctx, doc);
		return ;
	}

	// loop here to naviguate in pdf
	ppm = convert_page_to_ppm(ctx, doc, page_index);
	if (ppm == NULL) {
		return ;
	}

	draw_ppm(ppm);
	// free ppm
	fz_drop_pixmap(ctx, ppm);
	// end of loop

	sleep(1);

	deinit_mupdf(ctx, doc);
}
