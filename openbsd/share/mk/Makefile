ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

SOURCES= *.mk bsd.README
SHSRC= ${SRCDIR}/share/mk
SHDEST= ${DESTDIR}/usr/share/mk

all: ${SHSRC} ${TMPDIR} .patch

# make patch.exe use local tmp directory for proper ACL
#
.patch:
	/bin/cp -u ${SHSRC}/*.mk .
	/bin/cp -u ${SHSRC}/bsd.README .
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../mk.patch

install:
	@${MKDIR} -p ${SHDEST}
	${CP} ${SOURCES} ${SHDEST}/

patch-clean:
	/bin/rm -rf ${SOURCES} .patch
