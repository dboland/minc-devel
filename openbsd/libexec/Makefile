ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= 
LIBXSRC= ${SRCDIR}/libexec
LIBSOSRC= ${LIBXSRC}/ld.so
LIBXDIR= ${DESTDIR}/libexec

all: ${LIBXSRC} ${PROGS}
	@${MAKE} -C ld.so all install-local

${LIBXSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'libexec' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

ldconfig: ${LIBSOSRC}/ldconfig/ldconfig.c ${LIBSOSRC}/ldconfig/shlib.c \
	${LIBSOSRC}/ldconfig/etc.c ${LIBSOSRC}/ldconfig/prebind_delete.c \
	${LIBSOSRC}/ldconfig/debug.c ${LIBSOSRC}/ldconfig/prebind.c \
	${LIBSOSRC}/ldconfig/library.c ${LIBSOSRC}/ldconfig/sod.c \
	${LIBSOSRC}/ldconfig/prebind_path.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -I${LIBSOSRC} -I${SRCDIR}/lib/libssl/src/include -o $@ $^

install-local: ${LIBXSRC}

install:
	@${MAKE} -C ld.so install

release:
	@${MAKE} -C ld.so release

clean:
	/bin/rm -f *.exe
	@${MAKE} -C ld.so clean
