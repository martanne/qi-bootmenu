include config.mk

SRC += qi-bootmenu.c fstype/fstype.c
OBJ = ${SRC:.c=.o}

all: clean options qi-bootmenu

options:
	@echo qi-bootmenu build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS} ${LIBS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} ${CFLAGS} -c $< -o $@

${OBJ}: config.h config.mk

qi-bootmenu: ${OBJ}
	@echo CC -o $@
	@${CC} ${LDFLAGS} ${LIBS} ${OBJ} -o $@

debug:
	@make CFLAGS='${DEBUG_CFLAGS}'

static:
	LDFLAGS='${LDFLAGS_STATIC} ${LDFLAGS}' PKG_CONFIG_FLAGS='${PKG_CONFIG_STATIC_FLAGS}' make

static_debug:
	@make static CFLAGS='${DEBUG_CFLAGS}'

clean:
	@echo cleaning
	@rm -f qi-bootmenu ${OBJ} qi-bootmenu-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p qi-bootmenu-${VERSION}
	@cp -r Makefile config.h config.mk qi-bootmenu.c gui.c \
		gui-*.c kexec.c fstype data \
		qi-bootmenu-${VERSION}
	@tar -cf qi-bootmenu-${VERSION}.tar qi-bootmenu-${VERSION}
	@gzip qi-bootmenu-${VERSION}.tar
	@rm -rf qi-bootmenu-${VERSION}

install: qi-bootmenu
	@echo installing executable file to ${DESTDIR}${PREFIX}/sbin
	@mkdir -p ${DESTDIR}${PREFIX}/sbin
	@cp -f qi-bootmenu ${DESTDIR}${PREFIX}/sbin
	@chmod 755 ${DESTDIR}${PREFIX}/sbin/qi-bootmenu
	@mkdir -p ${DESTDIR}${DATADIR}
	@cp -r data/* ${DESTDIR}${DATADIR}

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/sbin
	@rm -f ${DESTDIR}${PREFIX}/sbin/qi-bootmenu

.PHONY: all options clean dist install uninstall debug static
