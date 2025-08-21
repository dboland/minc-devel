ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

OBJECTS= libdl.o
IMPLIB= libdl.dll.a
LDFLAGS+= -shared -s
LIBDL= ld.so
ULIBDIR= /usr/lib
ULIBXDIR= /usr/libexec

all: lib

%.o: %.c
	${XGCC} -c -g ${CFLAGS} -I${ROOTDIR}/include $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

libdl.a: ${OBJECTS}
	@/bin/echo "Making static library $@"
	@ar cru $@ $^

${LIBDL}: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBDL} ${XLIBDIR}/dllcrt2.o ${OBJECTS} version.o \
	-L${XLIBDIR} -lmingw32

# create /usr/libexec/ld.so for configure scripts to detect shared libs

${LIBXDIR}: ${LIBDL}
	@/bin/mkdir -p ${LIBXDIR}
	/bin/cp -vu ${LIBDL} ${LIBXDIR}/

lib: ${LIBDL} libdl.a

install-local: ${LIBXDIR}
	/bin/cp -vu libdl.a ${XLIBDIR}/
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/

install-cross:
	/bin/cp -vu libdl.a ${_LIBDIR}/
	/bin/cp -vu ${IMPLIB} ${_LIBDIR}/libdl.so

install: ${DESTDIR}
	@${MKDIR} -p ${DESTDIR}${ULIBXDIR}
	${INSTALL} ${LIBDL} ${DESTDIR}${ULIBXDIR}/
	${INSTALL} libdl.a ${DESTDIR}${ULIBDIR}/

release-clean:
	/bin/rm -f version.o

clean:
	/bin/rm -f *.o *.a ${LIBDL}

