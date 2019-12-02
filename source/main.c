// To compile linux version : gcc source/*.c -I include -L/usr/local/lib -lmupdf -lmupdf-third $(pkg-config --cflags --libs sdl2) -lm

#include <common.h>

t_graphic			*graphic = NULL;
t_transform			*trans = NULL;
t_ebook				*ebook = NULL;
t_controller		*controller = NULL;
t_layout			*layout = NULL;

int main(void)
{
	init_all();
	if (extract_cbr("/a.cbr", 0) == NULL) {
		log_warn("Extract cbr failed");
		draw_error("Extract cbr failed");
	}

	/*lblInitialize();*/
	/*lblSetCurrentBrightnessSetting(1.0);*/
	/*lblExit();*/

	home_page();

	deinit_all();

	return (0);
}
