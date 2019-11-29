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
	extract_cbr("/a.cbr");

	home_page();

	deinit_all();

	return (0);
}
