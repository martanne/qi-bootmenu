static void gui_show_error(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);

	if (gui->error)
		gui->error(errstr, ap);

	vfprintf(stderr, errstr, ap);
	va_end(ap);
}

static void gui_item_clicked(void *data, Evas *evas, Evas_Object *item, void *event) {
	MenuItem *menu = data;
	if (gui->select)
		gui->select(item);
	menu->callback(menu->data);
	if (gui->deselect)
		gui->deselect(item);
}

static void poweroff(void *data) {
	Eina_List *l;
	BootItem *s;

	EINA_LIST_FOREACH(systems, l, s) {
		umount(s->dev + sstrlen("/dev/"));
	}
	system("poweroff");
}

static void boot_nand(void *data) {
	BootItem *nand = scan_partition((const char *)data);
	if (!nand) {
		gui_show_error("No kernel found in NAND Flash.\n");
		return;
	}

	boot_kernel(nand);
}

static void gui_bootitem_clicked(void *data) {
	boot_kernel((BootItem*)data);
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
