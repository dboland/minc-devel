ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

FILES= man.conf motd gettytab login.conf sysctl.conf ftpchroot ftpusers
ETCSRC= ${SRCDIR}/etc
ETCDIR= ${DESTDIR}/etc

all: ${ETCSRC} ${FILES} ttys MAKEDEV

${ETCSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'etc' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${ETCDIR}: ${FILES}
	@${MKDIR} -p ${ETCDIR}
	${INSTALL} ${FILES} ttys MAKEDEV ${ETCDIR}/

%: ${ETCSRC}/%
	@/bin/cp -uv $< $@

login.conf:
	@/bin/cp -uv ${ETCSRC}/etc.${ARCH}/login.conf $@

install: ${ETCDIR}
