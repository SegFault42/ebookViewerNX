#ifndef GRAPHIC_H
#define GRAPHIC_H

typedef struct	s_graphic
{
	SDL_Window	*win;
	SDL_Renderer *renderer;
}				t_graphic;

t_graphic	*init(void);
void		deinit(void);
void		draw_ppm(fz_pixmap *ppm);

#endif
