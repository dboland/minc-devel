ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/fcntl.h ${XINCDIR}/stdarg.h ${XINCDIR}/stdint.h \
	${XINCDIR}/syslog.h ${XINCDIR}/termios.h
INCSRC= ${SRCDIR}/include
INCDEST= ${DESTDIR}/usr/include

all: ${INCSRC} ${TMPDIR} ${INCSRC}/.patch

${INCSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'include' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

# make patch.exe use local tmp directory for proper ACL
#
${INCSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${INCSRC} <../include.patch

${XINCDIR}:
	@/bin/mkdir -p ${XINCDIR}
	/bin/cp -ru ${INCSRC}/* ${XINCDIR}/

${XINCDIR}/fcntl.h:
	@/bin/cp -vu fcntl.h ${XINCDIR}/

${XINCDIR}/stdarg.h:
	@/bin/cp -vu stdarg.h ${XINCDIR}/

${XINCDIR}/stdint.h:
	@/bin/cp -vu stdint.h ${XINCDIR}/

${XINCDIR}/syslog.h:
	@/bin/cp -vu syslog.h ${XINCDIR}/

${XINCDIR}/termios.h:
	@/bin/cp -vu termios.h ${XINCDIR}/

install-local: ${XINCDIR} ${HEADERS}

${INCDEST}: ${XINCDIR}
	${CP} -R ${XINCDIR} ${DESTDIR}/usr/

install: ${INCDEST}
