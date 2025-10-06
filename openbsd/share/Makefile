ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

SHARESRC= ${SRCDIR}/share
SHAREDEST= ${DESTDIR}/usr/share

all: ${SHARESRC} ${TMPDIR} .patch

${SHARESRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'share' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

# make patch.exe use local tmp directory for proper ACL
#
.patch:
	/bin/cp -ru ${SHARESRC}/mk ./
	TMPDIR=${TMPDIR} /bin/patch -p 1 -d . <../share.patch

install:
	@${MKDIR} -p ${SHAREDEST}/mk
	${CP} -R ./mk ${SHAREDEST}/

patch-clean:
	/bin/rm -rf ./mk .patch

clean:
	/bin/rm -rf ${SHDIR}
