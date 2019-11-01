#include "common.h"

extern t_transform	*trans;

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

/*void	landscape_reset()*/
/*{*/

	/*// get size of page*/
    /*page = fz_load_page(ebook->ctx, ebook->doc, current_page);*/
	/*bounds = fz_bound_page(ebook->ctx, page);*/
	/*fz_drop_page(ebook->ctx, page);*/

	/*// calculate to fit in Y (Default zoom)*/
	/*zoom = (WIN_HEIGHT * 100) / bounds.y1;*/

	/*// set zoom and rotation*/
	/*ctm = fz_scale(zoom / 100, zoom / 100);*/
	/*ctm = fz_pre_rotate(ctm, rotate);*/

	/*SDL_Rect dstrect = {(WIN_WIDTH - ppm->w) / 2, 0, ppm->w, ppm->h};*/
/*}*/
