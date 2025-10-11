ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= gzip id find
XPROGS= arch uname touch find
OBJECTS= ${XPROGS} id du sed cap_mkdb sort xargs tsort mktemp join
UBINSRC= ${SRCDIR}/usr.bin
UBINDIR= /usr/bin

# GNU make can't cd into make if make/make.exe exists?!?!

all: ${UBINSRC} ${TMPDIR} ${UBINSRC}/.patch ${OBJECTS} xinstall
	@echo "done."
	@${MAKE} -C make

${UBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${UBINSRC}/.patch:
	TMPDIR=${TMPDIR} /bin/patch -p 2 -d ${UBINSRC} <../usr.bin.patch

${BINDIR}: ${PROGS} xinstall
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu ${PROGS} ${BINDIR}/
	/bin/cp -vu xinstall ${BINDIR}/install
	/bin/cp -vu manifest ${BINDIR}/install.exe.manifest

${XBINDIR}: ${XPROGS} xinstall
	@/bin/mkdir -p ${XBINDIR}
	/bin/cp -vu ${XPROGS} ${XBINDIR}/
	/bin/cp -vu xinstall ${XBINDIR}/install
	/bin/cp -vu manifest ${XBINDIR}/install.exe.manifest

uname: ${UBINSRC}/uname/uname.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

arch: ${UBINSRC}/arch/arch.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

touch: ${UBINSRC}/touch/touch.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

id: ${UBINSRC}/id/id.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

gzip: ${UBINSRC}/compress/main.c ${UBINSRC}/compress/zopen.c \
	${UBINSRC}/compress/gzopen.c ${UBINSRC}/compress/nullopen.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $^ -lz

xargs: ${UBINSRC}/xargs/xargs.c ${UBINSRC}/xargs/strnsubst.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $^

sort: ${UBINSRC}/sort/append.c ${UBINSRC}/sort/fields.c ${UBINSRC}/sort/files.c \
	${UBINSRC}/sort/fsort.c ${UBINSRC}/sort/init.c ${UBINSRC}/sort/msort.c \
	${UBINSRC}/sort/sort.c ${UBINSRC}/sort/tmp.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $^

tsort: ${UBINSRC}/tsort/tsort.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $< -lutil

mktemp: ${UBINSRC}/mktemp/mktemp.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

join: ${UBINSRC}/join/join.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

find: ${UBINSRC}/find/find.c ${UBINSRC}/find/function.c ${UBINSRC}/find/ls.c \
	${UBINSRC}/find/main.c ${UBINSRC}/find/misc.c ${UBINSRC}/find/operator.c \
	${UBINSRC}/find/option.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $^

# In Windows, "install", "setup" and "patch" seem to be reserved 
# words. When executed with such name, Vista checks if user is
# elevated, yields ERROR_ELEVATION_REQUIRED if not. XP just slows
# down execution to a crawl.
# see: https://irq5.io/2011/06/26/gnu-patch-and-windows-uac/

xinstall: ${UBINSRC}/xinstall/xinstall.c
	@echo -n "$@ "
	@${XGCC} -s -static ${CFLAGS} -DNOPIE -I${UBINSRC}/xinstall -o $@ $<

du: ${UBINSRC}/du/du.c
	@echo -n "$@ "
	@${XGCC} -s ${CFLAGS} -o $@ $< -lutil

sed: ${UBINSRC}/sed/compile.c ${UBINSRC}/sed/main.c ${UBINSRC}/sed/misc.c \
	${UBINSRC}/sed/process.c
	@echo -n "$@ "
	@${XGCC} -s ${CFLAGS} -o $@ $^

cap_mkdb: ${UBINSRC}/cap_mkdb/cap_mkdb.c ${UBINSRC}/cap_mkdb/getinfo.c
	@echo -n "$@ "
	@${XGCC} -s ${CFLAGS} -o $@ $^

install-local: ${BINDIR} ${XBINDIR}

install: ${DESTDIR} ${OBJECTS} xinstall
	@${MKDIR} -p ${DESTDIR}${UBINDIR}
	${INSTALL} -m ${BINMODE} ${OBJECTS} ${DESTDIR}${UBINDIR}/
	${INSTALL} -m ${BINMODE} xinstall ${DESTDIR}${UBINDIR}/install
	${INSTALL} -m ${NONBINMODE} manifest ${DESTDIR}${UBINDIR}/install.manifest
	${INSTALL} -m ${NONBINMODE} manifest ${DESTDIR}${UBINDIR}/patch.manifest
	@${MAKE} -C make install

clean:
	/bin/rm -f *.exe *.o
	${MAKE} -C make clean
