ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

all:
	@${MAKE} -C libpthread all install-local
	@${MAKE} -C libc all install-local
	@${MAKE} -C libsocket all install-local
	@${MAKE} -C libm all install-local
	@${MAKE} -C libz all install-local
	@${MAKE} -C libutil all install-local
	@${MAKE} -C libedit all install-local
	@${MAKE} -C libcurses all install-local
	@${MAKE} -C libocurses all install-local
	@${MAKE} -C libssl all install-local

install:
	@${MAKE} -C libc install
	@${MAKE} -C libsocket install

release:
	@${MAKE} -C libc release
	@${MAKE} -C libsocket release

clean:
	@${MAKE} -C libc clean
	@${MAKE} -C libsocket clean
	@${MAKE} -C libutil clean
	@${MAKE} -C libm clean
	@${MAKE} -C libz clean

