#include "common.h"

extern t_graphic	*graphic;
extern t_ebook		*ebook;
extern t_transform	*trans;

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


static bool	open_ebook(char *path)
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

static bool	count_page_number(void)
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

static fz_pixmap	*convert_page_to_ppm(int current_page)
{
	/* Render page to an RGB pixmap. */
	// fz_try(ebook->ctx)
		// fz_pixmap	*ppm = fz_new_pixmap_from_page_number(ebook->ctx, ebook->doc, current_page, trans->ctm, fz_device_rgb(ebook->ctx), 0);
		fz_pixmap	*ppm = fz_new_pixmap_from_page_contents(ebook->ctx, trans->page, trans->ctm, fz_device_rgb(ebook->ctx), 0);
		fz_drop_page(ebook->ctx, trans->page);
	// fz_catch(ebook->ctx) {
	// 	log_fatal("cannot render page: %s\n", fz_caught_message(ebook->ctx));
	// 	return (false);
	// }

	log_info("convert_page_to_ppm() [Success]");
	return (ppm);
}

void	get_page_info(int current_page)
{
	// get size of page
	// trans->page dropped n convert_page_to_ppm()
	trans->page = fz_load_page(ebook->ctx, ebook->doc, current_page);
	trans->bounds = fz_bound_page(ebook->ctx, trans->page);

	log_info("get_page_info() [Success]");
}

void	ebook_reader(char *path, int current_page)
{
	fz_pixmap *ppm = NULL;

	for (int i = 0; i <528 ; i++) {
		if (i != 0) {
			init_mupdf();
		}
	if (open_ebook(path) == false) {
		return ;
	}

	if (count_page_number() == false) {
		return ;
	}

	// check out of range index
	if (ebook->total_page < 0 || current_page >= ebook->total_page) {
		log_fatal("page number out of range: %d (page count %d)\n", ebook->total_page + 1, current_page);
		return ;
	}

	// loop here to naviguate in pdf
		get_page_info(current_page+i);
	
		portrait_default();
	
		if ((ppm = convert_page_to_ppm(current_page+i)) == NULL) {
			return ;
		}
	
		draw_ppm(ppm);
	
		fz_drop_pixmap(ebook->ctx, ppm);
		printf("%d/%d\n", i, ebook->total_page);
		deinit_mupdf();
	}
	// end of loop
}
