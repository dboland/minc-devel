ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SYSDIRS= ${XINCDIR}/net ${XINCDIR}/netinet ${XINCDIR}/netinet6 \
	${XINCDIR}/uvm ${XINCDIR}/dev ${XINCDIR}/dev/wscons \
	${XINCDIR}/ufs/ext2fs ${XINCDIR}/ufs/ffs ${XINCDIR}/ufs/mfs \
	${XINCDIR}/ufs/ufs ${XINCDIR}/sys
SYSSRC= ${SRCDIR}/sys

all: ${SYSSRC} ${TMPDIR} ${SYSSRC}/.patch

${SYSSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'sys' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${XINCDIR}/%: ${SRCDIR}/sys/%
	@/bin/mkdir -p $@
	/bin/cp -u $</*.h $@/

${SYSSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 1 -d ${SYSSRC} <../sys.patch

install-local: ${XINCDIR} ${SYSDIRS}
