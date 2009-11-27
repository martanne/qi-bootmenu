#include <stdbool.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include "config.h"
#include "kexec.h"
#include "util.h"

/* drawing related stuff */
static Ecore_Evas *ee;
static Evas *evas;

void poweroff(void *data, Evas *evas, Evas_Object *obj, void *event);

typedef struct {
	const char *text;
	const char *logo;
	void(*callback)(void*, Evas*, Evas_Object*, void *);
} MenuItem;

MenuItem menu[] = {
	{ "Power Off", POWEROFF_LOGO, poweroff },
};

void poweroff(void *data, Evas *evas, Evas_Object *obj, void *event) {
	system("poweroff");
}

static void bootitem_clicked(void *data, Evas *evas, Evas_Object *obj, void *event){
	boot_kernel((BootItem*)data);
	/* XXX: shouldn't be reached, display an error message? */
}

static bool canvas_init(){
	if (!ecore_init() || !ecore_evas_init())
		return false;

	/* XXX: fixed dimensions */
	if (getenv("DISPLAY"))
		ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 480, 640);
	else
		ee = ecore_evas_fb_new(NULL, 0, 480, 640);
		
	if (!ee)
		return false;

	ecore_evas_title_set(ee, APPNAME);
	ecore_evas_borderless_set(ee, 0);
	ecore_evas_show(ee);

	evas = ecore_evas_get(ee);
	evas_font_path_append(evas, FONT_PATH);

	return true;
}

static int draw_item(const char *text, const char *logo, void(*callback)(void*, Evas*, Evas_Object*, void *),
              void *data, int x, int y) {

	Evas_Object *ebox, *elogo, *etext;

	elogo = evas_object_image_add(evas);
	evas_object_image_file_set(elogo, logo, NULL);
	evas_object_image_fill_set(elogo, 0, 0, LOGO_WIDTH, LOGO_HEIGHT);
	evas_object_resize(elogo, LOGO_WIDTH, LOGO_HEIGHT);
	evas_object_show(elogo);

	etext = evas_object_text_add(evas);
	evas_object_text_font_set(etext, FONT, FONT_SIZE);
	evas_object_text_text_set(etext, text);
	evas_object_show(etext);

	ebox = evas_object_box_add(evas);
	evas_object_box_align_set(ebox, 0, 0.5);
	evas_object_box_padding_set(ebox, 10, 10);
	evas_object_move(ebox, x, y);
	evas_object_resize(ebox, 680, 50);
	evas_object_box_append(ebox, elogo);
	evas_object_box_append(ebox, etext);
	evas_object_event_callback_add(ebox, EVAS_CALLBACK_MOUSE_UP, callback, data);
	
	evas_object_show(ebox);

	return 0;
}

int gui(int argc, char **argv) {

	if (!canvas_init()) {
		eprint("Couldn't init GUI\n");
		return 1;
	}

	/* search for system images to boot and display them in a list */
	Eina_List *l, *systems = scan_system();
	int i, y = 50;
	BootItem *s;

	EINA_LIST_FOREACH(systems, l, s) {
		draw_item(s->dev, s->logo, bootitem_clicked, s,  10, y);
		y += LOGO_HEIGHT + 10;
	}

	/* add pre defined menu entries */
	for (i = 0; i < countof(menu); i++) {
		draw_item(menu[i].text, menu[i].logo, menu[i].callback, NULL,  10, y);
		y += LOGO_HEIGHT + 10;
	}

	ecore_main_loop_begin();

	return 0;
}
