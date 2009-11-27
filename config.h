#define APPNAME "Qi Bootmenu"
/* whether we should scan /lib/modules/`uname -r`/ for fs modules */
#define CONFIG_SUPPORT_KERNEL_FS_MODULES
/* partitions will be mounted under $MOUNTPOINT/$PARTITION */
#define MOUNTPOINT "/mnt"
#define MODPROBE "/sbin/modprobe"
#define KEXEC "/sbin/kexec"
/* XXX: use --command-line= instead? */
#define KEXEC_CMDLINE "--append="  
#define FONT_PATH "/usr/share/fonts"
#define FONT "Vera"
#define FONT_SIZE 20 
#define DEFAULT_LOGO "/usr/share/qi-bootmenu/defaultlogo.png"
#define POWEROFF_LOGO "/usr/share/qi-bootmenu/poweroff.png"
#define LOGO_WIDTH 100 
#define LOGO_HEIGHT 80
