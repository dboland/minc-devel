ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/pthread.h ${XINCDIR}/sched.h
INCSRC= ${SRCDIR}/lib/libpthread/include

all: ${INCSRC}

${XINCDIR}/%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

install-local: ${HEADERS}
