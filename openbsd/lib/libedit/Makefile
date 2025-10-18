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

${INCDIR}/readline:
	@/bin/cp -vu ${HEADERS} ${INCDIR}/

install-local:
	@/bin/cp -uv ${HEADERS} ${INCDIR}/
	@${MAKE} -C readline install-local
