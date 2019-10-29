// To compile linux version : gcc source/*.c -I include -L/usr/local/lib -lmupdf -lmupdf-third $(pkg-config --cflags --libs sdl2) -lm

#include <common.h>

t_graphic	*graphic = NULL;

int main(void)
{
	fz_pixmap	*ppm;

	#ifdef __SWITCH__
		socketInitializeDefault();
		nxlinkStdio();
	#endif

	graphic = init();
	if (graphic == NULL) {
		log_info("init() [Failure]");
		deinit();
		return (-1);
	} else {
		log_info("init() [Success]");
	}

	if ((ppm = ebook("./book.pdf", 0)) == NULL) {
		deinit();
		#ifdef __SWITCH__
			socketExit();
		#endif
		return (-1);
	}

	deinit();

	#ifdef __SWITCH__
		socketExit();
	#endif

	return (0);
}
