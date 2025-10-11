ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= 
INCSRC= ${SRCDIR}/lib/libocurses

all: ${INCSRC} ${TMPDIR} .patch

%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

.patch: curses.h
	@TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../libocurses.patch

${XINCDIR}/ocurses.h: curses.h
	@/bin/cp -vu $< $@

install-local: ${XINCDIR}/ocurses.h
