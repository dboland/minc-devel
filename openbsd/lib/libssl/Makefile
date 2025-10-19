ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

INCSRC= ${SRCDIR}/lib/libssl
IMPLIB= ${INCSRC}/ssl/libssl.dll.a
SSLDIR= src/crypto/asn1
SOURCES= ${SSLDIR}/asn1.h

all: ${INCSRC} .patch

.patch:
	/bin/mkdir -p ${SSLDIR}
	/bin/cp -uv ${INCSRC}/${SSLDIR}/asn1.h ${SSLDIR}/
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../libssl.patch

patch-clean:
	/bin/rm -rf ${SSLDIR} .patch

install-local:
	@/bin/mkdir -p ${INCDIR}/openssl
	/bin/cp -uv ${SSLDIR}/asn1.h ${INCDIR}/openssl/

install-cross: ${IMPLIB}
	/bin/cp -uv ${IMPLIB} ${XLIBDIR}/libssl.a
