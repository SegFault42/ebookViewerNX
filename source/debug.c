#include "common.h"

extern t_graphic	*graphic;
extern t_layout		*layout;

static void	generate_random_color(void)
{
	/*time_t	t;*/

	/*srand((unsigned) time(&t));*/

	/*int r = rand() % 255, g = rand() % 255, b = rand() % 255;*/

	SDL_SetRenderDrawColor(graphic->renderer, 255, 0, 0, 100);
}

static void	draw_help_button(void)
{
	generate_random_color();
	SDL_RenderFillRect(graphic->renderer, &layout->help_home);
}

static void	draw_exit_button(void)
{
	generate_random_color();
	SDL_RenderFillRect(graphic->renderer, &layout->exit_home);
}

static void	draw_next_page_button(void)
{
	generate_random_color();
	SDL_RenderFillRect(graphic->renderer, &layout->next_page_button);
}

static void	draw_prev_page_button(void)
{
	generate_random_color();
	SDL_RenderFillRect(graphic->renderer, &layout->prev_page_button);
}

static void	draw_cover(void)
{
	generate_random_color();
	SDL_RenderFillRect(graphic->renderer, &layout->cover);
}

void	debug_draw_hitbox(void)
{
	draw_help_button();
	draw_exit_button();
	draw_next_page_button();
	draw_prev_page_button();
	draw_cover();
}
