ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

LIB= libz.a
OBJECTS= adler32.o compress.o crc32.o deflate.o gzio.o infback.o \
	inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
HEADERS= zlib.h zconf.h
ULIBDEST= ${DESTDIR}/usr/lib
IMPLIB= ${SRCDIR}/lib/libz/libz.dll.a

all: ${HEADERS} ${LIB}

%.o: ${SRCDIR}/lib/libz/%.c
	@/bin/echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

%.h: ${SRCDIR}/lib/libz/%.h
	@/bin/cp -vu $< ${INCDIR}/

${LIB}: ${OBJECTS}
	@/bin/echo "done."
	@/bin/echo "Making static library $@"
	@ar cru $@ ${OBJECTS}

install-local:
	/bin/cp -vu ${LIB} ${LIBDIR}/

install-cross: ${IMPLIB}
	/bin/cp -uv ${IMPLIB} ${XLIBDIR}/libz.a

install:
	${INSTALL} -m ${BINMODE} ${LIB} ${ULIBDEST}/

lib-clean:
	/bin/rm -f ${LIB}

all-clean: lib-clean
	/bin/rm -f *.o

clean: lib-clean
