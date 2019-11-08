// To compile linux version : gcc source/*.c -I include -L/usr/local/lib -lmupdf -lmupdf-third $(pkg-config --cflags --libs sdl2) -lm

#include <common.h>

t_graphic	*graphic = NULL;
t_transform	*trans = NULL;
t_ebook		*ebook = NULL;

void	create_requiered_folder(void)
{
	if (mkdir("/switch/ebookReaderNX", 0777) != -1) {
		log_info("/switch/ebookReaderNX created !");
	}
}

static void	init_all(void)
{
	Result	ret = 0;

	#ifdef __NXLINK__
		socketInitializeDefault();
		nxlinkStdio();
	#endif

	#ifdef __TWILI__
		twiliInitialize();
	#endif

	if (R_FAILED(ret = romfsInit())) {
		log_fatal("romfsInit() [Failure]");
		exit(-1);
	}

	graphic = (t_graphic *)calloc(sizeof(t_graphic), 1);
	graphic->ttf = (t_ttf *)calloc(sizeof(t_ttf), 1);
	ebook = (t_ebook *)calloc(sizeof(t_ebook), 1);
	if (graphic == NULL || graphic->ttf == NULL || ebook == NULL) {
		free(graphic);
		free(graphic->ttf);
		free(ebook);
		log_fatal("init_all() : calloc [Failure]");
		return ;
	}

	if (init_graphic() == false) {
		exit(-1);
	}
	if (init_ttf() == false) {
		deinit_graphic();
		exit(-1);
	}
	/*if (init_mupdf() == false) {*/
		/*deinit_graphic();*/
		/*deinit_ttf();*/
		/*exit(-1);*/
	/*}*/
	if (init_layout() == false) {
		deinit_graphic();
		deinit_ttf();
		/*deinit_mupdf();*/
		exit (-1);
	}

	create_requiered_folder();
}

static void	deinit_all(void)
{
	romfsExit();
	deinit_ttf();
	deinit_graphic();
	deinit_layout();

	free(graphic->ttf);
	free(graphic);
	free(ebook);

	log_info("Quitting ...");
	#ifdef __NXLINK__
		socketExit();
	#endif

	#ifdef __TWILI__
		twiliExit();
	#endif
}

int main(void)
{
	init_all();

	home_page();

	deinit_all();


	return (0);
}
