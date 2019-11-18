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

typedef struct		s_layout
{
	// home
	SDL_Rect		cover;
	SDL_Rect		exit_home;
	SDL_Rect		help_home;

	SDL_Rect		line;
	SDL_Point		app_title;
	bool			show_bar;

	// refacto
	SDL_Rect		help_button;
}					t_layout;

bool	init_layout(void);
void	deinit_layout(void);
void	landscape_default(void);
void	portrait_default(void);

#endif
