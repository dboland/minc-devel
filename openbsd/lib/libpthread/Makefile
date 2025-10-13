ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${INCDIR}/pthread.h ${INCDIR}/sched.h
INCSRC= ${SRCDIR}/lib/libpthread/include

all: ${INCSRC}

${INCDIR}/%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

install-local: ${HEADERS}
