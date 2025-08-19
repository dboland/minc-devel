usage:
	@echo "usage: ${MAKE} TARGET"
	@echo
	@echo "Targets:"
	@echo " opt			set up a simple cross-compiler"
	@echo " all			build all targets"
	@echo " kernel			build libtrace and libposix"
	@echo " system			build OpenBSD system binaries"
	@echo " release		update version info in shared libraries"
	@echo " help			show build instructions"

all: kernel system

%.d: %
	@${MAKE} -C $<

opt: config.inc
	@${MAKE} -C openbsd install-local
	@${MAKE} -C mingw all install-local

kernel:
	@${MAKE} -C libtrace all
	@${MAKE} -C libposix all install-local

system:
	@${MAKE} -C openbsd

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

clean:
	@${MAKE} -C libposix clean
	@${MAKE} -C openbsd clean
