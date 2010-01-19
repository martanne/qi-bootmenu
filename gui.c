static void poweroff(void *data, Evas *evas, Evas_Object *item, void *event) {
	if (gui->select)
		gui->select(item);
	system("poweroff");
}

static void gui_bootitem_clicked(void *data, Evas *evas, Evas_Object *item, void *event) {
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
