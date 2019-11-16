#ifndef CONTROLLER_H
#define CONTROLLER_H

typedef struct	s_controller
{
	// common
	int			next_page;
	int			prev_page;
	int			quit;

	// menu mode
	int			launch_book;
	int			help;

	// reading mode
	int			layout;
	int			next_multiple_page;
	int			prev_multiple_page;
}				t_controller;

enum button_id
{
	e_exit = 0,
	e_help = 1,
	e_cover = 2
};

void			default_controller_layout(void);

bool			touch_next_page_home(touchPosition touch);
bool			touch_prev_page_home(touchPosition touch);

bool			touch_launch_book_home(touchPosition touch);

bool			touch_exit_home(touchPosition touch);

bool			touch_next_page_read(touchPosition touch);
bool			touch_prev_page_read(touchPosition touch);

bool			touch_button(touchPosition touch, int button_id);
bool	button_touch(touchPosition touch, SDL_Rect rect);

#endif
