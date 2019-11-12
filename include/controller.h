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
	int			info;

	// reading mode
	int			layout;
	int			next_multiple_page;
	int			prev_multiple_page;
}				t_controller;


void			default_controller_layout(void);

bool			touch_next_page_home(touchPosition touch);
bool			touch_prev_page_home(touchPosition touch);

bool			touch_launch_book_home(touchPosition touch);

bool			touch_exit_home(touchPosition touch);

bool			touch_next_page_read(touchPosition touch);
bool			touch_prev_page_read(touchPosition touch);

#endif
