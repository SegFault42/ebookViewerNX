#ifndef LAYOUT_H
#define LAYOUT_H

typedef struct	s_transform
{
	fz_matrix	ctm;
	fz_page		*page;
	fz_rect		bounds;
	float		zoom;
	float		rotate;
	SDL_Rect	dstrect;
}				t_transform;

bool	init_layout(void);
void	deinit_layout(void);

#endif
