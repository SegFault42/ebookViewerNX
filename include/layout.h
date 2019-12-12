#ifndef LAYOUT_H
#define LAYOUT_H

typedef struct	s_transform
{
	fz_matrix	ctm;
	fz_page		*page;
	fz_rect		bounds;
	float		zoom;
	float		rotate;
}				t_transform;

typedef struct		s_bar
{
	SDL_Rect		line;
	SDL_Rect		back_bar;
}					t_bar;

typedef struct		s_layout
{
	t_bar			bar;

	SDL_Point		app_title;
	SDL_Rect		cover;
	SDL_Rect		page;

	SDL_Rect		progress_bar;

	SDL_Rect		next_page_button;
	SDL_Rect		prev_page_button;
	SDL_Rect		exit_button;
	SDL_Rect		help_button;
	SDL_Rect		rotate_button;

	bool			show_bar;
}					t_layout;

bool	init_layout(void);
void	deinit_layout(void);
void	landscape_default(int type);
void	portrait_default(int type);
void	set_layout(void);

#endif
