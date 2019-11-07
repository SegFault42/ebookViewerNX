#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include <SDL2/SDL.h>

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

#endif
