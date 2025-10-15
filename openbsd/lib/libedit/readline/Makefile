ROOTDIR= ../../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= history.h readline.h
INCSRC= ${SRCDIR}/lib/libedit/readline

all: ${INCSRC}

%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

source: ${HEADERS}

${INCDIR}/readline:
	@/bin/mkdir -p ${INCDIR}/readline
	@/bin/cp -vu ${HEADERS} ${INCDIR}/readline/

install-local: ${INCDIR}/readline