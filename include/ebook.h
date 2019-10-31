#ifndef EBOOK_H
#define EBOOK_H

#include <mupdf/fitz.h>

typedef struct	s_ebook
{
	fz_context	*ctx;
	fz_document	*doc;
	fz_pixmap	*ppm;
	int			total_page;
	int			page_index; // Current page in book
}				t_ebook;

void	ebook(char *path, int page_index);

#endif
