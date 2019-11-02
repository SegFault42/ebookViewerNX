#include "common.h"

extern t_transform	*trans;
extern t_ebook		*ebook;

bool	init_layout(void)
{
	trans = (t_transform *)calloc(sizeof(t_transform), 1);
	if (trans == NULL) {
		log_info("init_layout() [Failure]");
		return (false);
	}

	log_info("init_layout() [Success]");
	return (true);
}

void	deinit_layout(void)
{
	free(trans);
	trans = NULL;
	log_info("deinit_layout() [Success]");
}

void	landscape_default(void)
{
	// calculate to fit in Y (Default zoom)
	trans->zoom = (WIN_HEIGHT * 100) / trans->bounds.y1;

	// set zoom and rotation
	trans->ctm = fz_scale(trans->zoom / 100, trans->zoom / 100);
	trans->ctm = fz_pre_rotate(trans->ctm, trans->rotate);

	// Center in middle of the screen X and Y
	trans->dstrect.x = (WIN_WIDTH - ((trans->zoom /100) * trans->bounds.x1)) / 2;	// calculate middle of X 
	trans->dstrect.y = 0;															// y pos must to begin in 0 
	trans->dstrect.w = (trans->zoom / 100) * trans->bounds.x1;						// add zoom percentage
	trans->dstrect.h = (trans->zoom / 100) * trans->bounds.y1;						// add zoom percentage

	log_info("landscape_default() [Success]");
}
