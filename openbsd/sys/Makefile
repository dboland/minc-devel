ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SYSDIRS= ${INCDIR}/machine ${INCDIR}/sys ${INCDIR}/dev ${INCDIR}/net \
	${INCDIR}/net80211 ${INCDIR}/netinet ${INCDIR}/netinet6 \
	${INCDIR}/crypto ${INCDIR}/uvm ${INCDIR}/dev/wscons \
	${INCDIR}/ufs/ext2fs ${INCDIR}/ufs/ffs ${INCDIR}/ufs/mfs \
	${INCDIR}/ufs/ufs
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

${INCDIR}/%: ${SRCDIR}/sys/%
	@/bin/mkdir -p $@
	/bin/cp -u $</*.h $@/

${INCDIR}/machine:
	@/bin/mkdir -p $@
	/bin/cp -ru  ${SRCDIR}/sys/arch/${ARCH}/include/* $@/

${XINCDIR}: ${INCDIR}
	@/bin/mkdir -p ${XINCDIR}
	/bin/cp -ru ${INCDIR}/* ${XINCDIR}/

install-local: ${INCDIR} ${SYSDIRS}
