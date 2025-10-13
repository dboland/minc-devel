ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SYSDIRS= ${XINCDIR}/net ${XINCDIR}/machine ${XINCDIR}/netinet ${XINCDIR}/netinet6 \
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

${SYSSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${SYSSRC} <../sys.patch

${XINCDIR}/%: ${SRCDIR}/sys/%
	@/bin/mkdir -p $@
	/bin/cp -u $</*.h $@/

${XINCDIR}/machine:
	@/bin/mkdir -p $@
	/bin/cp -ru  ${SRCDIR}/sys/arch/${ARCH}/include/* $@/

${_INCDIR}: ${XINCDIR}
	@/bin/mkdir -p ${_INCDIR}
	/bin/cp -ru ${XINCDIR}/* ${_INCDIR}/

install-local: ${XINCDIR} ${SYSDIRS}
