#include "common.h"

extern t_cbr		*cbr;

static bool	init_cbr(t_cbr *cbr, const char *path)
{
	int	ret = 0;

	cbr->handle = archive_read_new();
	if (cbr->handle == NULL) {
		log_warn("%s", archive_error_string(cbr->handle));
		return (false);
	}

	cbr->dst = archive_write_disk_new();
	if (cbr->dst == NULL) {
		log_warn("%s", archive_error_string(cbr->dst));
		return (false);
	}

	ret = archive_read_support_format_all(cbr->handle);
	if (ret != ARCHIVE_OK) {
		log_warn("%s", archive_error_string(cbr->handle));
		return (false);
	}

	ret = archive_read_open_filename(cbr->handle, path, 1024);
	if (ret != ARCHIVE_OK) {
		log_warn("%s", archive_error_string(cbr->handle));
		return (false);
	}

	log_info("init_cbr() [Success]");

	return (true);
}

static void	deinit_cbr(t_cbr *cbr)
{
	archive_read_free(cbr->handle);
	archive_write_free(cbr->dst);

	log_info("deinit_cbr() [Success]");
}

static u64 count_files(t_cbr *cbr, const char *path)
{
	int		ret = 0;
	size_t	count = 0;

	if (init_cbr(cbr, path) == false) {
		return (-1);
	}

	for (;;) {
		struct archive_entry *entry = NULL;

		ret = archive_read_next_header(cbr->handle, &entry);
		if (ret == ARCHIVE_EOF) {
			break;
		}

		count++;
	}

	deinit_cbr(cbr);

	log_info("count_files() [Success]");
	return (count);
}

// TODO: review return value
static int write_data(t_cbr *cbr)
{
	int		ret = 0;
	size_t	total_length = 0;

	for (;;) {
		const void *chunk = NULL;
		size_t length = 0;
		s64 offset = 0;

		ret = archive_read_data_block(cbr->handle, &chunk, &length, &offset);
		if (ret == ARCHIVE_EOF)
			return ARCHIVE_OK;

		if (ret != ARCHIVE_OK)
			return ret;

		ret = archive_write_data_block(cbr->dst, chunk, length, offset);
		if (ret != ARCHIVE_OK)
			return ret;
	}

	log_info("write_data() [Success]");
	return -1;
}

static int myCompare(const void* a, const void* b)
{
	return (strcmp(*(const char**)a, *(const char**)b));
}

char	**get_archive_content_file_name(char *path, t_cbr *cbr)
{
	u64				count = 0;
	int				ret = 0;
	char			**array = NULL;

	array = (char **)calloc(sizeof(char *), cbr->total_page + 1);
	if (array == NULL) {
		log_warn("calloc error\n");
		return (NULL);
	}

	if (init_cbr(cbr, path) == false) {
		free(array);
		array = NULL;
		return (NULL);
	}

	for (;;) {
		struct archive_entry *entry = NULL;

		ret = archive_read_next_header(cbr->handle, &entry);
		if (ret == ARCHIVE_EOF) {
			break;
		}

		const char *entry_path = archive_entry_pathname(entry);
		if (entry_path == NULL) {
			log_warn("%s", archive_error_string(cbr->handle));
			break ;
		}
		array[count] = strdup(entry_path);

		count++;
	}

	deinit_cbr(cbr);

	// Sort files name
	qsort(array, cbr->total_page, sizeof(const char *), myCompare);

	log_info("get_archive_content_file_name() [Success]");

	return (array);
}

bool	extract_cbr(char *path, int page_number)
{
	char	**array = NULL;
	int		ret = 0;
	u64		count = 0;

	// create temp directory
	if (isFileExist("/tmp") == false) {
		if (mkdir("/tmp", 0777) == -1) {
			log_warn("%s", strerror(errno));
			return (false);
		}
	}

	// Count files number in archive
	cbr->total_page = count_files(cbr, path);
	if (page_number > cbr->total_page) {
		log_warn("page %d not exist", page_number);
		return (false);
	} else if (cbr->total_page == -1) {
		log_warn("count_files [Failure]");
		return (false);
	}

	// Sort and store all filename in char **
	array = get_archive_content_file_name(path, cbr);
	if (array == NULL) {
		return (false);
	}

	if (init_cbr(cbr, path) == false) {
		free_2d_array(array);
		return (false);
	}

	for (;;) {
		struct archive_entry	*entry = NULL;

		ret = archive_read_next_header(cbr->handle, &entry);
		if (ret == ARCHIVE_EOF) {
			break;
		} else if (ret != ARCHIVE_OK) {
			log_warn("%s", archive_error_string(cbr->handle));
			if (ret != ARCHIVE_WARN) {
				break;
			}
		}

		const char *entry_path = archive_entry_pathname(entry);
		if (entry_path == NULL) {
			log_warn("%s", archive_error_string(cbr->handle));
			ret = -1;
			break ;
		}

		if (!strcmp(array[page_number], entry_path)) {
			cbr->path = (char *)calloc(sizeof(char), PATH_MAX + 1);
			if (cbr->path == NULL) {
				free_2d_array(array);
				deinit_cbr(cbr);
				return (false);
			}

			snprintf(cbr->path, PATH_MAX, "%s/%s", EBOOK_PATH, entry_path);

			if (isFileExist(cbr->path) == false) {
				ret = archive_entry_update_pathname_utf8(entry, cbr->path);
				if (!ret) {
					log_warn("%s", archive_error_string(cbr->handle));
					break;
				}

				ret = archive_write_disk_set_options(cbr->dst, ARCHIVE_EXTRACT_UNLINK);
				ret = archive_write_header(cbr->dst, entry);
				if (ret != ARCHIVE_OK) {
					log_warn("%s", archive_error_string(cbr->handle));
					break;
				}

				ret = write_data(cbr);
				ret = archive_write_finish_entry(cbr->dst);

				ret = write_data(cbr);
			}

			break ;
		}

		count++;
	}

	free_2d_array(array);
	deinit_cbr(cbr);

	log_info("extract_cbr() [Success]");
	return (true);
}
