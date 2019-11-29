#include "common.h"

static u64 count_files(const char *path)
{
	struct archive	*handle = NULL;
	u64				count = 0;
	int				ret = 0;

	handle = archive_read_new();
	if (handle == NULL) {
		log_warn("%s", archive_error_string(handle));
		return (-1);
	}

	ret = archive_read_support_format_all(handle);
	if (ret != ARCHIVE_OK) {
		log_warn("%s", archive_error_string(handle));
		return (-1);
	}

	ret = archive_read_open_filename(handle, path, 1024);
	if (ret != ARCHIVE_OK) {
		printf("%s\n", archive_error_string(handle));
		printf("archive_read_open_filename%d\n", ret);
		return 1;
	}

	for (;;) {
		struct archive_entry *entry = NULL;

		ret = archive_read_next_header(handle, &entry);
		if (ret == ARCHIVE_EOF) {
			break;
		}

		count++;
	}

	ret = archive_read_free(handle);

	return (count);
}

bool	extract_cbr(char *path)
{
	struct archive	*handle = archive_read_new();
	struct archive	*dst = archive_write_disk_new();
	int				ret = 0;
	u64				max = 0;

	max = count_files(path);

	ret = archive_read_support_format_all(handle);
	if (ret != ARCHIVE_OK) {
		log_warn("%s", archive_error_string(handle));
		return (false);
	}

	ret = archive_read_open_filename(handle, path, 1024);
	if (ret != ARCHIVE_OK) {
		log_warn("%s", archive_error_string(handle));
		return (false);
	}



	ret = archive_write_free(dst);
	ret = archive_read_free(handle);
}
