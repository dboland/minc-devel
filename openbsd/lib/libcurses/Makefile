ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

XINCLUDES= ${XINCDIR}/ncurses.h ${XINCDIR}/unctrl.h ${XINCDIR}/curses.h
XLIBS= ${XLIBDIR}/libncurses.a ${XLIBDIR}/libncursesw.a
_LIBS= ${_LIBDIR}/libncurses.a ${_LIBDIR}/libncursesw.a

ULIBDIR= ${DESTDIR}/usr/lib
UINCDIR= ${DESTDIR}/usr/include

${XINCDIR}/%.h: ${UINCDIR}/%.h
	@/bin/cp -vu $< $@

${XLIBDIR}/libncurses.a:
	/bin/cp -vu ${ULIBDIR}/libncurses.a ${XLIBDIR}/

${XLIBDIR}/libncursesw.a:
	/bin/ln ${XLIBDIR}/libncurses.a $@

${_LIBDIR}/%.a: ${XLIBDIR}/%.a
	/bin/ln -f $< $@

install-local: ${XINCLUDES} ${XLIBS} ${_LIBDIR} ${_LIBS}
