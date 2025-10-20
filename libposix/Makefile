ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

OBJECTS= crt1.o version.o msvc.o win_posix.o vfs_posix.o libposix.o \
	arch/fork.o arch/syscall.o
LIBPOSIX= libposix-${RELEASE}.dll
IMPLIB= libposix.dll.a
CFLAGS+= -I${ROOTDIR}/include -g
CFLAGS+= -DRELEASE=\"${RELEASE}\" -DVERSION=\"${VERSION}\" -DBUILD=${BUILD}
GCCDIR= ${ROOTDIR}/mingw/libgcc
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
	${XGCC} -c ${CFLAGS} -DTTYDEFCHARS -DKTRACE -DSYSVSEM $<

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

arch/%.o: arch/${PROCESSOR_ARCHITECTURE}/%.S
	${XGCC} -c ${CFLAGS} -o $@ $<

# make sure msvcrt.dll is always linked right after ntdll.dll
# ultimately, we want to remove the C++ runtime dependency
#
${LIBPOSIX}: ${OBJECTS}
	${LD} -shared -s ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBPOSIX} ${OBJECTS} \
	-L${MINGW}/lib -L${GCCDIR} -L${TRACEDIR} -ladvapi32 -lshell32 -luser32 \
	-lntdll -lmsvcrt -lkernel32 -lnetapi32 -lws2_32 -liphlpapi -lrpcrt4 \
	-lgcc -ltrace

${XLIBDIR}:
	/bin/mkdir -p ${XLIBDIR}

install-cross:
	@/bin/mkdir -p ${XLIBDIR}
	/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libposix.so

# make sure we always have access to the kernel

install-local:
	@/bin/cp -vu ${LIBPOSIX} /usr/bin/
	@/bin/cp -vu ${IMPLIB} ${LIBDIR}/libmingw32.a

install: ${DESTDIR}
	${INSTALL} -m 06555 ${LIBPOSIX} ${DESTDIR}${SBINDIR}/
	${INSTALL} -m ${BINMODE} ${IMPLIB} ${DESTDIR}${ULIBDIR}/libposix.a

release-clean:
	/bin/rm -f version.o libposix.o

release: release-clean all

clean:
	/bin/rm -f arch/*.o *.o *.a *.dll

