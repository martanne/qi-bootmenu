/*
 *  qi-bootmenu - A kexec based bootloader with an elementary based GUI
 *
 *  Copyright (c) 2009 Marc Andre Tanner <mat@brain-dump.org>
 *
 *  qi-bootmenu shares the basic idea and some small code snippets with
 *  kexecboot which is written by:
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

#include "kexec.h"
#include "gui.h"
#include "util.h"

static void usage() {
	eprint("usage: qi-bootmenu [-d] [-i ...]\n");
	exit(1);
}

int main(int argc, char **argv) {

	Eina_List *dev_ignore = NULL; /* partitions to ignore */
	bool diag = false;
	int arg;

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
		}
	}

	if (diag) {
		diagnostics(dev_ignore);
		exit(0);
	}

	return gui_show(argc, argv, dev_ignore);
}
