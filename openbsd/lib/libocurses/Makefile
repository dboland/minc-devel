ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/ocurses.h
INCSRC= ${SRCDIR}/lib/libocurses

all: ${INCSRC} ${TMPDIR} ${INCSRC}/.patch

${XINCDIR}/ocurses.h: ${INCSRC}/curses.h
	@/bin/cp -vu $< $@

${INCSRC}/.patch:
	@TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${INCSRC} <../libocurses.patch

install-local: ${HEADERS}
