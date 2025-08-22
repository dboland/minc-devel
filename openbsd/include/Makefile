ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/fcntl.h ${XINCDIR}/stdarg.h ${XINCDIR}/stdint.h \
	${XINCDIR}/syslog.h ${XINCDIR}/termios.h
SYSDIRS= ${XINCDIR}/machine ${XINCDIR}/net ${XINCDIR}/netinet ${XINCDIR}/netinet6 \
	${XINCDIR}/uvm ${XINCDIR}/dev/wscons ${XINCDIR}/ufs/ffs
INCSRC= ${SRCDIR}/include

all: ${INCSRC}

${INCSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'include' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${TMPDIR}:
	@${MKDIR} -p ${TMPDIR}

${XINCDIR}/%: ${SRCDIR}/sys/%
	@/bin/mkdir -p $@
	/bin/cp -u $</*.h $@/

# make patch.exe use local tmp directory

${XINCDIR}/machine: ${TMPDIR}
	@/bin/mkdir -p $@
	/bin/cp -ru  ${SRCDIR}/sys/arch/${ARCH}/include/* $@/
	TMPDIR=${TMPDIR} /bin/patch -p 0 -d ${XPREFIX} <../include.patch

${XINCDIR}:
	@/bin/mkdir -p ${XINCDIR}
	/bin/cp -ru ${SRCDIR}/include/* ${XINCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/sys ${XINCDIR}/

${XINCDIR}/fcntl.h:
	@/bin/cp -vu ../fcntl.h ${XINCDIR}/

${XINCDIR}/stdarg.h:
	@/bin/cp -vu ../stdarg.h ${XINCDIR}/

${XINCDIR}/stdint.h:
	@/bin/cp -vu ../stdint.h ${XINCDIR}/

${XINCDIR}/syslog.h:
	@/bin/cp -vu ../syslog.h ${XINCDIR}/

${XINCDIR}/termios.h:
	@/bin/cp -vu ../termios.h ${XINCDIR}/

${_INCDIR}:
	@/bin/mkdir -p ${_INCDIR}
	/bin/cp -ru ${XINCDIR}/* ${_INCDIR}/

install-local: ${INCSRC} ${XINCDIR} ${SYSDIRS} ${HEADERS}

install-cross: ${_INCDIR}
