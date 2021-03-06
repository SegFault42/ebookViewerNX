#include "common.h"

size_t	count_2d_array(char **array)
{
	size_t	idx = 0;

	if (array != NULL) {
		while (array[idx]) {
			idx++;
		}
	}

	return (idx);
}

void	free_2d_array(char **array)
{
	for (int i = 0; array[i]; i++) {
		free(array[i]);
		array[i] = NULL;
	}
	free(array);
	array = NULL;
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
