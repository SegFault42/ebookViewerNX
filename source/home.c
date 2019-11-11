#include "common.h"

extern t_graphic	*graphic;
extern t_ebook		*ebook;
extern t_controller	*controller;

static int	count_files_number(void)
{
	int				nb = 0;
	struct dirent	*entry = NULL;
	DIR				*dir = NULL;
	char			*ext = NULL;

	dir = opendir("/switch/ebookReaderNX/");
	if (dir == NULL) {
		perror("opendir");
		return (0);
	}

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG) {
			ext = strrchr(entry->d_name, '.');
			if (ext != NULL && !strcmp(ext, ".pdf")) {
				nb++;
			}
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
	char			*ext = NULL;

	file_number = count_files_number();
	if (file_number == 0) {
		log_warn("No files ...");
		return (NULL);
	}

	dir = opendir("/switch/ebookReaderNX/");
	if (dir == NULL) {
		log_fatal("%s", strerror(errno));
		return (NULL);
	}

	file_list = (char **)calloc(sizeof(char *), file_number + 1);
	if (file_list == NULL) {
		log_fatal("calloc failure");
		return (NULL);
	}

	for (int i = 0; (entry = readdir(dir)) != NULL;) {
		if (entry->d_type == DT_REG) {
			ext = strrchr(entry->d_name, '.');
			if (ext != NULL && ( !strcmp(ext, ".pdf") || !strcmp(ext, ".epub"))) {
				file_list[i] = strdup(entry->d_name);
				i++;
			}
		}
	}

	closedir(dir);

	log_info("get_ebook_list() [Success]");
	return (file_list);
}

// get last page stored in config
static void	load_last_page(char *book)
{
	int		fd = 0;
	char	*line = NULL;
	char	*token = NULL;
	char	*tmp = NULL;

	fd = open(CONFIG_PATH, O_RDONLY);
	if (fd == -1) {
		log_warn("open : %s", strerror(errno));
		return ;
	}

	ebook->last_page = 0;
	while (get_next_line(fd, &line) > 0) {
		tmp = strdup(line);
		token = strtok(tmp, "=");
		if (!strcmp(token, book)) {
			token = strtok(NULL, "=");
			ebook->last_page = atoi(token);
			break ;
		}

		free(tmp);
		tmp = NULL;
		free(line);
		line = NULL;
	}
	free(tmp);
	tmp = NULL;
	free(line);
	line = NULL;

	close(fd);
	log_info("load_last_page() [Success]");
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
	if (nb_books == 0) {
		free(books);
		books = NULL;
		log_fatal("Please put ebook in /switch/ebookReaderNX");
		return ;
	}

	while (appletMainLoop()) {
		hidScanInput();

		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		touchPosition touch;

		hidTouchRead(&touch, 0);

		// Draw the cover and book informations
		if (kDown & controller->next_page || touch_next_page_home(touch) == true) {
			index++;
			refresh = true;
		}
		if (kDown & controller->prev_page || touch_prev_page_home(touch) == true) {
			index--;
			refresh = true;
		}
		// loop in array
		if (index == nb_books) {
			index = 0;
			refresh = true;
		} else if (index < 0) {
			index = nb_books -1;
			refresh = true;
		}

		if (kDown & controller->launch_book || touch_launch_book(touch) == true) {
			ebook_reader(books[index]);
			refresh = true;
		}

		// draw only if needed
		if (refresh == true) {
			load_last_page(books[index]);
			draw_ui(books[index]);
			SDL_RenderPresent(graphic->renderer);
			refresh = false;
		}

		if (kDown & controller->quit) {
			break ;
		}
	}

	// free list
	free_2d_array(books);

	log_info("home_page() [Success]");
}
