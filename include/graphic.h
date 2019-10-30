#ifndef GRAPHIC_H
#define GRAPHIC_H

#define WIN_WIDTH	1280
#define WIN_HEIGHT	720

typedef struct		s_graphic
{
	SDL_Window		*win;
	SDL_Renderer	*renderer;
}					t_graphic;

t_graphic	*init(void);
void		deinit(void);
void		draw_ppm(fz_pixmap *ppm);

#endif
