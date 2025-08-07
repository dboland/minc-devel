ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

OBJECTS= libdl.o
IMPLIB= libdl.dll.a
LDFLAGS+= -shared -s
LIBDL= ld.so
ULIBXDIR= /usr/libexec

all: ${LIBDL} libdl.a

%.o: %.c
	${XGCC} -c -g ${CFLAGS} -I${ROOTDIR}/include -I${SRCDIR}/sys $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

libdl.a: ${OBJECTS}
	@/bin/echo "Making static library $@"
	@ar cru $@ $^

${LIBDL}: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBD} ${XLIBDIR}/dllcrt2.o ${OBJECTS} version.o \
	-L${XLIBDIR} -lmingw32

${LIBXDIR}:
	/bin/mkdir -p ${LIBXDIR}

install-local: ${LIBXDIR}
	/bin/cp -vu ld.so ${LIBXDIR}/
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/

install-cross:
	/bin/cp -vu libdl.a ${_LIBDIR}/
	/bin/cp -vu ${IMPLIB} ${_LIBDIR}/libdl.so

install: ${DESTDIR}
	@${MKDIR} -p ${DESTDIR}${ULIBXDIR}
	${INSTALL} ld.so ${DESTDIR}${ULIBXDIR}/
	${INSTALL} ${IMPLIB} ${DESTDIR}/usr/lib/libdl.a

release-clean:
	/bin/rm -f version.o

clean:
	/bin/rm -f *.o *.a ${LIBDL}

