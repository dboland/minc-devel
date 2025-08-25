ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= gzip id
XPROGS= arch uname touch
OBJECTS= ${PROGS} ${XPROGS} du env find xargs sed sort cap_mkdb
UBINSRC= ${SRCDIR}/usr.bin
UBINDIR= /usr/bin

# GNU make can't cd into make if make/make.exe exists?!?!

all: ${UBINSRC} ${OBJECTS} install.exe

${UBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${BINDIR}: ${PROGS} install.exe
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu install ${PROGS} ${BINDIR}/
	/bin/cp -vu manifest ${BINDIR}/install.exe.manifest

${XBINDIR}: ${XPROGS} install.exe
	@/bin/mkdir -p ${XBINDIR}
	/bin/cp -vu install ${XPROGS} ${XBINDIR}/
	/bin/cp -vu manifest ${XBINDIR}/install.exe.manifest

uname: ${UBINSRC}/uname/uname.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

arch: ${UBINSRC}/arch/arch.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

touch: ${UBINSRC}/touch/touch.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

id: ${UBINSRC}/id/id.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $<

# In Windows, "install", "setup" and "patch" seem to be reserved 
# words. When executed with such name, Vista checks if user is
# elevated, yields ERROR_ELEVATION_REQUIRED if not. XP just slows
# down execution to a crawl.
# see: https://irq5.io/2011/06/26/gnu-patch-and-windows-uac/

install.exe: xinstall.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -I${UBINSRC}/xinstall -o $@ $<

gzip: ${UBINSRC}/compress/main.c ${UBINSRC}/compress/zopen.c \
	${UBINSRC}/compress/gzopen.c ${UBINSRC}/compress/nullopen.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $^ -lz

env: env.c
	${XGCC} -s ${CFLAGS} -o $@ $<

du: ${UBINSRC}/du/du.c
	${XGCC} -s ${CFLAGS} -o $@ $< -lutil

find: ${UBINSRC}/find/find.c ${UBINSRC}/find/function.c ${UBINSRC}/find/ls.c \
	${UBINSRC}/find/main.c ${UBINSRC}/find/misc.c ${UBINSRC}/find/operator.c \
	${UBINSRC}/find/option.c
	${XGCC} -s ${CFLAGS} -o $@ $^

xargs: xargs.c ${UBINSRC}/xargs/strnsubst.c
	${XGCC} -s ${CFLAGS} -I${UBINSRC}/xargs -o $@ $^

sed: ${UBINSRC}/sed/compile.c ${UBINSRC}/sed/main.c ${UBINSRC}/sed/misc.c \
	${UBINSRC}/sed/process.c
	${XGCC} -s ${CFLAGS} -o $@ $^

sort: sort.c ${UBINSRC}/sort/append.c ${UBINSRC}/sort/fields.c \
	${UBINSRC}/sort/files.c ${UBINSRC}/sort/fsort.c ${UBINSRC}/sort/init.c \
	${UBINSRC}/sort/msort.c ${UBINSRC}/sort/tmp.c
	${XGCC} -s ${CFLAGS} -I${UBINSRC}/sort -o $@ $^

cap_mkdb: ${UBINSRC}/cap_mkdb/cap_mkdb.c ${UBINSRC}/cap_mkdb/getinfo.c
	${XGCC} -s ${CFLAGS} -o $@ $^

install-local: ${BINDIR} ${XBINDIR}

install: ${DESTDIR} ${OBJECTS} install.exe
	@${MKDIR} -p ${DESTDIR}${UBINDIR}
	${INSTALL} install ${OBJECTS} ${DESTDIR}${UBINDIR}/
	${INSTALL} -m 00444 manifest ${DESTDIR}${UBINDIR}/install.manifest
	${INSTALL} -m 00444 manifest ${DESTDIR}${UBINDIR}/patch.manifest

clean:
	/bin/rm -f *.exe *.o
