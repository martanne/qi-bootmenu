#define APPNAME "Qi Bootmenu"
/* whether we should scan /lib/modules/`uname -r`/ for fs modules */
#define CONFIG_SUPPORT_KERNEL_FS_MODULES 0
/* partitions will be mounted under $MOUNTPOINT/$PARTITION */
#define MOUNTPOINT "/mnt"
/* binaries to exec(3) */
#define MODPROBE "/sbin/modprobe"
#define KEXEC "/sbin/kexec"
/* XXX: use --command-line= instead? */
#define KEXEC_CMDLINE "--append="
#define FONT_PATH "/usr/share/fonts"
#define FONT "Vera"
#define FONT_SIZE 20
/* logo which is displayed if no logo is found in the partion */
#define DEFAULT_LOGO "/usr/share/qi-bootmenu/defaultlogo.png"
/* XXX: replace hardcoded values with runtime information */ 
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 640

/* static menu entries */
MenuItem menu[] = {
	{ "Power Off", "/usr/share/qi-bootmenu/poweroff.png", poweroff },
};

#define LIST_LOGO_WIDTH 100
#define LIST_LOGO_HEIGHT 80

#define GRID_LOGO_WIDTH 200
#define GRID_LOGO_HEIGHT 180

#include "gui-list.c"
#include "gui-grid.c"

Gui guis[] = {
	{ 'l', gui_list, gui_list_select_item },
	{ 'g', gui_grid, NULL },
};
