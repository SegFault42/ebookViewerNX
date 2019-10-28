// To compile linux version :  gcc source/main.c $(pkg-config --cflags --libs sdl2)

#include <common.h>

int main(void)
{
	t_graphic	*graphic = NULL;
	fz_pixmap	*ppm;

	socketInitializeDefault();
	nxlinkStdio();

	graphic = init();

	if (graphic == NULL) {
		SDL_Log("init() [Failure]\n");
		deinit(graphic);
		return (-1);
	} else {
		SDL_Log("init() [Success]\n");
	}

	if ((ppm = ebook("/book.pdf", 50)) == NULL) {
		deinit(graphic);
		socketExit();
		return (-1);
	}

	draw_ppm(graphic, ppm);

	sleep(3);

	deinit(graphic);
	socketExit();

	return (0);
}
