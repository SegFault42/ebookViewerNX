#include "common.h"

extern t_controller	*controller;
extern t_ebook		*ebook;

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
	if (touch.px >= (WIN_WIDTH / 2) - (COVER_WIDTH / 2) + COVER_WIDTH) {
		return (true);
	}

	return (false);
}

bool	touch_prev_page_home(touchPosition touch)
{
	if (touch.px > 0 && touch.px <= (WIN_WIDTH / 2) - (COVER_WIDTH / 2)) {
		return (true);
	}

	return (false);
}

bool	touch_launch_book(touchPosition touch)
{
	if (touch.px >= (WIN_WIDTH / 2) - (COVER_WIDTH / 2) &&
			touch.px <= (WIN_WIDTH / 2) - (COVER_WIDTH / 2) + COVER_WIDTH &&
			touch.py >= (WIN_HEIGHT / 2) - (COVER_HEIGHT / 2) &&
			touch.py <= (WIN_HEIGHT / 2) - (COVER_HEIGHT / 2) + COVER_HEIGHT) {
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
