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
bool	open_ebook(char *path);
bool	convert_page_to_ppm(int current_page);
bool	count_page_number(void);

#endif
