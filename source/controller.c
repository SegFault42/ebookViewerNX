#include "common.h"

extern t_controller	*controller;
extern t_ebook		*ebook;
extern t_graphic	*graphic;
extern t_layout		*layout;

bool	button_touch(touchPosition touch, SDL_Rect rect)
{
	touchPosition			released = {0};
	static touchPosition	released_save = {0};

	if (touch.px == 0 || touch.py == 0) {
		return (false);
	}

	while (true) {
		hidScanInput();
		hidTouchRead(&released, 0);

		// check if button is pressed and released not outside de hitbox
		if (released.px == 0 && released.py == 0) {
			if (touch.px >= (u32)(rect.x) &&
				touch.px <= (u32)(rect.x + rect.w) &&
				touch.py >= (u32)(rect.y) &&
				touch.py <= (u32)(rect.y + rect.h) &&
				released_save.px >= (u32)(rect.x) &&
				released_save.px <= (u32)(rect.x + rect.w) &&
				released_save.py >= (u32)(rect.y) &&
				released_save.py <= (u32)(rect.y + rect.h)) {
				return (true);
			}
		}

		if (released.px != 0 && released.py != 0) {
			memcpy(&released_save, &released, sizeof(touchPosition));
		}
		if (released.px == 0 && released.py == 0) {
			break ;
		}
	}

	return (false);
}

void	default_controller_layout(void)
{
	controller->next_page = KEY_RSTICK_RIGHT;
	controller->prev_page = KEY_RSTICK_LEFT;

	controller->next_multiple_page = KEY_RSTICK_UP;
	controller->prev_multiple_page = KEY_RSTICK_DOWN;

	controller->quit = KEY_PLUS;
	controller->launch_book = KEY_A;
	controller->help = KEY_X;
	controller->layout = KEY_ZR;
}

/*bool	touch_prev_page_home(touchPosition touch)*/
/*{*/
	/*if (touch.px > 0 && touch.px <= (u32)(layout->cover.x) &&*/
			/*touch.py > (u32)(layout->bar.line.y)) {*/
		/*return (true);*/
	/*}*/

	/*return (false);*/
/*}*/

bool	touch_button(touchPosition touch, int button_id)
{
	if (button_id == e_exit && button_touch(touch, layout->exit_home) == true) {
		return (true);
	} else if (button_id == e_cover && button_touch(touch, layout->cover) == true) {
		return (true);
	} else if (button_id == e_help && button_touch(touch, layout->help_home) == true) {
		return (true);
	} else if (button_id == e_rotate && button_touch(touch, layout->rotate_button) == true) {
		return (true);
	} else if (button_id == e_bar && button_touch(touch, layout->bar.back_bar) == true) {
		return (true);
	} else if (button_id == e_next_page && button_touch(touch, layout->next_page_button) == true) {
		return (true);
	} else if (button_id == e_prev_page && button_touch(touch, layout->prev_page_button) == true) {
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
