ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= chmod mv rm mkdir rmdir test chown chgrp ls cp
OBJECTS= ${PROGS} cat ln
CFLAGS+= -DNOPIE
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

chmod: ${BINSRC}/chmod/chmod.c
	${XGCC} -s -static ${CFLAGS} -o $@ $<

chown: chmod.exe
	/bin/cp -vu $< $@.exe

chgrp: chmod.exe
	/bin/cp -vu $< $@.exe

ln: ${BINSRC}/ln/ln.c
	${XGCC} -s -static ${CFLAGS} -o $@ $<

mkdir: ${BINSRC}/mkdir/mkdir.c
	${XGCC} -s -static ${CFLAGS} -o $@ $<

mv: ${BINSRC}/mv/mv.c
	${XGCC} -s -static ${CFLAGS} -o $@ $<

rm: ${BINSRC}/rm/rm.c
	${XGCC} -s -static ${CFLAGS} -o $@ $<

rmdir: ${BINSRC}/rmdir/rmdir.c
	${XGCC} -s -static ${CFLAGS} -o $@ $<

test: ${BINSRC}/test/test.c
	${XGCC} -s -static ${CFLAGS} -o $@ $<

cat: ${BINSRC}/cat/cat.c
	${XGCC} -s -static ${CFLAGS} -o $@ $<

ls: ls-main.c ${BINSRC}/ls/print.c ${BINSRC}/ls/cmp.c ${BINSRC}/ls/util.c \
	${BINSRC}/ls/ls.c
	${XGCC} -s -static ${CFLAGS} -o $@ $^ -lutil

cp: ${BINSRC}/cp/utils.c ${BINSRC}/cp/cp.c
	${XGCC} -s -static ${CFLAGS} -o $@ $^

install-local: ${BINDIR}

install: ${DESTDIR} ${OBJECTS}
	@${MKDIR} -p ${DESTDIR}/bin
	${INSTALL} ${OBJECTS} ${DESTDIR}/bin/
	@${MAKE} -C ksh install

clean:
	/bin/rm -f *.exe *.o
	@${MAKE} -C ksh clean
