#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <archive.h>
#include <archive_entry.h>

typedef struct		s_cbr
{
	struct archive	*handle;
	char			*image;
	int				max;
}					t_cbr;

char	*extract_cbr(char *path, int page_number);

#endif
