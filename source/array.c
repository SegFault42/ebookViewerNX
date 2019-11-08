#include "common.h"

size_t	count_2d_array(char **array)
{
	size_t	idx = 0;

	while (array[idx]) {
		idx++;
	}

	return (idx);
}

void	free_2d_array(char ***array)
{
	if (array) {
		for (size_t idx = 0; array[idx]; idx++) {
			free(*array[idx]);
			*array[idx] = NULL;
		}

		free(*array);
		*array = NULL;
	}
}

void	print_2d_array(char **array)
{
	if (array == NULL) {
		printf("NULL");
		return ;
	}

	printf("[ ");

	for (int idx = 0; array[idx]; idx++) {
		printf("[ %s ]", array[idx]);

		if (array[idx +1]) {
			printf(", ");
		}
	}

	printf(" ]");
}
