ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

SSLSRC= ${SRCDIR}/lib/libssl
SSLDIR= ${XINCDIR}/openssl
HEADERS= ${SSLDIR}/asn1.h

all: ${TMPDIR} ${SSLSRC}/.patch

${SSLSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${SSLSRC} <../libssl.patch

${SSLDIR}/asn1.h:
	@/bin/mkdir -p ${SSLDIR}
	@/bin/cp -vu ${SSLSRC}/src/crypto/asn1/asn1.h $@

install-local: ${HEADERS}
