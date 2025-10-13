ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

OBJECTS= libdl.o
IMPLIB= libdl.dll.a
LDFLAGS+= -shared -s
LIBDL= ld.so
ULIBDIR= /usr/lib
ULIBXDIR= /usr/libexec

all: ${LIBDL}

%.o: %.c
	${XGCC} -c -g ${CFLAGS} -I${ROOTDIR}/include $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBDL}: ${OBJECTS} version.o
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBDL} ${XLIBDIR}/dllcrt2.o ${OBJECTS} version.o \
	-L${XLIBDIR} -lmingw32

# create /usr/libexec/ld.so for configure scripts to detect shared libs

${LIBXDIR}: ${LIBDL}
	@/bin/mkdir -p ${LIBXDIR}
	/bin/cp -vu ${LIBDL} ${LIBXDIR}/

install-cross: ${_LIBDIR}
	/bin/cp -vu ${IMPLIB} ${_LIBDIR}/libdl.a

install-local: ${LIBXDIR}
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/

install: ${DESTDIR}
	@${MKDIR} -p ${DESTDIR}${ULIBXDIR}
	${INSTALL} -m ${BINMODE} ${LIBDL} ${DESTDIR}${ULIBXDIR}/
	${INSTALL} -m ${BINMODE} ${IMPLIB} ${DESTDIR}${ULIBDIR}/libdl.a

release-clean:
	/bin/rm -f version.o

release: release-clean all

clean:
	/bin/rm -f *.o *.a ${LIBDL}

