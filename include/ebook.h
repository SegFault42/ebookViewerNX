#ifndef EBOOK_H
#define EBOOK_H

#include <mupdf/fitz.h>

fz_pixmap	*ebook(char *path, int page_index);

#endif
