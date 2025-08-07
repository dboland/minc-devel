ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

OBJECTS= dev_mkdb pwd_mkdb
CFLAGS+= -I${SRCDIR}/sys
USBINSRC= ${SRCDIR}/usr.sbin
USBINDIR= /usr/bin

all: ${USBINSRC} ${OBJECTS}

${USBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.sbin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

dev_mkdb: ${USBINSRC}/dev_mkdb/dev_mkdb.c
	${XGCC} -s ${CFLAGS} -o $@ $<

pwd_mkdb: ${USBINSRC}/pwd_mkdb/pwd_mkdb.c
	${XGCC} -s ${CFLAGS} -o $@ $< -lutil

install: ${DESTDIR}
	@${MKDIR} -p ${DESTDIR}${USBINDIR}
	${INSTALL} ${OBJECTS} ${DESTDIR}${USBINDIR}/

clean:
	/bin/rm -f *.exe *.o
