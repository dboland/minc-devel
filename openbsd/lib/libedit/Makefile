ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= histedit.h
INCSRC= ${SRCDIR}/lib/libedit

all: ${INCSRC} ${TMPDIR} .patch

%.h: ${INCSRC}/%.h
	@/bin/cp -vu $< $@

.patch:	${HEADERS}
	@${MAKE} -C readline source
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../libedit.patch

${XINCDIR}/readline:
	@/bin/cp -vu ${HEADERS} ${XINCDIR}/

install-local:
	@/bin/cp -uv ${HEADERS} ${XINCDIR}/
	@${MAKE} -C readline install-local