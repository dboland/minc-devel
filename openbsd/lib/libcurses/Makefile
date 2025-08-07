ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

XINCLUDES= ${XINCDIR}/ncurses.h ${XINCDIR}/unctrl.h ${XINCDIR}/curses.h
XLIBS= ${XLIBDIR}/libncurses.a ${XLIBDIR}/libncursesw.a

ULIBDIR= ${DESTDIR}/usr/lib
UINCDIR= ${DESTDIR}/usr/include

${XINCDIR}/%.h: ${UINCDIR}/%.h
	@/bin/cp -vu $< $@

${XLIBDIR}/libncurses.a:
	/bin/cp -vu ${ULIBDIR}/libncurses.a ${XLIBDIR}/

${XLIBDIR}/libncursesw.a:
	/bin/ln ${XLIBDIR}/libncurses.a $@

install-local: ${XINCLUDES} ${XLIBS}
