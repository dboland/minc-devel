ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SHSRC= ${SRCDIR}/share
SHDEST= ${DESTDIR}/usr/share

all: ${SHSRC} ${TMPDIR} ${SHSRC}/.patch
	@${MAKE} -C mk

${SHSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'share' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${SHSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${SHSRC} <../share.patch

install:
	@${MAKE} -C mk install

clean:
	/bin/rm -rf ${SHDIR}/*
