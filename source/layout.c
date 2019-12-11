#include "common.h"

extern t_transform	*trans;
extern t_ebook		*ebook;
extern t_layout		*layout;
extern t_graphic	*graphic;
extern t_cbr		*cbr;

void	set_layout(void)
{
	int w = 0, h = 0;

	if (ebook->layout_orientation == LANDSCAPE) {
		// Bar coord
		layout->bar.line.x = WIN_WIDTH / 32;
		layout->bar.line.y = WIN_HEIGHT / 12;
		layout->bar.line.w = WIN_WIDTH - (layout->bar.line.x * 2);
		layout->bar.line.h = 2;

		// coord to touch bar
		layout->bar.back_bar.x = 0;
		layout->bar.back_bar.y = 0;
		layout->bar.back_bar.w = WIN_WIDTH;
		layout->bar.back_bar.h = layout->bar.line.y;

		// coord app title
		TTF_SizeText(graphic->ttf->font_large, APP_NAME, &w, &h);
		layout->app_title.x = (WIN_WIDTH / 2) - (w / 2);
		layout->app_title.y = WIN_HEIGHT / 90;

		// coord exit button
		layout->exit_home.w = WIN_WIDTH / 14;
		layout->exit_home.h = layout->bar.line.y / 1.30;
		layout->exit_home.x = 0.8984375 * WIN_WIDTH;
		layout->exit_home.y = (layout->bar.line.y - layout->exit_home.h) / 2;

		if (ebook->read_mode == true) {
			// progress bar coord
			layout->progress_bar.x = (WIN_WIDTH / 2) - (400 / 2);
			layout->progress_bar.y = 50;
			layout->progress_bar.w = 400;
			layout->progress_bar.h = 2;

			// coord touch next_page
			layout->next_page_button.x = WIN_WIDTH / 2;
			layout->next_page_button.y = layout->bar.line.y;
			layout->next_page_button.w = WIN_WIDTH - (WIN_WIDTH / 2);
			layout->next_page_button.h = WIN_HEIGHT - layout->bar.line.y;

			// coord touch prev_page
			layout->prev_page_button.x = 0;
			layout->prev_page_button.y = layout->bar.line.y;
			layout->prev_page_button.w = WIN_WIDTH/2;
			layout->prev_page_button.h = WIN_HEIGHT - layout->bar.line.y;

			// coord rotate button
			layout->rotate_button.w = WIN_WIDTH / 14;
			layout->rotate_button.h = layout->bar.line.y / 1.30;
			layout->rotate_button.x = 0.739125 * WIN_WIDTH;
			layout->rotate_button.y = (layout->bar.line.y - layout->rotate_button.h) / 2;
		}
	} else if (ebook->layout_orientation == PORTRAIT) {
		layout->bar.line.x = WIN_WIDTH - (WIN_WIDTH / 28);
		layout->bar.line.y = WIN_HEIGHT / 20;
		layout->bar.line.w = 2;
		layout->bar.line.h = WIN_HEIGHT - (layout->bar.line.y * 2);

		// coord to touch bar
		layout->bar.back_bar.x = layout->bar.line.x;
		layout->bar.back_bar.y = 0;
		layout->bar.back_bar.w = WIN_WIDTH - layout->bar.line.x;
		layout->bar.back_bar.h = WIN_HEIGHT;

		// coord app title
		TTF_SizeText(graphic->ttf->font_medium, APP_NAME, &w, &h);
		layout->app_title.x = 1274;
		layout->app_title.y = (WIN_HEIGHT / 2) - (w /2);

		if (ebook->read_mode == true) {
			// progress bar coord
			layout->progress_bar.x = 1243;
			layout->progress_bar.y = (WIN_HEIGHT / 2) - (300 / 2);
			layout->progress_bar.w = 2;
			layout->progress_bar.h = 300;

			// coord touch next_page
			layout->prev_page_button.x = 0;
			layout->prev_page_button.y = 0;
			layout->prev_page_button.w = WIN_WIDTH - (WIN_WIDTH - layout->bar.line.x);
			layout->prev_page_button.h = WIN_HEIGHT / 2;

			// coord touch prev_page
			layout->next_page_button.x = 0;
			layout->next_page_button.y = WIN_HEIGHT - (WIN_HEIGHT / 2);
			layout->next_page_button.w = WIN_WIDTH - (WIN_WIDTH - layout->bar.line.x);
			layout->next_page_button.h = WIN_HEIGHT / 2;
		
			// coord rotate button
			layout->rotate_button.x = 1240;
			layout->rotate_button.y = 105;
			layout->rotate_button.w = 34;
			layout->rotate_button.h = 58;

			// coord exit button
			layout->exit_home.x = 1240;
			layout->exit_home.y = 600;
			layout->exit_home.w = 34;
			layout->exit_home.h = 58;
		}
	}

	if (ebook->read_mode == false) {
		// Progress bar coord
		layout->progress_bar.x = layout->cover.x;
		layout->progress_bar.y = 700;
		layout->progress_bar.w = layout->cover.w;
		layout->progress_bar.h = 2;

		// coord touch next_page
		layout->next_page_button.x = layout->cover.x + layout->cover.w;
		layout->next_page_button.y = layout->bar.line.y;
		layout->next_page_button.w = WIN_WIDTH - (layout->cover.x + layout->cover.w);
		layout->next_page_button.h = WIN_HEIGHT - layout->bar.line.y;

		// coord touch prev_page
		layout->prev_page_button.x = 0;
		layout->prev_page_button.y = layout->bar.line.y;
		layout->prev_page_button.w = layout->cover.x;
		layout->prev_page_button.h = WIN_HEIGHT - layout->bar.line.y;
	} else if (ebook->read_mode == true) {
	}
}

bool	init_layout(void)
{
	// Set default orientation
	ebook->layout_orientation = LANDSCAPE;

	// show bar default
	layout->show_bar = true;

	// Cover position in home menu
	layout->cover.w = COVER_WIDTH;
	layout->cover.h = COVER_HEIGHT;
	layout->cover.x = (WIN_WIDTH / 2) - (COVER_WIDTH / 2);
	layout->cover.y = (WIN_HEIGHT / 2) - (COVER_HEIGHT / 2) + 20;

	set_layout();

	log_info("init_layout() [Success]");
	return (true);
}

void	deinit_layout(void)
{
	free(trans);
	trans = NULL;
	log_info("deinit_layout() [Success]");
}

void	portrait_default(int type)
{
	float	ratio = 0;

	if (type == CBR) {
		ratio = (float)cbr->image->w / (float)WIN_HEIGHT;

		layout->page.h = cbr->image->h / ratio;
		layout->page.w = cbr->image->w / ratio;
		layout->page.x = (WIN_WIDTH / 2) - (layout->page.w / 2);
		layout->page.y = (WIN_HEIGHT /2) - (layout->page.h / 2);
	} else {
		trans->zoom = (WIN_HEIGHT * 100) / trans->bounds.x1;

		trans->ctm = fz_scale(trans->zoom / 100, trans->zoom / 100);
		trans->ctm = fz_pre_rotate(trans->ctm, 90);

		layout->page.x = (WIN_WIDTH - ((trans->zoom /100) * trans->bounds.y1)) / 2;	// calculate middle of X
		layout->page.y = 0;															// y pos must to begin in 0
		layout->page.w = (trans->zoom / 100) * trans->bounds.y1;						// add zoom percentage
		layout->page.h = (trans->zoom / 100) * trans->bounds.x1;						// add zoom percentage
	}

	log_info("portrait_default() [Success]");
}

void	landscape_default(int type)
{
	float	ratio = 0;

	if (type == CBR) {
		ratio = (float)cbr->image->h / (float)WIN_HEIGHT;

		layout->page.h = cbr->image->h / ratio;
		layout->page.w = cbr->image->w / ratio;
		layout->page.x = (WIN_WIDTH / 2) - (layout->page.w / 2);
		layout->page.y = 0;
	} else {
	// calculate to fit in Y (Default zoom)
		trans->zoom = (WIN_HEIGHT * 100) / trans->bounds.y1;

		// set zoom and rotation
		trans->ctm = fz_scale(trans->zoom / 100, trans->zoom / 100);
		trans->ctm = fz_pre_rotate(trans->ctm, 0);

		// Center in middle of the screen X and Y
		layout->page.x = (WIN_WIDTH - ((trans->zoom /100) * trans->bounds.x1)) / 2;	// calculate middle of X
		layout->page.y = 0;															// y pos must to begin in 0
		layout->page.w = (trans->zoom / 100) * trans->bounds.x1;						// add zoom percentage
		layout->page.h = (trans->zoom / 100) * trans->bounds.y1;						// add zoom percentage
	}

	log_info("landscape_default() [Success]");
}
