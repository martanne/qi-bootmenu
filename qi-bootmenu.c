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

#include <stdbool.h>
#include <Eina.h>
#include "qi-bootmenu.h"
#include "kexec.h"

int main(int argc, char **argv) {

	if (!eina_mempool_init() || !eina_list_init())
		return 1;

	if (argc > 1 && argv[1][0] == '-') {
		switch (argv[1][1]) {
			case 'd':
				diagnostics();
				exit(0);
		}
	}

	eina_list_shutdown();

	return 0;
}
