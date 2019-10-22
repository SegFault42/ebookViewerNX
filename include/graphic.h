#ifndef GRAPHIC_H
#define GRAPHIC_H

typedef struct	s_graphic
{
	SDL_Window	*win;
	SDL_Renderer *renderer;
}				t_graphic;

void	deinit(t_graphic *graphic);
t_graphic	*init(void);

#endif
