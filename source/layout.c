#include "common.h"

extern t_transform	*trans;
extern t_ebook		*ebook;
extern t_layout		*layout;
extern t_graphic	*graphic;

void	set_layout(void)
{
	// Bar coord
	if (ebook->layout_orientation == LANDSCAPE) {
		layout->bar.line.x = WIN_WIDTH / 32;
		layout->bar.line.y = WIN_HEIGHT / 12;
		layout->bar.line.w = WIN_WIDTH - (layout->bar.line.x * 2);
		layout->bar.line.h = 2;

		// coord to touch bar
		layout->bar.back_bar.x = 0;
		/*layout->bar.back_bar.y = 0;*/
		layout->bar.back_bar.w = WIN_WIDTH;
		layout->bar.back_bar.h = layout->bar.line.y;
	} else if (ebook->layout_orientation == PORTRAIT) {
		layout->bar.line.x = WIN_WIDTH - (WIN_WIDTH / 28);
		layout->bar.line.y = WIN_HEIGHT / 20;
		layout->bar.line.w = 2;
		layout->bar.line.h = WIN_HEIGHT - (layout->bar.line.y * 2);

		// coord to touch bar
		layout->bar.back_bar.x = layout->bar.line.x;
		/*layout->bar.back_bar.y = 0;*/
		layout->bar.back_bar.w = WIN_WIDTH - layout->bar.line.x;
		layout->bar.back_bar.h = WIN_HEIGHT;
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
