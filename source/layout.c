#include "common.h"

extern t_transform	*trans;
extern t_ebook		*ebook;
extern t_layout		*layout;
extern t_graphic	*graphic;

void	set_layout(void)
{
	if (ebook->read_mode == true && ebook->layout_orientation == PORTRAIT) {
		layout->help_button.w = 34;
		layout->help_button.h = 58;
		layout->help_button.x = 1240;
		layout->help_button.y = 550;

		
	} else if ((ebook->read_mode == true && ebook->layout_orientation == LANDSCAPE) || ebook->read_mode == false) {
		// line
		layout->line.x = WIN_WIDTH - (WIN_WIDTH / 28);
		layout->line.y = WIN_HEIGHT / 20;
		layout->line.w = 2;
		layout->line.h = WIN_HEIGHT - (layout->line.y * 2);

		// help button
		layout->help_home.w = WIN_WIDTH / 14;
		layout->help_home.h = layout->line.y / 1.30;
		layout->help_home.x = 0.8203125 * WIN_WIDTH;
		layout->help_home.y = (layout->line.y - layout->help_home.h) / 2;

		// exit button
		layout->exit_home.w = WIN_WIDTH / 14;
		layout->exit_home.h = layout->line.y / 1.30;
		layout->exit_home.x = 0.8984375 * WIN_WIDTH;
		layout->exit_home.y = (layout->line.y - layout->exit_home.h) / 2;

		// app title
		layout->app_title.x = 1090;
		layout->app_title.y = (WIN_HEIGHT / 2);
	}
}

bool	init_layout(void)
{
	trans = (t_transform *)calloc(sizeof(t_transform), 1);
	if (trans == NULL) {
		log_info("init_layout() [Failure]");
		return (false);
	}

	// Cover position in home menu
	layout->cover.w = COVER_WIDTH;
	layout->cover.h = COVER_HEIGHT;
	layout->cover.x = (WIN_WIDTH / 2) - (COVER_WIDTH / 2);
	layout->cover.y = (WIN_HEIGHT / 2) - (COVER_HEIGHT / 2) + 20;

	/*set_layout();*/

	// show bar default
	layout->show_bar = true;

	log_info("init_layout() [Success]");
	return (true);
}

void	deinit_layout(void)
{
	free(trans);
	trans = NULL;
	log_info("deinit_layout() [Success]");
}

void	portrait_default(void)
{
	trans->zoom = (WIN_HEIGHT * 100) / trans->bounds.x1;

	trans->ctm = fz_scale(trans->zoom / 100, trans->zoom / 100);
	trans->ctm = fz_pre_rotate(trans->ctm, 90);

	trans->dstrect.x = (WIN_WIDTH - ((trans->zoom /100) * trans->bounds.y1)) / 2;	// calculate middle of X
	trans->dstrect.y = 0;															// y pos must to begin in 0
	trans->dstrect.w = (trans->zoom / 100) * trans->bounds.y1;						// add zoom percentage
	trans->dstrect.h = (trans->zoom / 100) * trans->bounds.x1;						// add zoom percentage

	log_info("portrait_default() [Success]");
}

void	landscape_default(void)
{
	// calculate to fit in Y (Default zoom)
	trans->zoom = (WIN_HEIGHT * 100) / trans->bounds.y1;

	// set zoom and rotation
	trans->ctm = fz_scale(trans->zoom / 100, trans->zoom / 100);
	trans->ctm = fz_pre_rotate(trans->ctm, 0);

	// Center in middle of the screen X and Y
	trans->dstrect.x = (WIN_WIDTH - ((trans->zoom /100) * trans->bounds.x1)) / 2;	// calculate middle of X
	trans->dstrect.y = 0;															// y pos must to begin in 0
	trans->dstrect.w = (trans->zoom / 100) * trans->bounds.x1;						// add zoom percentage
	trans->dstrect.h = (trans->zoom / 100) * trans->bounds.y1;						// add zoom percentage

	log_info("landscape_default() [Success]");
}
