#include "common.h"

extern t_graphic	*graphic;

static fz_context	*init_mupdf(void)
{
	fz_context	*ctx = NULL;

	/* Create a context to hold the exception stack and various caches. */
	ctx = fz_new_context(NULL, NULL, FZ_STORE_UNLIMITED);
	if (!ctx) {
		fprintf(stderr, "cannot create mupdf context\n");
		return (NULL);
	}

	/* Register the default file types to handle. */
	fz_try(ctx) {
		fz_register_document_handlers(ctx);
	}
	fz_catch(ctx) {
		fprintf(stderr, "cannot register document handlers: %s\n", fz_caught_message(ctx));
		fz_drop_context(ctx);
		return (NULL);
	}

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
		fprintf(stderr, "cannot open document: %s\n", fz_caught_message(ctx));
		fz_drop_context(ctx);
		return (NULL);
	}

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
		fprintf(stderr, "cannot count number of pages: %s\n", fz_caught_message(ctx));
		deinit_mupdf(ctx, doc);
		return (-1);
	}

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
		fprintf(stderr, "cannot render page: %s\n", fz_caught_message(ctx));
		deinit_mupdf(ctx, doc);
		return (NULL);
	}

	return (ppm);
}

fz_pixmap	*ebook(char *path, int page_index)
{
	fz_context	*ctx = NULL;
	fz_document	*doc = NULL;
	fz_pixmap	*ppm = NULL;
	int			total_page = 0;

	ctx = init_mupdf();
	if (ctx == NULL) {
		return (NULL);
	}

	doc = open_ebook(path, ctx);
	if (doc == NULL) {
		return (NULL);
	}

	total_page = count_page_number(ctx, doc);
	if (total_page == -1) {
		return (NULL);
	}

	// check out of range index
	if (total_page < 0 || page_index >= total_page) {
		fprintf(stderr, "page number out of range: %d (page count %d)\n", total_page + 1, page_index);
		deinit_mupdf(ctx, doc);
		return (NULL);
	}

	// loop here to naviguate in pdf
	ppm = convert_page_to_ppm(ctx, doc, page_index);
	if (ppm == NULL) {
		return (NULL);
	}

	draw_ppm(ppm);
	fz_drop_pixmap(ctx, ppm);
	// end of loop

	deinit_mupdf(ctx, doc);

	return (ppm);
}
