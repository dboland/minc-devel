ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= unctrl.h term.h termcap.h
INCSRC= ${SRCDIR}/lib/libcurses

all: ${INCSRC} ${TMPDIR} .patch

%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

.patch: ${HEADERS} curses.h
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../libcurses.patch

${INCDIR}/ncurses.h: curses.h
	@/bin/cp -vu $< $@

install-local: ${INCDIR}/ncurses.h
	/bin/cp -uv ${HEADERS} ${INCDIR}/
