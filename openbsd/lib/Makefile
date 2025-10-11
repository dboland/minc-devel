ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

LIBSRC= ${SRCDIR}/lib

all: ${TMPDIR} ${LIBSRC}/.patch
	@${MAKE} -C libpthread all install-local
	@${MAKE} -C libc all install-local
	@${MAKE} -C libsocket all install-local
	@${MAKE} -C libm all install-local
	@${MAKE} -C libz all install-local
	@${MAKE} -C libutil all install-local
	@${MAKE} -C libedit all install-local
	@${MAKE} -C libcurses all install-local
	@${MAKE} -C libocurses all install-local

${LIBSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${LIBSRC} <../lib.patch

install:
	@${MAKE} -C libc install
	@${MAKE} -C libsocket install
	@${MAKE} -C libm install

release:
	@${MAKE} -C libc release
	@${MAKE} -C libsocket release

clean:
	@${MAKE} -C libc clean
	@${MAKE} -C libsocket clean
	@${MAKE} -C libutil clean
	@${MAKE} -C libm clean
	@${MAKE} -C libz clean

