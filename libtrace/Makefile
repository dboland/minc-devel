ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

LIB= libtrace.a

all: ${LIB}

${ROOTDIR}/config.inc:
	@echo " ---------------------------------------------------"
	@echo "| Cross-compiler not set up. Please run 'make opt' first."
	@echo " ---------------------------------------------------"
	@exit 1

%.a: %
	${MAKE} -C $<

${LIB}: win.a vfs.a
	ar cru $@ $(shell /bin/ls -f win/*.o vfs/*.o)

clean:
	/bin/rm -f win/*.o vfs/*.o *.a ${LIB}
