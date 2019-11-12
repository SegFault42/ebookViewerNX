#include "common.h"

extern t_transform	*trans;
extern t_ebook		*ebook;
extern t_layout		*layout;;

bool	init_layout(void)
{
	trans = (t_transform *)calloc(sizeof(t_transform), 1);
	if (trans == NULL) {
		log_info("init_layout() [Failure]");
		return (false);
	}

	// Cover position in home menu
	layout->cover_pos.w = COVER_WIDTH;
	layout->cover_pos.h = COVER_HEIGHT;
	layout->cover_pos.x = (WIN_WIDTH / 2) - (COVER_WIDTH / 2);
	layout->cover_pos.y = (WIN_HEIGHT / 2) - (COVER_HEIGHT / 2) + 20;

	// Exit button position in home menu
	layout->exit_home[0].x = 1150;
	layout->exit_home[0].y = 10;
	layout->exit_home[1].x = 1240;
	layout->exit_home[1].y = 10;
	layout->exit_home[2].x = 1240;
	layout->exit_home[2].y = 45;
	layout->exit_home[3].x = 1150;
	layout->exit_home[3].y = 45;
	layout->exit_home[4].x = 1150;
	layout->exit_home[4].y = 10;

	// Line in top
	layout->line[0].x = 40;
	layout->line[0].y = 55;
	layout->line[1].x = 1240;
	layout->line[1].y = 55;
	layout->line[2].x = 1240;
	layout->line[2].y = 56;
	layout->line[3].x = 40;
	layout->line[3].y = 56;

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
