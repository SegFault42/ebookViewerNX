#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <archive.h>
#include <archive_entry.h>

typedef struct		s_cbr
{
	struct archive	*handle;
	struct archive	*dst;
	int				total_page; // total page number
	char			*path;
	SDL_Surface		*image;
	SDL_Texture		*texture;
}					t_cbr;

bool	extract_cbr(char *path, int page_number);

#endif
