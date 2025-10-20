ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

INCSRC= ${SRCDIR}/lib/libssl
IMPLIB= ${INCSRC}/ssl/libssl.dll.a

all: ${INCSRC}

install-cross: ${IMPLIB}
	/bin/cp -uv ${IMPLIB} ${XLIBDIR}/libssl.a
