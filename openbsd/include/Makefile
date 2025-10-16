ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${INCDIR}/fcntl.h ${INCDIR}/stdarg.h ${INCDIR}/stdint.h \
	${INCDIR}/syslog.h ${INCDIR}/termios.h
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

${INCDIR}:
	@/bin/mkdir -p ${INCDIR}
	/bin/cp -ru ${INCSRC}/* ${INCDIR}/

${INCDIR}/fcntl.h:
	@/bin/cp -vu fcntl.h ${INCDIR}/

${INCDIR}/stdarg.h:
	@/bin/cp -vu stdarg.h ${INCDIR}/

${INCDIR}/stdint.h:
	@/bin/cp -vu stdint.h ${INCDIR}/

${INCDIR}/syslog.h:
	@/bin/cp -vu syslog.h ${INCDIR}/

${INCDIR}/termios.h:
	@/bin/cp -vu termios.h ${INCDIR}/

install-local: ${INCDIR} ${HEADERS}

install-cross:
	/bin/cp -ru ${INCDIR} ${XPREFIX}/

${INCDEST}: ${INCDIR}
	${CP} -R ${INCDIR} ${DESTDIR}/usr/

install: ${INCDEST}
