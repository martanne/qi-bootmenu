#define LIST_LOGO_WIDTH 100
#define LIST_LOGO_HEIGHT 80

static void gui_list_draw_item(const char *text, const char *logo, void(*callback)(void*, Evas*, Evas_Object*, void *),
                      void *data, int x, int y) {

	Evas_Object *ebox, *elogo, *etext;

	elogo = evas_object_image_add(evas);
	evas_object_image_file_set(elogo, logo, NULL);
	evas_object_image_fill_set(elogo, 0, 0, LIST_LOGO_WIDTH, LIST_LOGO_HEIGHT);
	evas_object_resize(elogo, LIST_LOGO_WIDTH, LIST_LOGO_HEIGHT);
	evas_object_show(elogo);

	etext = evas_object_text_add(evas);
	evas_object_text_font_set(etext, FONT, FONT_SIZE);
	evas_object_text_text_set(etext, text);
	evas_object_show(etext);

	ebox = evas_object_box_add(evas);
	evas_object_box_align_set(ebox, 0, 0.5);
	evas_object_box_padding_set(ebox, 10, 10);
	evas_object_move(ebox, x, y);
	evas_object_resize(ebox, SCREEN_WIDTH, LIST_LOGO_HEIGHT);
	evas_object_box_append(ebox, elogo);
	evas_object_box_append(ebox, etext);
	evas_object_event_callback_add(ebox, EVAS_CALLBACK_MOUSE_UP, callback, data);

	evas_object_show(ebox);
}

static void gui_list_select_item(Evas_Object *item) {
	Evas_Object *eline; 
	Evas_Coord x, y, w, h;
	evas_object_geometry_get(item, &x, &y, &w, &h);
	eline = evas_object_line_add(evas);
	evas_object_line_xy_set(eline, x, y, x+w, y);
	evas_object_show(eline);
	eline = evas_object_line_add(evas);
	evas_object_line_xy_set(eline, x, y, x, y+h);
	evas_object_show(eline);
	eline = evas_object_line_add(evas);
	evas_object_line_xy_set(eline, x+w, y, x+w, y+h);
	evas_object_show(eline);
	eline = evas_object_line_add(evas);
	evas_object_line_xy_set(eline, x, y+h, x+w, y+h);
	evas_object_show(eline);
}

static void gui_list(Eina_List *systems) {
	Eina_List *l;
	int i, y = 0;
	BootItem *s;

	EINA_LIST_FOREACH(systems, l, s) {
		gui_list_draw_item(s->dev, s->logo, bootitem_clicked, s, 0, y);
		y += LIST_LOGO_HEIGHT;
	}
	/* add pre defined menu entries */
	for (i = 0; i < countof(menu); i++) {
		gui_list_draw_item(menu[i].text, menu[i].logo, menu[i].callback, NULL,  0, y);
		y += LIST_LOGO_HEIGHT;
	}
}
