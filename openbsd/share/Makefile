ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SHARESRC= ${SRCDIR}/share
SHAREDIR= ${DESTDIR}/usr/share

${SHARESRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'share' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

# BSD patch needs /tmp

${SHAREDIR}/mk:
	@${MKDIR} -p $@
	cp -R ${SHARESRC}/mk ${SHAREDIR}/
	TMPDIR=/tmp ${PATCH} --posix --silent -p 1 -d ${SHAREDIR} <../share.patch

install: ${SHARESRC} ${SHAREDIR}/mk
