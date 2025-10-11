ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

RELEASE= 75.0
OBJECTS= libc.minc.o version.o
LIBS= stdio.a stdlib.a quad.a locale.a citrus.a compat-43.a crypt.a gdtoa.a gen.a \
	hash.a regex.a string.a termios.a time.a thread.a \
	sys.a nls.a dlfcn.a gmon.a net.a
LIBS+= arch/string.a arch/gen.a arch/net.a arch/sys.a
LIBS+= db/btree.a db/db.a db/hash.a db/mpool.a db/recno.a
SHOBJECTS= $(shell /bin/ls arch/*/*.o */*.o db/*/*.o)
LIBC= libc.so.${RELEASE}
GCCDIR= ${XLIBDIR}/gcc/mingw32/${GCCVER}
CFLAGS+= -g -DNOPIE
IMPLIB= libc.dll.a
ULIBDIR= /usr/lib
INCSRC= ${SRCDIR}/lib/libc/include
LDFLAGS+= --export-all

NLS=    C.msg Pig.msg da.ISO8859-1.msg da.UTF-8.msg de.ISO8859-1.msg \
        de.UTF-8.msg es.ISO8859-1.msg es.UTF-8.msg fi.ISO8859-1.msg \
        fi.UTF-8.msg fr.ISO8859-1.msg fr.UTF-8.msg it.UTF-8.msg \
        nl.ISO8859-1.msg nl.UTF-8.msg no.ISO8859-1.msg no.UTF-8.msg \
        ru.KOI8-R.msg ru.UTF-8.msg sv.ISO8859-1.msg sv.UTF-8.msg

all: ${TMPDIR} .patch ${LIBS} ${LIBC} libc.a

%.a: %
	@CFLAGS="${CFLAGS}" ${MAKE} -C $< XGCC="${XGCC}" ROOTDIR="${ROOTDIR}/.." \
	SRCDIR=${SRCDIR} ARCH=${ARCH}

%.o: %.c
	${XGCC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<

.patch:
	@${MAKE} -C asr SRCDIR=${SRCDIR} source
	@${MAKE} -C citrus SRCDIR=${SRCDIR} source
	@${MAKE} -C gen SRCDIR=${SRCDIR} source
	@${MAKE} -C gmon SRCDIR=${SRCDIR} source
	@${MAKE} -C include SRCDIR=${SRCDIR} source
	@${MAKE} -C net SRCDIR=${SRCDIR} source
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../libc.patch

libc.a:
	@/bin/echo "Making static library $@"
	@ar cru $@ ${OBJECTS} ${SHOBJECTS}

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${LIBC}: ${OBJECTS}
	${LD} -s -shared ${LDFLAGS} -e _DllMainCRTStartup@12 --out-implib=${IMPLIB} \
	-o $@ --image-base=${IMGBASE_LIBC} ${XLIBDIR}/dllcrt2.o ${SHOBJECTS} ${OBJECTS} version.o \
	-L${XLIBDIR} -L${GCCDIR} -lmingw32 -lmsvcrt -lgcc

lib-clean:
	/bin/rm -f ${OBJECTS} ${LIBC} ${IMPLIB} libc.a

all-clean: lib-clean
	@${MAKE} -C hash clean
	/bin/rm -f ${LIBS} */*.o arch/*/*.o db/*/*.o

release-clean:
	/bin/rm -f version.o

patch-clean:
	@${MAKE} -C asr patch-clean
	@${MAKE} -C citrus patch-clean
	@${MAKE} -C gen patch-clean
	@${MAKE} -C gmon patch-clean
	@${MAKE} -C include patch-clean
	@${MAKE} -C net patch-clean
	/bin/rm -f .patch

release: release-clean all

install-cross: ${_LIBDIR}
	@/bin/mkdir -p ${_LIBDIR}
	@/bin/cp -vu libc.a ${_LIBDIR}/
	@/bin/cp -vu ${IMPLIB} ${_LIBDIR}/libc.so

# Perl needs libc.so

install-local: install-cross
	@/bin/cp -vu ${LIBC} ${XBINDIR}/
	@/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libc.so
	@/bin/cp -vu ${IMPLIB} ${XLIBDIR}/libmingwex.dll.a
	@/bin/cp -vu ${IMPLIB} ${XLIBDIR}/
	@/bin/cp -vu libc.a ${XLIBDIR}/libmingwex.a
	@/bin/cp -vu libc.a ${XLIBDIR}/

install: ${DESTDIR}
	@${MKDIR} -p ${DESTDIR}${ULIBDIR}
	${INSTALL} -m ${BINMODE} ${LIBC} libc.a ${DESTDIR}${ULIBDIR}/

clean: lib-clean
	@${MAKE} -C hash clean
