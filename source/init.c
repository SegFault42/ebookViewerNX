#include "common.h"

extern t_graphic	*graphic;
extern t_transform	*trans;
extern t_ebook		*ebook;
extern t_controller	*controller;
extern t_layout		*layout;

static void	create_requiered_folder(void)
{
	if (mkdir("/switch/ebookViewerNX", 0777) != -1) {
		log_info("/switch/ebookViewerNX created !");
	}

	int fd = open("/switch/ebookViewerNX/config", O_CREAT, 0777);
	if (fd == -1) {
		log_warn("create config failed : %s", strerror(errno));
	}
	close(fd);
}


void	init_all(void)
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
	controller = (t_controller *)calloc(sizeof(t_controller), 1);
	layout = (t_layout *)calloc(sizeof(t_layout), 1);
	trans = (t_transform *)calloc(sizeof(t_transform), 1);
	if (graphic == NULL || graphic->ttf == NULL || ebook == NULL || controller == NULL || layout == NULL || trans == NULL) {
		free(graphic);
		free(graphic->ttf);
		free(ebook);
		free(controller);
		free(layout);
		free(trans);
		log_fatal("init_all() : calloc [Failure]");
		exit (-1);
	}

	default_controller_layout();
	if (init_graphic() == false) {
		exit(-1);
	}
	if (init_ttf() == false) {
		deinit_graphic();
		exit(-1);
	}
	if (init_layout() == false) {
		deinit_graphic();
		deinit_ttf();
		exit (-1);
	}

	create_requiered_folder();

	log_info("init_all() [Success]");
}

void	deinit_all(void)
{
	romfsExit();
	deinit_ttf();
	deinit_graphic();
	deinit_layout();

	free(graphic->ttf);
	free(graphic);
	free(ebook);
	free(controller);
	free(layout);
	free(trans);

	log_info("Quitting ...");
	#ifdef __NXLINK__
		socketExit();
	#endif

	#ifdef __TWILI__
		twiliExit();
	#endif

	log_info("deinit_all() [Success]");
}

bool	init_ttf(void)
{
	if (TTF_Init() == -1) {
		log_fatal("TTF_Init(): %s\n", TTF_GetError());
		free(graphic->ttf);
		graphic->ttf = NULL;
		return (false);
	}

	graphic->ttf->font_small = TTF_OpenFont("romfs:/fonts/NintendoStandard.ttf", 24);
	graphic->ttf->font_medium = TTF_OpenFont("romfs:/fonts/NintendoStandard.ttf", 31);
	graphic->ttf->font_large = TTF_OpenFont("romfs:/fonts/NintendoStandard.ttf", 42);
	if (graphic->ttf->font_small == NULL || graphic->ttf->font_medium == NULL || graphic->ttf->font_large == NULL) {
		log_fatal("TTF_OpenFont(): %s\n", TTF_GetError());
		free(graphic->ttf);
		graphic->ttf = NULL;
		TTF_Quit();
		return (false);
	}

	log_info("init_ttf() [Success]");
	return (true);
}

void	deinit_ttf(void)
{
	TTF_CloseFont(graphic->ttf->font_small);
	TTF_CloseFont(graphic->ttf->font_medium);
	TTF_CloseFont(graphic->ttf->font_large);

	TTF_Quit();

	log_info("deinit_ttf() [Success]");
}

bool	init_graphic(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		log_fatal("SDL_Init(): %s\n", SDL_GetError());
		free(graphic);
		return (false);
	}
	if (IMG_Init(IMG_INIT_PNG) < 0) {
		log_fatal("IMG_Init(): %s\n", IMG_GetError());
		SDL_Quit();
		free(graphic);
		return (false);
	}

	graphic->win = SDL_CreateWindow("", 0, 0, WIN_WIDTH, WIN_HEIGHT, 0);
	if (graphic->win == NULL) {
		log_fatal("SDL_CreateWindow(): %s\n", SDL_GetError());
		SDL_Quit();
		IMG_Quit();
		free(graphic);
		return (false);
	}

	graphic->renderer = SDL_CreateRenderer(graphic->win, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (graphic->renderer == NULL) {
		log_fatal("SDL_CreateRenderer(): %s\n", SDL_GetError());
		SDL_DestroyWindow(graphic->win);
		SDL_Quit();
		IMG_Quit();
		free(graphic);
		return (false);
	}

	SDL_SetRenderDrawBlendMode(graphic->renderer, SDL_BLENDMODE_BLEND);

	log_info("init_graphic() [Success]");

	return (true);
}

void	deinit_graphic(void)
{
	SDL_DestroyWindow(graphic->win);
	SDL_DestroyRenderer(graphic->renderer);

	SDL_Quit();
	IMG_Quit();

	log_info("deinit_graphic() [Success]");
}
