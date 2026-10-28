ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= unctrl.h term.h termcap.h
SOURCES= ${HEADERS} curses.h
LIBSRC= ${SRCDIR}/lib/libcurses
IMPLIB= ${LIBSRC}/libcurses.dll.a

all: ${LIBSRC} ${TMPDIR} ${SOURCES} .patch

%.h: ${LIBSRC}/%.h
	@/bin/cp -vu $< $@

.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../libcurses.patch

${INCDIR}/ncurses.h: curses.h
	@/bin/cp -vu $< $@

install-local: ${INCDIR}/ncurses.h
	/bin/cp -uv ${HEADERS} ${INCDIR}/

install-cross: ${IMPLIB}
	/bin/cp -uv ${IMPLIB} ${XLIBDIR}/libncurses.a

patch-clean:
	/bin/rm -rf ${SOURCES} .patch
