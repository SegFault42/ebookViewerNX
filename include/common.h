#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifdef __NXLINK__
	#include <switch.h>
#endif

#ifdef __TWILI__
	#include <twili.h>
#endif

#include "log.h"

#include "ebook.h"
#include "graphic.h"
#include "layout.h"
#include "home.h"
#include "array.h"
#include "get_next_line.h"
#include "controller.h"

#endif
