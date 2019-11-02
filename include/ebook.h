#ifndef EBOOK_H
#define EBOOK_H

#include <mupdf/fitz.h>

typedef struct	s_ebook
{
	fz_context	*ctx;
	fz_document	*doc;
	fz_pixmap	*ppm;
	int			total_page;
	int			current_page; // Current page in book
}				t_ebook;

void	ebook_reader(char *path, int page_index);
bool	init_mupdf(void);
void	deinit_mupdf(void);

#endif
