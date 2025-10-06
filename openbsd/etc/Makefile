ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SOURCES= man.conf motd gettytab login.conf sysctl.conf ftpchroot ftpusers
ETCSRC= ${SRCDIR}/etc
ETCDIR= ${DESTDIR}/etc
# install-only
CONFIG= ${ETCDIR}/profile

all: ${ETCSRC} ${SOURCES} ttys MAKEDEV

${ETCSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'etc' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${ETCDIR}: ${SOURCES}
	@${MKDIR} -p ${ETCDIR}
	${INSTALL} ${SOURCES} ttys MAKEDEV ${ETCDIR}/

${ETCDIR}/profile:
	${INSTALL} profile ${ETCDIR}/

%: ${ETCSRC}/%
	@/bin/cp -uv $< $@

login.conf:
	@/bin/cp -uv ${ETCSRC}/etc.${ARCH}/login.conf $@

install: ${ETCDIR} ${CONFIG}
