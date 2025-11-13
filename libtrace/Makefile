ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

LIB= libtrace.a

all: win.d vfs.d libtrace.a

%.d: %
	@${MAKE} -C $<

${ROOTDIR}/config.inc:
	@echo " ---------------------------------------------------"
	@echo "| Cross-compiler not set up. Please run 'make opt' first."
	@echo " ---------------------------------------------------"
	@exit 1

${LIB}:
	ar cru $@ $(shell /bin/ls -f win/*.o vfs/*.o)

clean:
	/bin/rm -f win/*.o vfs/*.o ${LIB}
