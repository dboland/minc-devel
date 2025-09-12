ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/histedit.h
INCSRC= ${SRCDIR}/lib/libedit

all: ${INCSRC} ${TMPDIR} ${INCSRC}/.patch

${XINCDIR}/%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

${XINCDIR}/readline:
	@/bin/mkdir -p $@
	/bin/cp -rvu ${INCSRC}/readline/*.h $@/

${INCSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${INCSRC} <../libedit.patch

install-local: ${HEADERS} ${XINCDIR}/readline
