ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= unctrl.h term.h termcap.h
LIBSRC= ${SRCDIR}/lib/libcurses
IMPLIB= ${LIBSRC}/libcurses.dll.a

all: ${LIBSRC} ${TMPDIR} .patch

%.h: ${LIBSRC}/%.h
	@/bin/cp -vu $< $@

.patch: ${HEADERS} curses.h
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../libcurses.patch

${INCDIR}/ncurses.h: curses.h
	@/bin/cp -vu $< $@

install-local: ${INCDIR}/ncurses.h
	/bin/cp -uv ${HEADERS} ${INCDIR}/

install-cross: ${IMPLIB}
	/bin/cp -uv ${IMPLIB} ${XLIBDIR}/libncurses.a
