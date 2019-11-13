#include "common.h"

extern t_controller	*controller;
extern t_ebook		*ebook;
extern t_graphic	*graphic;
extern t_layout		*layout;

void	default_controller_layout(void)
{
	controller->next_page = KEY_RSTICK_RIGHT;
	controller->prev_page = KEY_RSTICK_LEFT;

	controller->next_multiple_page = KEY_RSTICK_UP;
	controller->prev_multiple_page = KEY_RSTICK_DOWN;

	controller->quit = KEY_PLUS;
	controller->launch_book = KEY_A;
	controller->info = KEY_X;
	controller->layout = KEY_ZR;
}

bool	touch_next_page_home(touchPosition touch)
{
	if (touch.px >= (u32)(layout->cover_pos.x + layout->cover_pos.w) &&
			touch.py > (u32)(layout->line.y)) {
		return (true);
	}

	return (false);
}

bool	touch_prev_page_home(touchPosition touch)
{
	if (touch.px > 0 && touch.px <= (u32)(layout->cover_pos.x) &&
			touch.py > (u32)(layout->line.y)) {
		return (true);
	}

	return (false);
}

bool	touch_launch_book_home(touchPosition touch)
{
	if (touch.px >= (u32)(layout->cover_pos.x) &&
			touch.px <= (u32)(layout->cover_pos.x + layout->cover_pos.w) &&
			touch.py >= (u32)(layout->cover_pos.y) &&
			touch.py <= (u32)(layout->cover_pos.y + layout->cover_pos.h)) {
		return (true);
	}

	return (false);
}

bool	touch_exit_home(touchPosition touch)
{
	if (touch.px >= (u32)layout->exit_home.x &&
			touch.px <= (u32)(layout->exit_home.x + layout->exit_home.w) &&
			touch.py >= (u32)layout->exit_home.y &&
			touch.py <= (u32)(layout->exit_home.y + layout->exit_home.h)) {
		return (true);
	}
	return (false);
}

bool	touch_next_page_read(touchPosition touch)
{
	if (ebook->layout_orientation == LANDSCAPE &&
			touch.px >= WIN_WIDTH / 2
			) {
		return (true);
	} else if (ebook->layout_orientation == PORTRAIT &&
			touch.py >= WIN_HEIGHT / 2
			) {
		return (true);
	}

	return (false);
}

bool	touch_prev_page_read(touchPosition touch)
{
	if (ebook->layout_orientation == LANDSCAPE &&
			touch.px <= WIN_WIDTH / 2 && touch.px > 0) {
		return (true);
	} else if (ebook->layout_orientation == PORTRAIT &&
			touch.py <= WIN_HEIGHT / 2 &&
			touch.py > 0
			) {
		return (true);
	}

	return (false);
}
