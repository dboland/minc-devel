ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

PROG= minc.exe
SBINDEST= ${DESTDIR}/sbin

all: ${PROG}

version.o: version.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

${PROG}: version.o minc.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $^

install:
	${INSTALL} -m 02555 -o 0 -g 0 ${PROG} ${SBINDEST}/
