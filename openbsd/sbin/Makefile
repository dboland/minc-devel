ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= mknod wtrace
OBJECTS= ${PROGS} wrc mkent mount_refs mount_cdfs console
TRACEDIR= ${ROOTDIR}/libtrace
SBINSRC= ${SRCDIR}/sbin
SBINDEST= ${DESTDIR}/sbin

all: ${SBINSRC} ${SBINSRC}/.patch ${OBJECTS} bsd.exe

${SBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'sbin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

%.o: %.c
	${XGCC} -c ${CFLAGS} -DNOPIE -I${ROOTDIR}/include -o $@ $<

%.exe: %.o
	${XGCC} -s -static ${CFLAGS} -o $@ $<

${SBINSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${SBINSRC} <../sbin.patch

libwtrace.o: libwtrace.c
	${CC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<

version.o: bsd.rc
	windres -DVERSION=\\\"${VERSION}\\\" -DRELEASE=\\\"${RELEASE}\\\" $< $@

wrc: wrc.exe
console: console.exe
bsd: bsd.exe

mknod: ${SBINSRC}/mknod/mknod.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

mkent: mkent.o
	${XGCC} -s ${CFLAGS} -o $@ $< ${MINGW}/lib/libiphlpapi.a

wtrace: libwtrace.o wtrace.o
	${XGCC} -s ${CFLAGS} -o $@ -L${TRACEDIR} $^ -ltrace \
	${MINGW}/lib/libkernel32.a ${MINGW}/lib/libuser32.a ${MINGW}/lib/libadvapi32.a

mount_refs: ${SBINSRC}/mount_ntfs/mount_ntfs.c ${SBINSRC}/mount/getmntopts.c
	${XGCC} -s ${CFLAGS} -I${SBINSRC}/mount -o $@ $^

mount_cdfs: mount_refs
	/bin/cp $< $@

install-local:
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu ${PROGS} ${BINDIR}/

install: ${DESTDIR} ${OBJECTS}
	@${MKDIR} -p ${SBINDEST}
	${INSTALL} ${OBJECTS} ${SBINDEST}/
	${INSTALL} -m 02555 -o 0 -g 0 bsd.exe ${SBINDEST}/
	${INSTALL} -m 00754 -o 0 -g 0 setup.sh ${SBINDEST}/

release-clean:
	rm -f version.o

release: release-clean all

clean:
	/bin/rm -f *.exe *.o
