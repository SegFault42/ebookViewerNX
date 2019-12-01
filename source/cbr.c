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

typedef struct		s_cbr
{
	struct archive	*handle;
	struct archive	*dst;
	u64				max;
}					t_cbr;

static bool	init_cbr(t_cbr *cbr, char *path)
{
	int	ret = 0;

	cbr->handle = archive_read_new();
	if (cbr->handle == NULL) {
		log_warn("%s", archive_error_string(cbr->handle));
		return (false);
	}

	cbr->dst = archive_write_disk_new();
	if (cbr->handle == NULL) {
		log_warn("%s", archive_error_string(cbr->handle));
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

	return (true);
}

bool	extract_cbr(char *path)
{
	t_cbr	cbr = {0};
	int		ret = 0;

	cbr.max = count_files(path);

	if (init_cbr(&cbr, path) == false) {
		return (false);
	}

	mkdir("/tmp", 0777);

	for (;;) {
		struct archive_entry *entry = NULL;
		ret = archive_read_next_header(handle, &cbr.entry);
		if (ret == ARCHIVE_EOF) {
			break;
		}

		if (ret != ARCHIVE_OK) {
			Menu_DisplayError("archive_read_next_header failed:", ret);
			if (ret != ARCHIVE_WARN) {
				break;
			}
		}

		const char *entry_path = archive_entry_pathname(entry);
		char new_path[1024];

		ret = snprintf(new_path, sizeof(new_path), "%s/%s", dest_path, entry_path);
		ret = archive_entry_update_pathname_utf8(entry, new_path);
		if (!ret) {
			Menu_DisplayError("archive_entry_update_pathname_utf8:", ret);
			break;
		}

		ret = archive_write_disk_set_options(dst, ARCHIVE_EXTRACT_UNLINK);
		ret = archive_write_header(dst, entry);
		if (ret != ARCHIVE_OK) {
			Menu_DisplayError("archive_write_header failed:", ret);
			break;
		}

		ret = Archive_WriteData(handle, dst);
		ret = archive_write_finish_entry(dst);
		count++;
	}

	ret = archive_write_free(cbr.dst);
	ret = archive_read_free(cbr.handle);

	return (true);
}
