/*
 *  qi-bootmenu - A kexec based bootloader with an elementary based GUI
 *
 *  Copyright (c) 2009-2010 Marc Andre Tanner <mat@brain-dump.org>
 *
 *  qi-bootmenu shares the basic idea with kexecboot which is written by:
 *
 *  Copyright (c) 2008-2009 Yuri Bushmelev <jay4mail@gmail.com>
 *  Copyright (c) 2008 Thomas Kunze <thommycheck@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>

#define countof(arr) (sizeof(arr) / sizeof((arr)[0]))
#define sstrlen(str) (sizeof(str) - 1)

static void eprint(const char *errstr, ...);

#ifdef NDEBUG
 #define debug(format, args...)
#else
 #define debug eprint
#endif

typedef struct {
	const char *fs;
	const char *dev;
	const char *kernel;
	const char *cmdline;
	const char *logo;
} BootItem;

typedef struct {
	const char *text;
	const char *logo;
	void(*callback)(void*, Evas*, Evas_Object*, void *);
} MenuItem;

/* menu actions */
static void poweroff(void *data, Evas *evas, Evas_Object *obj, void *event);

/* drawing related stuff */
static Ecore_Evas *ee;
static Evas *evas;

typedef struct {
	char option;
	void(*show)(Eina_List *systems);
	void(*select)(Evas_Object *item);
} Gui;

/* functions available to gui modules */
static void gui_bootitem_clicked(void *data, Evas *evas, Evas_Object *item, void *event); 

#include "config.h"

/* if no option is passed in then use the first entry */
static Gui *gui = &guis[0];

#include "kexec.c"
#include "gui.c"

static void eprint(const char *errstr, ...) {
	va_list ap;
	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
}

static void usage() {
	eprint("usage: qi-bootmenu [-d] [-i ...]\n");
	exit(1);
}

int main(int argc, char **argv) {

	Eina_List *dev_ignore = NULL; /* partitions to ignore */
	bool diag = false;
	int arg;
	unsigned int g;

	if (!eina_init())
		return 1;

	for (arg = 1; arg < argc; arg++) {
		if (argv[arg][0] != '-')
			usage();
			 
		switch (argv[arg][1]) {
			case 'd':
				diag = true;
				break;
			case 'i':
				if (arg + 1 >= argc)
					usage();
				dev_ignore = eina_list_append(dev_ignore, argv[++arg]);
				break;
			default:
				for (g = 0; g < countof(guis); g++) {
					if (argv[arg][1] == guis[g].option)
						gui = &guis[g];
				}
				break;
		}
	}

	if (diag) {
		diagnostics(dev_ignore);
		return 0;
	}

	if (!gui_init()) {
		eprint("Couldn't init GUI\n");
		return 1;
	}

	/* search for system images to boot and display them */
	gui->show(scan_system(dev_ignore));

	debug("entering main loop\n");

	ecore_main_loop_begin();

	return 0;
}
