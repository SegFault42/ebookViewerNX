#ifndef GRAPHIC_H
#define GRAPHIC_H

#define WIN_WIDTH	1280
#define WIN_HEIGHT	720
#define CHAR_WIDTH_LARGE	22.5 // size of one character in px in X
#define CHAR_WIDTH_MEDIUM	17.25 // size of one character in px in X
#define COVER_WIDTH		350
#define COVER_HEIGHT	500

#define APP_NAME	"EbookReaderNX v0.1"

#define COVER		0
#define READ		1

typedef struct		s_ttf
{
	TTF_Font		*font_small;
	TTF_Font		*font_medium;
	TTF_Font		*font_large;
}					t_ttf;

typedef struct		s_graphic
{
	SDL_Window		*win;
	SDL_Renderer	*renderer;

	t_ttf			*ttf;
}					t_graphic;

bool	init_graphic(void);
void	deinit_graphic(void);
void	draw_ppm(fz_pixmap *ppm, bool cover);
void	draw_home_menu(char *books);
bool	init_ttf(void);
void	deinit_ttf(void);

#endif
