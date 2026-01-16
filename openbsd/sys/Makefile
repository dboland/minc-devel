ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SYSDIRS= ${INCDIR}/machine ${INCDIR}/sys ${INCDIR}/dev ${INCDIR}/net \
	${INCDIR}/net80211 ${INCDIR}/netinet ${INCDIR}/netinet6 \
	${INCDIR}/crypto ${INCDIR}/uvm ${INCDIR}/dev/wscons \
	${INCDIR}/ufs/ext2fs ${INCDIR}/ufs/ffs ${INCDIR}/ufs/mfs \
	${INCDIR}/ufs/ufs
SYSSRC= ${SRCDIR}/sys

all: ${SYSSRC} ${TMPDIR} ${SYSSRC}/.patch
	${MAKE} -C stand/boot

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

${INCDIR}/machine: ${SRCDIR}/sys/arch/${ARCH}/include
	@/bin/mkdir -p $@
	/bin/cp -ru  $</* $@/

install-local: ${INCDIR} ${SYSDIRS}

install:
	${MAKE} -C stand/boot install

clean:
	${MAKE} -C stand/boot clean
