static void gui_grid_draw_item(const char *logo, void(*callback)(void*, Evas*, Evas_Object*, void *),
                               void *data, int x, int y) {

	Evas_Object *elogo = evas_object_image_add(evas);
	evas_object_image_file_set(elogo, logo, NULL);
	evas_object_image_fill_set(elogo, 0, 0, GRID_LOGO_WIDTH, GRID_LOGO_HEIGHT);
	evas_object_move(elogo, x, y);
	evas_object_resize(elogo, GRID_LOGO_WIDTH, GRID_LOGO_HEIGHT);
	evas_object_show(elogo);

	evas_object_event_callback_add(elogo, EVAS_CALLBACK_MOUSE_UP, callback, data);
}

static void gui_grid(Eina_List *systems) {
	/* Divide the screen into 6ths */
	Evas_Object *eline;
	eline = evas_object_line_add(evas);
	evas_object_line_xy_set(eline, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);
	evas_object_show(eline);
	eline = evas_object_line_add(evas);
	evas_object_line_xy_set(eline, 0, SCREEN_HEIGHT/3, SCREEN_WIDTH, SCREEN_HEIGHT/3);
	evas_object_show(eline);
	eline = evas_object_line_add(evas);
	evas_object_line_xy_set(eline, 0, 2*(SCREEN_HEIGHT/3), SCREEN_WIDTH, 2*(SCREEN_HEIGHT/3) );
	evas_object_show(eline);

	Eina_List *l;
	int i = 0, x, y;
	BootItem *s;

	EINA_LIST_FOREACH(systems, l, s) {
		x = (i%2 == 0) ? 20 : 260;
		y = 16 + ((SCREEN_HEIGHT/3) * (i/2));
		gui_grid_draw_item(s->logo, gui_bootitem_clicked, s, x, y);
		i++;
	}

	for (i = 0; i < countof(menu); i++) {
		x = ((5-i)%2 == 0) ? 20 : 260;
		y = 16 + ((SCREEN_HEIGHT/3) * ((5-i)/2));
		gui_grid_draw_item(menu[i].logo, menu[i].callback, NULL, x, y);
	}
}
