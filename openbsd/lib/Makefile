ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

all:
	@${MAKE} -C libpthread install-local
	@${MAKE} -C libz all install-local
	@${MAKE} -C libc all install-local
	@${MAKE} -C libdl all install-local
	@${MAKE} -C libsocket all install-local
	@${MAKE} -C libutil all install-local

install:
	@${MAKE} -C libc install
	@${MAKE} -C libdl install
	@${MAKE} -C libsocket install

release:
	@${MAKE} -C libdl release-clean lib
	@${MAKE} -C libc release-clean lib
	@${MAKE} -C libsocket release-clean lib

clean:
	@${MAKE} -C libc clean
	@${MAKE} -C libdl clean
	@${MAKE} -C libsocket clean
	@${MAKE} -C libutil clean

