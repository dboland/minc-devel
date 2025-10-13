ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= ln mkdir chmod ls cp
XPROGS= ${PROGS} mv rmdir test chown chgrp
OBJECTS= ${XPROGS} cat echo
CFLAGS+= -DNOPIE -s -static
BINSRC= ${SRCDIR}/bin

all: ${BINSRC} ${OBJECTS}
	@${MAKE} -C ksh

${BINSRC}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD 'bin' directory ($@) not found. Make sure"
	@echo "| you extracted this directory from the source archive."
	@echo " ---------------------------------------------------"
	@exit 1

${BINDIR}: ${PROGS}
	@/bin/mkdir -p ${BINDIR}
	/bin/cp -vu ${PROGS} ${BINDIR}/

${XBINDIR}: ${XPROGS}
	@/bin/mkdir -p ${XBINDIR}
	/bin/cp -vu ${XPROGS} ${XBINDIR}/

chmod: ${BINSRC}/chmod/chmod.c
	${XGCC} ${CFLAGS} -o $@ $<

chown: chmod.exe
	/bin/cp -vu $< $@.exe

chgrp: chmod.exe
	/bin/cp -vu $< $@.exe

ln: ${BINSRC}/ln/ln.c
	${XGCC} ${CFLAGS} -o $@ $<

mkdir: ${BINSRC}/mkdir/mkdir.c
	${XGCC} ${CFLAGS} -o $@ $<

mv: ${BINSRC}/mv/mv.c
	${XGCC} ${CFLAGS} -o $@ $<

rmdir: ${BINSRC}/rmdir/rmdir.c
	${XGCC} ${CFLAGS} -o $@ $<

test: ${BINSRC}/test/test.c
	${XGCC} ${CFLAGS} -o $@ $<

cat: ${BINSRC}/cat/cat.c
	${XGCC} ${CFLAGS} -o $@ $<

echo: ${BINSRC}/echo/echo.c
	${XGCC} ${CFLAGS} -o $@ $<

ls: ${BINSRC}/ls/main.c ${BINSRC}/ls/print.c ${BINSRC}/ls/cmp.c ${BINSRC}/ls/util.c \
	${BINSRC}/ls/ls.c
	${XGCC} ${CFLAGS} -o $@ $^ -lutil

cp: ${BINSRC}/cp/utils.c ${BINSRC}/cp/cp.c
	${XGCC} ${CFLAGS} -o $@ $^

install-local: ${BINDIR} ${XBINDIR}

install: ${DESTDIR} ${OBJECTS}
	@${MKDIR} -p ${DESTDIR}/bin
	${INSTALL} -m ${BINMODE} ${OBJECTS} ${DESTDIR}/bin/
	@${MAKE} -C ksh install

clean:
	/bin/rm -f *.exe *.o
	@${MAKE} -C ksh clean
