ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

LIBS= libadvapi32.a libkernel32.a libshell32.a libuser32.a \
	libmoldname.a libgmon.a libmsvcrt.a
ULIBDIR= /usr/lib
GCCBIN= ${XBINDIR}/gcc.exe ${XBINDIR}/cpp.exe ${XBINDIR}/g++.exe ${XBINDIR}/c++.exe

all: ${LIBS} libiphlpapi.dll.a
	@${MAKE} -C gcc

include ${ROOTDIR}/Errorfile.inc

# below -k option makes ld remove "@" suffix (--enable-stdcall-fixup)

%.a: %.def
	dlltool -d $< -l $@ -k

${BINDIR}/%.dll: ${MINGW}/bin/%.dll
	@/bin/cp -vu $^ $@

${BINDIR}/%.exe: ${MINGW}/bin/%.exe
	@/bin/cp -vu $^ $@

${BINDIR}:
	@/bin/mkdir -p $@

${XBINDIR}/%.exe: ${MINGW}/bin/%.exe
	@/bin/cp -vu $^ $@

${XBINDIR}:
	@/bin/mkdir -p $@

${XLIBXDIR}:
	@/bin/mkdir -p $@
	/bin/cp -ru ${MINGW}/libexec/gcc $@/

# GetIpStatisticsEx() is missing in vanilla MinGW (libiphlpapi.dll.a)

install-local: ${MINGW} ${XBINDIR} ${GCCBIN} ${XLIBXDIR}
	@/bin/mkdir -p ${XLIBDIR}
	/bin/cp -vu ${LIBS} ${XLIBDIR}/
	/bin/cp -vu libiphlpapi.dll.a ${MINGW}/lib/
	@${MAKE} -C gcc install-local

install-cross:
	/bin/cp -vu libmsvcrt.a ${_LIBDIR}/
	@${MAKE} -C gcc install-cross

install: ${DESTDIR}
	@${MKDIR} -p ${DESTDIR}${ULIBDIR}
	${INSTALL} libmsvcrt.a ${DESTDIR}${ULIBDIR}/
	@${MAKE} -C gcc install

clean:
	/bin/rm -f *.a
	@${MAKE} -C gcc clean
