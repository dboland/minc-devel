ROOTDIR=../../..

include ${ROOTDIR}/Makefile.inc

RELEASE=75.0
OBJECTS= crt1.o version.o
LIBS= asr.a net.a rpc.a
LIBSOCKET= libsocket.so.${RELEASE}
IMPLIB= libsocket.dll.a
GCCDIR= ${XLIBDIR}/gcc/mingw32/${GCCVER}
ULIBDIR= /usr/lib

all: ${TMPDIR} .patch ${LIBS} ${LIBSOCKET} libsocket.a

%.a: %
	@CFLAGS="${CFLAGS} -I../../libc/include" ${MAKE} -C $< XGCC="${XGCC}" \
	ROOTDIR="${ROOTDIR}/.." SRCDIR="${SRCDIR}"

.patch:
	@${MAKE} -C asr SRCDIR="${SRCDIR}" source
	@${MAKE} -C net SRCDIR="${SRCDIR}" source
	TMPDIR=${TMPDIR} /bin/patch -p 0 -d . <../libsocket.patch

crt1.o: crt1.minc.c
	${CC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<

libsocket.a:
	@echo "Making static library $@"
	@ar cru $@ $(shell ls */*.o)

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBSOCKET}: ${OBJECTS}
	${LD} -s -shared ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ crt1.o --image-base=${IMGBASE_LIBSOCKET} $(shell /bin/ls */*.so) version.o \
	-L${XLIBDIR} -L${GCCDIR} -lmingw32 -lmingwex -lmsvcrt -lgcc

lib-clean:
	/bin/rm -f *.o ${LIBSOCKET} ${IMPLIB} libsocket.a

all-clean: lib-clean
	/bin/rm -f ${LIBS} */*.o */*.so

release-clean:
	/bin/rm -f ${OBJECTS}

release: release-clean all

# Perl needs libsocket.so

install-cross: ${_LIBDIR}
	/bin/cp -vu ${IMPLIB} ${_LIBDIR}/libsocket.so
	/bin/cp -vu libsocket.a ${_LIBDIR}/

install-local: install-cross
	/bin/cp -vu ${LIBSOCKET} ${XBINDIR}/
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libsocket.so
	/bin/cp -vu libsocket.a ${XLIBDIR}/
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/

install: ${DESTDIR}
	${INSTALL} -m ${BINMODE} ${LIBSOCKET} libsocket.a ${DESTDIR}${ULIBDIR}/

clean: lib-clean

