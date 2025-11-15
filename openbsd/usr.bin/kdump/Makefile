ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

PROG= kdump
SOURCES= ktrstruct.c mksubr
OBJECTS= kdump.o ioctl.o kdump_subr.o ktrstruct.o
BINSRC= ${SRCDIR}/usr.bin/kdump
KTRSRC= ${BINSRC}/../ktrace
SYS_DIR= ${SRCDIR}/sys
CFLAGS+= -I${BINSRC}/../ktrace -I${BINSRC} -I${SYS_DIR}
CFLAGS+= -Wno-implicit-function-declaration
UBINDEST= ${DESTDIR}/usr/bin

all: ${TMPDIR} ${SOURCES} .patch ${PROG}

%.c: ${BINSRC}/%.c
	@/bin/cp -uv $< $@

%.o: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DNOPIE -o $@ $<

.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d . <../kdump.patch

kdump_subr.c: mksubr
	@sh mksubr ${SYS_DIR} > $@

ioctl.c: ${BINSRC}/mkioctls
	@CC="${XGCC}" sh ${BINSRC}/mkioctls \
	${SYS_DIR}/sys/ioctl.h \
	${SYS_DIR}/sys/ioctl_compat.h \
	${SYS_DIR}/crypto/cryptodev.h \
	${SYS_DIR}/dev/biovar.h \
	${SYS_DIR}/dev/systrace.h \
	${SYS_DIR}/dev/wscons/wsconsio.h \
	${SYS_DIR}/dev/vndioctl.h \
	${SYS_DIR}/dev/pci/drm/drm.h \
	${SYS_DIR}/dev/usb/usb.h \
	${SYS_DIR}/net/bpf.h \
	${SYS_DIR}/net/if_ppp.h \
	${SYS_DIR}/net/if_pppoe.h \
	${SYS_DIR}/net/if_tun.h \
	${SYS_DIR}/net/pfvar.h \
	${SYS_DIR}/net80211/ieee80211_ioctl.h \
	${SYS_DIR}/netinet6/in6_var.h \
	${SYS_DIR}/sys/tty.h \
	${SYS_DIR}/sys/ataio.h \
	${SYS_DIR}/sys/audioio.h \
	${SYS_DIR}/sys/cdio.h \
	${SYS_DIR}/sys/chio.h \
	${SYS_DIR}/sys/dkio.h \
	${SYS_DIR}/sys/filio.h \
	${SYS_DIR}/sys/gpio.h \
	${SYS_DIR}/sys/mtio.h \
	${SYS_DIR}/sys/pciio.h \
	${SYS_DIR}/sys/radioio.h \
	${SYS_DIR}/sys/scanio.h \
	${SYS_DIR}/sys/scsiio.h \
	${SYS_DIR}/sys/sockio.h \
	${SYS_DIR}/sys/videoio.h \
	> ioctl.c

mksubr: ${BINSRC}/mksubr
	@/bin/cp -uv $< $@

${PROG}: ${OBJECTS}
	@echo "done."
	${XGCC} -s -static ${CFLAGS} -o ${PROG} ${KTRSRC}/subr.c ${OBJECTS} -lsocket

install:
	${INSTALL} -m ${BINMODE} ${PROG} ${UBINDEST}/

patch-clean:
	/bin/rm -f ${SOURCES} .patch

clean:
	/bin/rm -f *.o *.exe ioctl.c kdump_subr.c
