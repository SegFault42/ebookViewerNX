// To compile linux version : gcc source/*.c -I include -L/usr/local/lib -lmupdf -lmupdf-third $(pkg-config --cflags --libs sdl2) -lm

#include <common.h>

t_graphic	*graphic = NULL;

int main(void)
{
	#ifdef __SWITCH__
		socketInitializeDefault();
		nxlinkStdio();
		/*twiliInitialize();*/
	#endif

	graphic = init();
	if (graphic == NULL) {
		log_info("init() [Failure]");
		deinit();
		return (-1);
	} else {
		log_info("init() [Success]");
	}

	ebook("./book.pdf", 4);

	deinit();

	log_info("Quitting ...");

	#ifdef __SWITCH__
		socketExit();
		/*twiliExit();*/
	#endif

	return (0);
}
