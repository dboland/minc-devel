ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SOURCES= man.conf motd gettytab login.conf sysctl.conf ftpchroot ftpusers \
	syslog.conf
ETCSRC= ${SRCDIR}/etc
ETCDEST= ${DESTDIR}/etc

all: ${ETCSRC} ${SOURCES}

${ETCSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'etc' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

%: ${ETCSRC}/%
	@/bin/cp -uv $< $@

login.conf:
	@/bin/cp -uv ${ETCSRC}/etc.${ARCH}/login.conf $@

${ETCDEST}: ${SOURCES} ttys MAKEDEV
	@${MKDIR} -p ${ETCDEST}
	${INSTALL} $^ ${ETCDEST}/

# install first-time only
#
${ETCDEST}/profile:
	${INSTALL} profile ${ETCDEST}/

install: ${ETCDEST} ${ETCDEST}/profile
