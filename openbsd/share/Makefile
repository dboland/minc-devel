ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SHARESRC= ${SRCDIR}/share
SHAREDEST= ${DESTDIR}/usr/share

all: ${SHARESRC} ${TMPDIR} ${SHARESRC}/.patch

${SHARESRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'share' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

# make patch.exe use local tmp directory for proper ACL

${SHDIR}/mk: ${SHARESRC}/mk
	@/bin/mkdir -p ${SHDIR}
	/bin/cp -ru ${SHARESRC}/mk ${SHDIR}/

${SHARESRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 1 -d ${SHARESRC} <../share.patch

install-local: ${SHDIR}/mk

install:
	@${MKDIR} -p ${SHAREDEST}/mk
	${CP} -R ${SHARESRC}/mk ${SHAREDEST}/

clean:
	/bin/rm -rf ${SHDIR}/mk
