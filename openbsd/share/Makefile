ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SHARESRC= ${SRCDIR}/share
SHAREDEST= ${DESTDIR}/usr/share

all: ${SHARESRC} ${TMPDIR}

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

${SHDIR}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 1 -d ${SHDIR} <../share.patch

install-local: ${SHDIR}/mk ${SHDIR}/.patch

install:
	@${MKDIR} -p ${SHAREDEST}/mk
	${CP} -R ${SHDIR}/mk ${SHAREDEST}/

clean:
	/bin/rm -rf ${SHDIR}
