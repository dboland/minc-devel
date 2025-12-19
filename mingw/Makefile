ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

LIBS= libadvapi32.a libkernel32.a libshell32.a libuser32.a \
	libmoldname.a libgmon.a libmsvcrt.a libgdi32.a libcomdlg32.a
ULIBDIR= /usr/lib
GCCBIN= ${BINDIR}/gcc.exe ${BINDIR}/cpp.exe ${BINDIR}/g++.exe ${BINDIR}/c++.exe

all: ${LIBS} libiphlpapi.dll.a
	@${MAKE} -C libgcc

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

${LIBXDIR}:
	@/bin/mkdir -p $@
	/bin/cp -ru ${MINGW}/libexec/gcc $@/

# GetIpStatisticsEx() is missing in vanilla MinGW (libiphlpapi.dll.a)

install-cross:
	@/bin/mkdir -p ${XLIBDIR}
	/bin/cp -vu libmsvcrt.a ${XLIBDIR}/

install-local: ${MINGW} ${BINDIR} ${GCCBIN} ${LIBXDIR} install-cross
	@/bin/mkdir -p ${LIBDIR}
	/bin/cp -vu ${LIBS} ${LIBDIR}/
	/bin/cp -vu libiphlpapi.dll.a ${MINGW}/lib/
	@${MAKE} -C libgcc install-local

install: ${DESTDIR}
	@${MKDIR} -p ${DESTDIR}${ULIBDIR}
	${INSTALL} libmsvcrt.a ${DESTDIR}${ULIBDIR}/
	@${MAKE} -C libgcc install

clean:
	/bin/rm -f *.a
	@${MAKE} -C libgcc clean
