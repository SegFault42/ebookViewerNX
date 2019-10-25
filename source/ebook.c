#include "common.h"

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

static int	count_page_number(fz_context *ctx, fz_document *doc, int page_number)
{
	int	page_count = 0;

	/* Count the number of pages. */
	fz_try(ctx) {
		page_count = fz_count_pages(ctx, doc);
	}
	fz_catch(ctx) {
		fprintf(stderr, "cannot count number of pages: %s\n", fz_caught_message(ctx));
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return (-1);
	}

	if (page_number < 0 || page_number >= page_count) {
		fprintf(stderr, "page number out of range: %d (page count %d)\n", page_number + 1, page_count);
		fz_drop_document(ctx, doc);
		fz_drop_context(ctx);
		return (-1);
	}

	return (page_number);
}

static void	deinit_mupdf(fz_context *ctx, fz_document *doc)
{
	fz_drop_document(ctx, doc);
	fz_drop_context(ctx);
}

void	*ebook(char *path, int page_count)
{
	fz_context	*ctx = NULL;
	fz_document	*doc = NULL;
	int			page_number = 0;

	ctx = init_mupdf();
	if (ctx == NULL) {
		return (NULL);
	}

	doc = open_ebook(path, ctx);
	if (doc == NULL) {
		return (NULL);
	}

	page_number = count_page_number(ctx, doc, page_count);
	if (page_number == -1) {
		return (NULL);
	}

	printf("page number = %d\n", page_number);

	deinit_mupdf(ctx, doc);

	return ((void *)-1);
}
