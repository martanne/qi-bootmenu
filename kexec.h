#include <stdbool.h>
#include <Eina.h>

typedef struct {
	const char *dev;
	const char *kernel;
	const char *cmdline;
	const char *logo;
} BootItem;

void diagnostics();
Eina_List* scan_system();
char* get_kernel_cmdline(BootItem *i);
bool boot_kernel(BootItem *i);
