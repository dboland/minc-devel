ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

HEADERS= 
INCSRC= ${SRCDIR}/lib/libcrypto
IMPLIB= ${INCSRC}/crypto/libcrypto.dll.a

all: ${INCSRC}

install-cross: ${IMPLIB}
	/bin/cp -uv ${IMPLIB} ${XLIBDIR}/libcrypto.a
