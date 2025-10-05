ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SOURCES= ctype.h getopt.h math.h netdb.h resolv.h signal.h stdio.h \
	stdlib.h time.h unistd.h
HEADERS= ${XINCDIR}/fcntl.h ${XINCDIR}/stdarg.h ${XINCDIR}/stdint.h \
	${XINCDIR}/syslog.h ${XINCDIR}/termios.h
INCSRC= ${SRCDIR}/include

all: ${INCSRC} ${TMPDIR} ${SOURCES} .patch

${INCSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'include' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

%.h: ${INCSRC}/%.h
	/bin/cp -u $< $@

${XINCDIR}:
	@/bin/mkdir -p ${XINCDIR}
	/bin/cp -ru ${INCSRC}/* ${XINCDIR}/
	/bin/cp -u ${SOURCES} ${XINCDIR}/

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

.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 1 -d . <../include.patch

install-local: ${XINCDIR} ${HEADERS}
