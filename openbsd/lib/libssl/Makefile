ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/openssl/asn1.h
INCSRC= ${SRCDIR}/lib/libssl

all: ${TMPDIR} ${INCSRC}/.patch

${INCSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${INCSRC} <../libssl.patch

${XINCDIR}/openssl/asn1.h:
	@/bin/mkdir -p ${XINCDIR}/openssl
	@/bin/cp -vu ${INCSRC}/src/crypto/asn1/asn1.h $@

install-local: ${HEADERS}
