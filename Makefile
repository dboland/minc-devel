.SUFFIXES:
.DEFAULT:

usage:
	@echo "usage: ${MAKE} TARGET"
	@echo
	@echo "Targets:"
	@printf " opt\t\t\tset up a simple cross-compiler\n"
	@printf " all\t\t\tbuild all targets\n"
	@printf " kernel\t\t\tbuild libtrace and libposix\n"
	@printf " system\t\t\tbuild OpenBSD system binaries\n"
	@printf " release\t\tupdate version info in shared libraries\n"
	@printf " help\t\t\tshow build instructions\n"

all: kernel system

opt: config.inc
	@${MAKE} -C openbsd install-local
	@${MAKE} -C mingw all install-local

kernel:
	@${MAKE} -C libtrace
	@${MAKE} -C libposix all install-local

system:
	@${MAKE} -C openbsd all

config.inc:
	@/bin/cp $@.sample $@

help:
	@less BUILD.md

release:
	@${MAKE} -C libposix release
	@${MAKE} -C openbsd release

install-cross:
	@${MAKE} -C mingw install-cross
	@${MAKE} -C libposix install-cross
	@${MAKE} -C openbsd install-cross

install:
	@${MAKE} -C openbsd install
	@${MAKE} -C libposix install
	@${MAKE} -C mingw install

clean:
	@${MAKE} -C libposix clean
	@${MAKE} -C openbsd clean
