#include "common.h"

extern t_transform	*trans;
extern t_ebook		*ebook;
extern t_layout		*layout;
extern t_graphic	*graphic;

bool	init_layout(void)
{
	int w = 0;
	int h = 0;

	trans = (t_transform *)calloc(sizeof(t_transform), 1);
	if (trans == NULL) {
		log_info("init_layout() [Failure]");
		return (false);
	}

   /* // title app coord*/
	/*TTF_SizeText(graphic->ttf->font_large, APP_NAME, &w, &h);*/
	/*layout->app_title.x = (WIN_WIDTH / 2) - (w / 2);*/
	/*layout->app_title.y = WIN_HEIGHT / 90;*/

	/*[>// Line in top<]*/
	/*[>layout->line.x = WIN_WIDTH / 32;<]*/
	/*[>layout->line.y = WIN_HEIGHT / 12;<]*/
	/*[>layout->line.w = WIN_WIDTH - (layout->line.x * 2);<]*/
	/*[>layout->line.h = 2;<]*/

	// Cover position in home menu
	layout->cover.w = COVER_WIDTH;
	layout->cover.h = COVER_HEIGHT;
	layout->cover.x = (WIN_WIDTH / 2) - (COVER_WIDTH / 2);
	layout->cover.y = (WIN_HEIGHT / 2) - (COVER_HEIGHT / 2) + 20;

	// Help home
	/*layout->help_home.x = 1150;*/
	/*layout->help_home.y = 10;*/
	/*layout->help_home.w = 90;*/
	/*layout->help_home.h = 35;*/

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
