#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <asm/setup.h> /* for COMMAND_LINE_SIZE */
#include <sys/fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include "fstype/fstype.h"

#ifndef COMMAND_LINE_SIZE
# define COMMAND_LINE_SIZE 256
#endif

/* filesystems built into the kernel */ 
static Eina_List *fs_core;
/* filesystems supported by kernel modules */
static Eina_List *fs_mods;
/* partitions read from /proc/partitions */
static Eina_List *partitions;
/* systems which were found to boot */
static Eina_List *systems;

static void skip_until(char** str, char c) {
	while (**str && **str != c)
		(*str)++;
}

static void skip_spaces(char **str) {
	while (isspace(**str))
		(*str)++;
}

/* originated from kexecboot, it is used instead of system(3) because
 * there is no shell involved which means it *could* be a bit faster
 */

static int fexecw(const char *path, char *const argv[], char *const envp[]) {
	pid_t pid;
	struct sigaction ignore, old_int, old_quit;
	sigset_t masked, oldmask;
	int status;

	/* Block SIGCHLD and ignore SIGINT and SIGQUIT before forking
	 * restore the original signal handlers afterwards. */

	ignore.sa_handler = SIG_IGN;
	sigemptyset(&ignore.sa_mask);
	ignore.sa_flags = 0;
	sigaction(SIGINT, &ignore, &old_int);
	sigaction(SIGQUIT, &ignore, &old_quit);

	sigemptyset(&masked);
	sigaddset(&masked, SIGCHLD);
	sigprocmask(SIG_BLOCK, &masked, &oldmask);

	pid = fork();

	if (pid < 0)
		return -1; /* can't fork */
	else if (pid == 0) { /* child process */
		sigaction(SIGINT, &old_int, NULL);
		sigaction(SIGQUIT, &old_quit, NULL);
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
		execve(path, (char *const *)argv, (char *const *)envp);
		_exit(127);
	}

	/* wait for our child and store it's exit status */
	waitpid(pid, &status, 0);

	/* restore signal handlers */
	sigaction(SIGINT, &old_int, NULL);
	sigaction(SIGQUIT, &old_quit, NULL);
	sigprocmask(SIG_SETMASK, &oldmask, NULL);

	return status;
}

/* returns all partions known to the system with it's full paths */
static Eina_List* get_partitions() {
	FILE *f = fopen("/proc/partitions", "r");

	if (!f)
		return NULL;

	Eina_List *partitions = NULL;

	char line[64];

	/* skip header */	
	fgets(line, sizeof(line), f);
	fgets(line, sizeof(line), f);

	while (fgets(line, sizeof(line), f)) {
		char *dev, *q, *p = line;
		long int blocks;
		int len;
		/* skip first two columns */
		skip_spaces(&p);
		skip_until(&p, ' ');
		skip_spaces(&p);
		skip_until(&p, ' ');
		/* read number of blocks */
		blocks = strtol(p, &q, 10);
		if (!blocks || p == q)
			continue;
		/* ignore small partitions < 10MB */
		if (blocks < 10 * 1024)
			continue;
		/* read partition name and remove trailing \n */
		skip_spaces(&q);
		p = q;
		skip_until(&q, '\n');
		*q = '\0';
		/* prepend /dev */
		len = q - p;
		dev = malloc(sstrlen("/dev/") + len + 1);
		strcpy(dev, "/dev/");
		strncat(dev, p, len);
		partitions = eina_list_append(partitions, dev);
	}

	fclose(f);
	return partitions;
}

static Eina_List* get_kernel_filesystems_builtin() {
	FILE *f = fopen("/proc/filesystems", "r");

	if (!f)
		return NULL;

	Eina_List *fs = NULL; 

	char line[32];

	while (fgets(line, sizeof(line), f)) {
		/* overwrite new line */
		line[strlen(line) - 1] = '\0';
		/* skip tab or "nodev\t" */
		fs = eina_list_append(fs, strdup(line + (line[0] == '\t' ? 1 : sstrlen("nodev\t"))));
	}

	fclose(f);
	return fs;
}

static Eina_List* get_kernel_filesystems_modules() {

#if CONFIG_SUPPORT_KERNEL_FS_MODULES

	Eina_List *fs = NULL; 
	struct utsname uts;
	FILE *f;
	char buf[1024], *p, *q;
	int len;

	if (uname(&uts))
		return NULL;

	len = snprintf(buf, sizeof(buf), "/lib/modules/%s/modules.dep", uts.release);

	if (len < 0)
		return NULL;

	f = fopen(buf, "r");
	if (!f)
		return NULL;

	/* calculate the length of the prefix for the modules, this assumes that the 
	 * modules are installed under the directroy where we found the modules.dep file.
	 */

	len -= sstrlen("modules.dep");

	/* Maybe we should readdir("/lib/modules/`uname -r`/kernel/fs") instead? */

	while (fgets(buf, sizeof(buf), f)) {
		/* skip lines which don't contain a colon */
		if (!(p = strchr(buf, ':')))
			continue;
		/* we are only interested in the part before '.ko:' */
		*(p - sstrlen(".ko")) = '\0';


		/* skip the prefix '/lib/modules/`uname -r`/' */
		p = buf + len;

		if (strncmp(p, "kernel/fs/", sstrlen("kernel/fs/")))
			continue;

		q = p += sstrlen("kernel/fs/");

		skip_until(&q, '/');
		*q = '\0';

		/* XXX: check for duplicates? */
		fs = eina_list_append(fs, strdup(p));
	}

	fclose(f);
	return fs;
#else
	return NULL;
#endif /* CONFIG_SUPPORT_KERNEL_FS_MODULES */
}

static bool is_supported_filesystem(const char *fs) {

	if (eina_list_search_unsorted(fs_core, EINA_COMPARE_CB(strcmp), fs))
		return true;

#if CONFIG_SUPPORT_KERNEL_FS_MODULES
	const char *modprobe[] = { MODPROBE , fs };
	if (eina_list_search_unsorted(fs_mods, EINA_COMPARE_CB(strcmp), fs) &&
	    fexecw(modprobe[0], (char *const *)modprobe, NULL) == 0) {
		debug("Successfully loaded kernel filesystem module: '%s'\n", fs);
		fs_mods = eina_list_remove(fs_mods, fs);
		fs_core = eina_list_append(fs_core, fs);
		return true;
	}
#endif

	return false;
}

static BootItem* scan_partition(const char *dev) {
	BootItem *sys = NULL;
	const char *mnt, *fs;
	char buf[COMMAND_LINE_SIZE];
	struct stat st;

	int fd = open(dev, O_RDONLY);
	if (fd < 0) {
		perror("open");
		return NULL;
	}
	
	if (identify_fs(fd, &fs, NULL, 0)) {
		eprint("Couldn't identify filesystem on '%s'\n", dev);
		goto out;
	}

	if (!is_supported_filesystem(fs)) {
		eprint("Unsupported filesystem '%s' on '%s'\n", fs, dev);
		goto out;
	}

	/* we chdir()-ed and now use relative paths */ 
	mnt = dev + sstrlen("/dev/");
	if (mkdir(mnt, 0755) && errno != EEXIST) {
		perror("mkdir");
		goto out;
	}

	if (mount(dev, mnt, fs, MS_RDONLY, NULL)) { 
		perror("mount");
		goto out;
	}

	snprintf(buf, sizeof buf, "%s/%s/boot/uImage-%s.bin", MOUNTPOINT, mnt, machine);
	if (stat(buf, &st)) {
		/* no uImage present now check for zImage */
		buf[sstrlen(MOUNTPOINT) + sstrlen("/") + strlen(mnt) + sstrlen("/boot/")] = 'z';
		if (stat(buf, &st)) {
			eprint("No kernel found at '%s'\n", buf);
			umount(mnt);
			goto out;
		}
	}

	sys = calloc(sizeof(BootItem), 1);
	sys->fs = fs;
	sys->dev = dev;
	sys->kernel = strdup(buf);
		
	snprintf(buf, sizeof buf, "%s/%s/boot/append-%s", MOUNTPOINT, mnt, machine);
	FILE *f = fopen(buf, "r");
	if (f) {
		fgets(buf, sizeof buf, f);
		sys->cmdline = strdup(buf);
		fclose(f);
	}

	snprintf(buf, sizeof buf, "%s/%s/boot/bootlogo.png", MOUNTPOINT, mnt);
	if (!stat(buf, &st))
		sys->logo = strdup(buf);
	else
		sys->logo = DEFAULT_LOGO;

out:
	close(fd);
	return sys;
}

static Eina_List* scan_system(Eina_List *dev_ignore) {

	Eina_List *l;
	const char *dev;

	if (systems)
		return systems;

	/* check if we need to read data from /proc first */
	if (!partitions)
		partitions = get_partitions();
	if (!fs_core)
		fs_core = get_kernel_filesystems_builtin();
	if (!fs_mods)
		fs_mods = get_kernel_filesystems_modules();

	/* XXX: doesn't handle sub directories assumes /mnt */	
	mkdir(MOUNTPOINT, 0755); 
	/* chdir so we can use relative paths for mount(2) */ 
	if (chdir(MOUNTPOINT)) {
		perror("chdir");
		return NULL;
	}

	EINA_LIST_FOREACH(partitions, l, dev) {

		if (eina_list_search_unsorted(dev_ignore, EINA_COMPARE_CB(strcmp), dev))
			continue;
		BootItem *sys = scan_partition(dev);
		if (sys)
			systems = eina_list_append(systems, sys);
	}

	return systems;
} 


/* Genereate kernel command line for use with kexec. It consist of the following:
 * 
 *  - /proc/cmdline of the running system
 *  - root=$PARTITON rootfstype=$FS
 *  - append-$MACHINE in the same directory as the kernel
 */

static char* get_kernel_cmdline(BootItem *i) {
	static char cmdline[2 * COMMAND_LINE_SIZE] = KEXEC_CMDLINE, *s = cmdline + sstrlen(KEXEC_CMDLINE);

	/* read the cmdline of the currently running system only once */
	if (!cmdline[sstrlen(KEXEC_CMDLINE) + 1]) {
		FILE *f = fopen("/proc/cmdline","r");
		if (f && fgets(s, sizeof(cmdline) - sstrlen(KEXEC_CMDLINE), f)) {
			/* /proc/cmdline ends with a new line which we want to overwrite thus -1 */
			s += strlen(s) - 1;
		}
		fclose(f);
	}

	/* append root partition, filesystem and the image specific command line */
	int len = snprintf(s, sizeof(cmdline) - (s - cmdline), "rootwait root=%s rootfstype=%s %s",
	                   i->dev, i->fs, i->cmdline ? i->cmdline : "");
	if (len > 0 && s[len - 1] == '\n')
		s[len - 1] = '\0';

	return cmdline;
}

static bool boot_kernel(BootItem *i) {
	Eina_List *l;
	BootItem *s;
	
	/* umount all filesystem we probed except the one where the kernel is located */
	EINA_LIST_FOREACH(systems, l, s) {
		if (s != i) {
			/* XXX: assumes we are still chdir()-ed */
			umount(s->dev + sstrlen("/dev/"));
		}
	}

	const char *kexec_load[] = { KEXEC, get_kernel_cmdline(i), "-l", i->kernel, NULL };
	if (fexecw(kexec_load[0], (char *const *)kexec_load, NULL)) {
		gui_show_error("Couldn't load kernel from '%s'\n", i->kernel);
		return false;
	}

	const char *kexec_exec[] = { KEXEC , "-e", NULL };

	/* XXX: assumes we are still chdir()-ed */
	umount(i->dev + sstrlen("/dev/"));

	if (execve(kexec_exec[0], (char *const *)kexec_exec, NULL)) {
		gui_show_error("Couldn't exec kernel '%s'\n", i->kernel);
		return false;
	}

	/* can't be reached just here to silence compiler warning */
	return true;
}

static void diagnostics(Eina_List *dev_ignore) {
	Eina_List *l;
	BootItem *s;
	char *p;

	puts("Partitions:");
	partitions = get_partitions();

	EINA_LIST_FOREACH(partitions, l, p) {
		puts(p);
	}

	puts("Built in filesystems:");
	fs_core = get_kernel_filesystems_builtin();

	EINA_LIST_FOREACH(fs_core, l, p) {
		puts(p);
	}

#if CONFIG_SUPPORT_KERNEL_FS_MODULES
	puts("Filesystem modules:");
	fs_mods = get_kernel_filesystems_modules();
 
	EINA_LIST_FOREACH(fs_mods, l, p) {
		puts(p);
	}
#endif

	puts("Bootable images:");
	systems = scan_system(dev_ignore);

	EINA_LIST_FOREACH(systems, l, s) {
		printf("kexec %s'%s' -l %s\n", KEXEC_CMDLINE,
		       get_kernel_cmdline(s) + sstrlen(KEXEC_CMDLINE), s->kernel);
		printf("umount '%s'\n", s->dev);
		printf("kexec -e\n\n");
	}
}
