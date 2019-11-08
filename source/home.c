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
	int		index = 0;
	int		nb_books = 0;

	// get all books
	books = get_ebook_list();
	if (books == NULL) {
		return (NULL);
	}

	nb_books = count_2d_array(books);

	draw_ui(books[index]);

	while (appletMainLoop()) {
		hidScanInput();

		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		// Draw the cover and book informations
		if (kDown & KEY_PLUS) {
			break ;
		}
		if (kDown & KEY_DRIGHT) {
			index++;
		}
		if (kDown & KEY_DLEFT) {
			index--;
		}

		// loop in array
		if (index == nb_books) {
			index = 0;
		} else if (index == -1) {
			index = nb_books -1;
		}

		if (kDown)
			draw_ui(books[index]);

	}

	log_info("home_page() [Success]");
	return (books[0]);
}
