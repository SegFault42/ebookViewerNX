// To compile linux version :  gcc source/main.c $(pkg-config --cflags --libs sdl2)

#include <common.h>

int main(void)
{
	t_graphic	*graphic = NULL;

	graphic = init();

	if (graphic == NULL) {
		SDL_Log("init() [Failure]\n");
		deinit(graphic);
		return (-1);
	} else {
		SDL_Log("init() [Success]\n");
	}

	ebook("./book.pdf");

	deinit(graphic);

	return (0);
}
