#ifndef EBOOK_H
#define EBOOK_H

#include <mupdf/fitz.h>

#define PORTRAIT	false
#define LANDSCAPE	true
#define CONFIG_PATH	"/switch/ebookReaderNX/config"

typedef struct	s_ebook
{
	fz_context	*ctx;
	fz_document	*doc;
	fz_pixmap	*ppm;

	int			total_page;
	int			last_page;

	bool		layout_orientation;
}				t_ebook;

void	ebook_reader(char *path);
bool	init_mupdf(void);
void	deinit_mupdf(void);
bool	open_ebook(char *path);
bool	convert_page_to_ppm(int current_page);
bool	count_page_number(void);
void	get_page_info(int current_page);

#endif
