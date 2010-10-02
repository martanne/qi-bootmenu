static void gui_list_draw_item(const char *text, const char *logo, void(*callback)(void*, Evas*, Evas_Object*, void *),
                               void *data, int x, int y) {

	Evas_Object *ebox, *elogo, *etext, *erect;

	elogo = evas_object_image_add(evas);
	evas_object_image_file_set(elogo, logo, NULL);
	evas_object_image_fill_set(elogo, 0, 0, LIST_LOGO_WIDTH, LIST_LOGO_HEIGHT);
	evas_object_resize(elogo, LIST_LOGO_WIDTH, LIST_LOGO_HEIGHT);
	evas_object_show(elogo);

	etext = evas_object_text_add(evas);
	evas_object_text_font_set(etext, FONT, FONT_SIZE);
	evas_object_text_text_set(etext, text);
	evas_object_color_set(etext, LIST_FONT_COLOR, 255);
	evas_object_show(etext);

	ebox = evas_object_box_add(evas);
	evas_object_box_align_set(ebox, 0, 0.5);
	evas_object_box_padding_set(ebox, 10, 10);
	evas_object_move(ebox, x, y);
	evas_object_resize(ebox, SCREEN_WIDTH, LIST_LOGO_HEIGHT);
	evas_object_box_append(ebox, elogo);
	evas_object_box_append(ebox, etext);

	/* this is just an invisible rect to catch all mouse down events in the area */
	erect = evas_object_rectangle_add(evas);
	evas_object_move(erect, x, y);
	evas_object_resize(erect, SCREEN_WIDTH, LIST_LOGO_HEIGHT);
	evas_object_color_set(erect, 0, 0, 0, 0);
	evas_object_event_callback_add(erect, EVAS_CALLBACK_MOUSE_DOWN, callback, data);
	evas_object_data_set(erect, "text", etext);

	evas_object_show(erect);
	evas_object_show(ebox);
}

static void gui_list_select_item(Evas_Object *box) {
	Evas_Object *text = evas_object_data_get(box, "text");
	evas_object_color_set(text, LIST_FONT_SELECTED_COLOR, 255);
}

static void gui_list_show_error(const char *errstr, va_list ap) {
	static Evas_Object *etext;
	char buf[128];

	if (!etext) {
		Evas_Object *ebox = evas_object_box_add(evas);
		evas_object_box_align_set(ebox, 0.5, 0.5);
		evas_object_box_padding_set(ebox, 10, 10);
		evas_object_move(ebox, 0, SCREEN_HEIGHT - LIST_LOGO_HEIGHT);
		evas_object_resize(ebox, SCREEN_WIDTH, LIST_LOGO_HEIGHT);

		etext = evas_object_text_add(evas);
		evas_object_text_font_set(etext, FONT, FONT_SIZE);
		evas_object_color_set(etext, LIST_FONT_COLOR, 255);

		evas_object_box_append(ebox, etext);
		evas_object_show(ebox);
	}

	vsnprintf(buf, sizeof buf, errstr, ap);

	evas_object_text_text_set(etext, buf);
	evas_object_show(etext);
}

static void gui_list(Eina_List *systems) {
	Eina_List *l;
	unsigned int i, y = 0;
	BootItem *s;

	EINA_LIST_FOREACH(systems, l, s) {
		if (!strcmp(menu[0].data, s->dev))
			menu[0].text = NULL;
		gui_list_draw_item(s->dev, s->logo, gui_bootitem_clicked, s, 0, y);
		y += LIST_LOGO_HEIGHT;
	}
	/* add pre defined menu entries */
	for (i = 0; i < countof(menu); i++) {
		if (!menu[i].text)
			continue;
		gui_list_draw_item(menu[i].text, menu[i].logo, menu[i].callback, menu[i].data, 0, y);
		y += LIST_LOGO_HEIGHT;
	}
}
