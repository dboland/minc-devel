ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/fcntl.h ${XINCDIR}/stdarg.h ${XINCDIR}/stdint.h \
	${XINCDIR}/syslog.h ${XINCDIR}/termios.h \
	${XINCDIR}/.patch
INCSRC= ${SRCDIR}/include

all: ${INCSRC} ${TMPDIR}

${INCSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'include' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${XINCDIR}/machine:
	@/bin/mkdir -p $@
	/bin/cp -ru  ${SRCDIR}/sys/arch/${ARCH}/include/* $@/

${XINCDIR}: ${XINCDIR}/machine
	@/bin/mkdir -p ${XINCDIR}
	/bin/cp -ru ${INCSRC}/* ${XINCDIR}/

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

# make patch.exe use local tmp directory for proper ACL

${XINCDIR}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 0 -d ${XPREFIX} <../include.patch

install-local: ${XINCDIR} ${HEADERS}
