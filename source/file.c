#include "common.h"

char	*get_file_extension(char *str)
{
	char	*ext = NULL;

	if (str == NULL) {
		return (NULL);
	}

	ext = strrchr(str, '.');
	if (ext == NULL) {
		return (NULL);
	}

	return (ext);
}

bool	isFileExist(const char *file)
{
	struct stat	st = {0};

	if (stat(file, &st) == -1) {
		return (false);
	}

	return (true);
}
