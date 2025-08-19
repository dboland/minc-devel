ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= gzip id patch
XPROGS= arch uname touch
OBJECTS= ${PROGS} ${XPROGS} grep du env find xargs
#CFLAGS+= -I${SRCDIR}/sys
UBINSRC= ${SRCDIR}/usr.bin
UBINDIR= /usr/bin

# GNU make can't cd into make if make/make.exe exists?!?!

all: ${UBINSRC} ${OBJECTS} install.exe
	@${MAKE} -C make UBINSRC="${UBINSRC}"

${UBINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'usr.bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${BINDIR}: ${PROGS} install.exe
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu ${PROGS} install ${BINDIR}/
	/bin/cp -vu manifest ${BINDIR}/install.exe.manifest
	/bin/cp -vu manifest ${BINDIR}/patch.exe.manifest

${XBINDIR}: ${XPROGS} install.exe
	@/bin/mkdir -p ${XBINDIR}
	/bin/cp -vu ${XPROGS} install ${XBINDIR}/
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

patch: ${UBINSRC}/patch/patch.c ${UBINSRC}/patch/pch.c ${UBINSRC}/patch/inp.c \
	${UBINSRC}/patch/util.c ${UBINSRC}/patch/backupfile.c ${UBINSRC}/patch/mkpath.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $^

gzip: ${UBINSRC}/compress/main.c ${UBINSRC}/compress/zopen.c \
	${UBINSRC}/compress/gzopen.c ${UBINSRC}/compress/nullopen.c
	${XGCC} -s -static ${CFLAGS} -DNOPIE -o $@ $^ -lz

grep: grep.c ${UBINSRC}/grep/binary.c ${UBINSRC}/grep/file.c ${UBINSRC}/grep/mmfile.c \
	${UBINSRC}/grep/queue.c ${UBINSRC}/grep/util.c
	${XGCC} -s ${CFLAGS} -I${UBINSRC}/grep -o $@ $^ -lz

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

install-local: ${BINDIR} ${XBINDIR}

install: ${DESTDIR} ${OBJECTS} install.exe
	@${MKDIR} -p ${DESTDIR}${UBINDIR}
	${INSTALL} ${OBJECTS} install ${DESTDIR}${UBINDIR}/
	${INSTALL} -m 00444 manifest ${DESTDIR}${UBINDIR}/install.manifest
	${INSTALL} -m 00444 manifest ${DESTDIR}${UBINDIR}/patch.manifest
	@${MAKE} -C make install UBINDIR="${UBINDIR}"

clean:
	/bin/rm -f *.exe *.o
	@${MAKE} -C make clean
