ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

OBJECTS= libdl.o version.o
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

${LIBDL}: ${OBJECTS}
	${LD} ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBDL} ${LIBDIR}/dllcrt2.o ${OBJECTS} \
	-L${LIBDIR} -lmingw32

${LIBXDIR}: ${LIBDL}
	@/bin/mkdir -p ${LIBXDIR}
	/bin/cp -vu ${LIBDL} ${LIBXDIR}/

install-cross:
	@/bin/mkdir -p ${XLIBXDIR}
	/bin/cp -vu ${LIBDL} ${XLIBXDIR}/
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libdl.a

# create /usr/libexec/ld.so for configure scripts to enable shared libs

install-local:
	@/bin/mkdir -p ${ULIBXDIR}
	/bin/cp -vu ${LIBDL} ${ULIBXDIR}/
	/bin/cp -vu ${IMPLIB} ${LIBDIR}/

install: ${DESTDIR}
	@${MKDIR} -p ${DESTDIR}${ULIBXDIR}
	${INSTALL} -m ${BINMODE} ${LIBDL} ${DESTDIR}${ULIBXDIR}/
	${INSTALL} -m ${BINMODE} ${IMPLIB} ${DESTDIR}${ULIBDIR}/libdl.a

release-clean:
	/bin/rm -f version.o

release: release-clean all

clean:
	/bin/rm -f *.a ${OBJECTS} ${LIBDL}

