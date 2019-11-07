#ifndef GRAPHIC_H
#define GRAPHIC_H

#define WIN_WIDTH	1280
#define WIN_HEIGHT	720

typedef struct		s_graphic
{
	SDL_Window		*win;
	SDL_Renderer	*renderer;

	TTF_Font		*font_small;
	TTF_Font		*font_medium;
	TTF_Font		*font_large;
}					t_graphic;

bool	init_graphic(void);
void	deinit_graphic(void);
void	draw_ppm(fz_pixmap *ppm);
void	draw_ui(char *books);

#endif
