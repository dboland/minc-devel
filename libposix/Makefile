ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

OBJECTS= crt1.o version.o msvc.o win_posix.o vfs_posix.o libposix.o \
	arch/fork.o arch/syscall.o
LIBPOSIX= libposix-${RELEASE}.dll
IMPLIB= libposix.dll.a
CFLAGS+= -I${ROOTDIR}/include -g
CFLAGS+= -DRELEASE=\"${RELEASE}\" -DVERSION=\"${VERSION}\" -DBUILD=${BUILD}
GCCDIR= ${ROOTDIR}/mingw/gcc
TRACEDIR= ${ROOTDIR}/libtrace
SBINDIR= /sbin
ULIBDIR= /usr/lib

all: ${LIBPOSIX}

include ${ROOTDIR}/Errorfile.inc

crt1.o: crt1.c
	${CC} -c ${CFLAGS} $<

msvc.o: msvc.c
	${CC} -c ${CFLAGS} -o $@ $<

win_posix.o: win/win.c
	${CC} -c ${CFLAGS} -o $@ $<

vfs_posix.o: vfs/vfs.c
	${CC} -c ${CFLAGS} -I${TRACEDIR} -o $@ $<

libposix.o: libposix.c
	${XGCC} -c ${CFLAGS} -I${SRCDIR}/sys -DTTYDEFCHARS -DKTRACE -DSYSVSEM $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

arch/%.o: arch/${PROCESSOR_ARCHITECTURE}/%.S
	${XGCC} -c ${CFLAGS} -o $@ $<

${LIBPOSIX}: ${OBJECTS}
	${LD} -shared -s ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBPOSIX} ${OBJECTS} \
	-L${MINGW}/lib -L${GCCDIR} -L${TRACEDIR} -lmsvcrt -ladvapi32 -lshell32 -luser32 -lntdll -lkernel32 \
	-lnetapi32 -lws2_32 -liphlpapi -lrpcrt4 -lmpr -lgcc -ltrace

${XLIBDIR}:
	/bin/mkdir -p ${XLIBDIR}

# make sure we always have access to the kernel

install-local:
	/bin/cp -vu ${LIBPOSIX} /usr/bin/
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libmingw32.a

install-cross:
	/bin/cp -vu ${IMPLIB} ${_LIBDIR}/libposix.so

install: ${INSTALL}
	${INSTALL} -m 06555 ${LIBPOSIX} ${DESTDIR}${SBINDIR}/
	${INSTALL} ${IMPLIB} ${DESTDIR}${ULIBDIR}/libposix.so

release-clean:
	/bin/rm version.o

release: release-clean all

clean:
	/bin/rm -f arch/*.o *.o *.a *.dll

