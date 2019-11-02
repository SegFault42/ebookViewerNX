// To compile linux version : gcc source/*.c -I include -L/usr/local/lib -lmupdf -lmupdf-third $(pkg-config --cflags --libs sdl2) -lm

#include <common.h>

t_graphic	*graphic = NULL;
t_transform	*trans = NULL;
t_ebook		*ebook = NULL;

static void	init_all(void)
{
	if (init_graphic() == false) {
		exit(-1);
	}
	if (init_mupdf() == false) {
		deinit_graphic();
		exit(-1);
	}
	if (init_layout() == false) {
		deinit_graphic();
		deinit_mupdf();
		exit (-1);
	}
}

static void	deinit_all(void)
{
	deinit_graphic();
	deinit_mupdf();
	deinit_layout();
}

int main(void)
{
	#ifdef __SWITCH__
		socketInitializeDefault();
		nxlinkStdio();
		/*twiliInitialize();*/
	#endif

	init_all();

	ebook_reader("./book.pdf", 0);

	sleep(3);

	deinit_all();

	log_info("Quitting ...");

	#ifdef __SWITCH__
		socketExit();
		/*twiliExit();*/
	#endif

	return (0);
}
