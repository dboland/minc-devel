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

install-local: ${LIBXSRC}

install:
	@${MAKE} -C ld.so install

release:
	@${MAKE} -C ld.so release

clean:
	/bin/rm -f *.exe
	@${MAKE} -C ld.so clean
