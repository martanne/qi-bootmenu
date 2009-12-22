#include <stdbool.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include "config.h"
#include "kexec.h"
#include "util.h"

/* XXX: replace hardcoded values with runtime information */ 
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 640

/* drawing related stuff */
static Ecore_Evas *ee;
static Evas *evas;

static void poweroff(void *data, Evas *evas, Evas_Object *obj, void *event);
static void bootitem_clicked(void *data, Evas *evas, Evas_Object *item, void *event); 

typedef struct {
	const char *text;
	const char *logo;
	void(*callback)(void*, Evas*, Evas_Object*, void *);
} MenuItem;

MenuItem menu[] = {
	{ "Power Off", POWEROFF_LOGO, poweroff },
};

#include "gui-list.c"
#include "gui-grid.c"

typedef struct {
	char option;
	void(*show)(Eina_List *systems);
	void(*select)(Evas_Object *item);
} Gui;

Gui guis[] = {
	{ 'l', gui_list, gui_list_select_item },
	{ 'g', gui_grid, NULL },
};

/* if no option is passed in then use the first entry */
static Gui *gui = &guis[0];

static void poweroff(void *data, Evas *evas, Evas_Object *item, void *event) {
	if (gui->select)
		gui->select(item);
	system("poweroff");
}

static void bootitem_clicked(void *data, Evas *evas, Evas_Object *item, void *event) {
	if (gui->select)
		gui->select(item);
	boot_kernel((BootItem*)data);
	/* XXX: shouldn't be reached, display an error message? */
}

static bool gui_init(){
	if (!ecore_init() || !ecore_evas_init())
		return false;

	/* XXX: fixed dimensions */
	ee = ecore_evas_new(NULL, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL);

	if (!ee)
		return false;

	ecore_evas_title_set(ee, APPNAME);
	ecore_evas_borderless_set(ee, 0);
	ecore_evas_show(ee);

	evas = ecore_evas_get(ee);
	if (!evas)
		return false;

	evas_font_path_append(evas, FONT_PATH);

	return true;
}

int gui_show(int argc, char **argv, Eina_List *dev_ignore) {

	int arg, g;

	for (arg = 1; arg < argc; arg++) {
		if (argv[arg][0] != '-')
			continue;
		for (g = 0; g < countof(guis); g++) {
			if (argv[arg][1] == guis[g].option)
				gui = &guis[g];
		}
	}

	if (!gui_init()) {
		eprint("Couldn't init GUI\n");
		return 1;
	}

	/* search for system images to boot and display them */
	gui->show(scan_system(dev_ignore));

	ecore_main_loop_begin();

	return 0;
}
