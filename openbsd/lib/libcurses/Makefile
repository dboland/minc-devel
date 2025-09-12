ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/ncurses.h ${XINCDIR}/unctrl.h \
	${XINCDIR}/term.h ${XINCDIR}/termcap.h
INCSRC= ${SRCDIR}/lib/libcurses

all: ${INCSRC} ${TMPDIR} ${INCSRC}/.patch

${XINCDIR}/%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

${XINCDIR}/ncurses.h: ${INCSRC}/curses.h
	@/bin/cp -vu $< $@

${INCSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${INCSRC} <../libcurses.patch

install-local: ${HEADERS}
