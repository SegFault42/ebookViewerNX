#include "common.h"

extern t_graphic	*graphic;
extern t_ebook		*ebook;

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

void	home_page(void)
{
	char	**books = NULL;
	int		index = 0;
	int		nb_books = 0;
	bool	refresh = true;

	// get all books
	books = get_ebook_list();
	if (books == NULL) {
		log_fatal("Get ebook list failure");
		return ;
	}

	nb_books = count_2d_array(books);


	while (appletMainLoop()) {
		hidScanInput();

		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

		// Draw the cover and book informations
		if (kDown & KEY_DRIGHT) {
			index++;
			refresh = true;
		}
		if (kDown & KEY_DLEFT) {
			index--;
			refresh = true;
		}
		// loop in array
		if (index == nb_books) {
			index = 0;
			refresh = true;
		} else if (index == -1) {
			index = nb_books -1;
			refresh = true;
		}

		if (kDown & KEY_A) {
			ebook_reader(books[index]);
			refresh = true;
		}

		if (refresh == true) {
			draw_ui(books[index]);
			refresh = false;
		}
		if (kDown & KEY_PLUS) {
			break ;
		}

		SDL_RenderPresent(graphic->renderer);
	}

	// free list 
	free_2d_array(books);

	log_info("home_page() [Success]");
}
