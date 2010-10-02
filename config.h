#define APPNAME "Qi Bootmenu"
/* whether we should scan /lib/modules/`uname -r`/ for fs modules */
#define CONFIG_SUPPORT_KERNEL_FS_MODULES 0
/* used to locate kernel ({u,z}Image-$MACHINE.bin) and append file (append-$MACHINE)
 * can be overriden on the command line -m */ 
#define DEFAULT_MACHINE "GTA02"
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
	{ "NAND Flash", DEFAULT_LOGO, boot_nand, "/dev/mtdblock6" },
	{ "Power Off", "/usr/share/qi-bootmenu/poweroff.png", poweroff, NULL },
};

#define LIST_FONT_COLOR RGB(255, 255, 255)
#define LIST_FONT_SELECTED_COLOR RGB(255, 102, 0)
#define LIST_LOGO_WIDTH 100
#define LIST_LOGO_HEIGHT 80

#define GRID_LOGO_WIDTH 200
#define GRID_LOGO_HEIGHT 180

#include "gui-list.c"

Gui guis[] = {
	{ 'l', gui_list, gui_list_select_item, gui_list_deselect_item, gui_list_show_error },
};
