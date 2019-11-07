#include "common.h"

static int	count_files_number(void)
{
	int				nb = 0;
	struct dirent	*entry = NULL;
	DIR				*dir = NULL;

	dir = opendir("/switch/ebookReaderNX/");
	if (dir == NULL) {
		perror("opendir");
		return (0);
	}

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG) {
			nb++;
		}
	}

	closedir(dir);
	log_info("count_files_number() [Success]");
	return (nb);
}

static char	**get_ebook_list(void)
{
	DIR				*dir = NULL;
	struct dirent	*entry = NULL;
	int				file_number = 0;
	char			**file_list = NULL;

	file_number = count_files_number();
	if (file_number == 0) {
		log_warn("No files ...");
		return (NULL);
	}

	file_list = (char **)calloc(sizeof(char *), file_number + 1);
	if (file_list == NULL) {
		log_fatal("calloc failure");
		return (NULL);
	}

	dir = opendir("/switch/ebookReaderNX/");
	if (dir == NULL) {
		perror("opendir");
		return (NULL);
	}

	for (int i = 0; (entry = readdir(dir)) != NULL;) {
		if (entry->d_type == DT_REG) {
			file_list[i] = strdup(entry->d_name);
			i++;
		}
	}

	closedir(dir);

	log_info("get_ebook_list() [Success]");
	return (file_list);
}

char	*home_page(void)
{
	char	**books = NULL;

	// get all books
	books = get_ebook_list();

	// draw menu to list and select books
	/*menu_select();*/

	return (books[0]);
}
