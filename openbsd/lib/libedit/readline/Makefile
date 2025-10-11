ROOTDIR= ../../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= history.h readline.h
INCSRC= ${SRCDIR}/lib/libedit/readline

all: ${INCSRC}

%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

source: ${HEADERS}

${XINCDIR}/readline:
	@/bin/mkdir -p ${XINCDIR}/readline
	@/bin/cp -vu ${HEADERS} ${XINCDIR}/readline/

install-local: ${XINCDIR}/readline