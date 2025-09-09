ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/histedit.h
INCSRC= ${SRCDIR}/lib/libedit/readline

all: ${INCSRC} ${TMPDIR}

${XINCDIR}/%.h: ${SRCDIR}/lib/libedit/%.h
	@/bin/cp -vu $< $@

${XINCDIR}/readline:
	@/bin/mkdir -p $@
	@/bin/cp -rvu ${INCSRC}/*.h $@/
	@TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${XINCDIR} <../../libedit.patch

install-local: ${HEADERS} ${XINCDIR}/readline
