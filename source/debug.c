#include "common.h"

extern t_graphic	*graphic;
extern t_layout		*layout;
extern t_ebook		*ebook;

static void	draw_hitbox(SDL_Rect rect)
{
	SDL_SetRenderDrawColor(graphic->renderer, 255, 0, 0, 100);
	SDL_RenderFillRect(graphic->renderer, &rect);

	SDL_SetRenderDrawColor(graphic->renderer, 255, 255, 0, 100);
	SDL_RenderDrawRect(graphic->renderer, &rect);
}

void	debug_draw_hitbox(void)
{
	draw_hitbox(layout->help_home);
	draw_hitbox(layout->exit_home);
	draw_hitbox(layout->next_page_button);
	draw_hitbox(layout->prev_page_button);

	if (ebook->read_mode == false) {
		draw_hitbox(layout->cover);
	}
}
