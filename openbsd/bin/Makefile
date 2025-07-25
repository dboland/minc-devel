ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

PROGS= chmod ln mkdir mv rm rmdir test chown chgrp ls cp
OBJECTS= ${PROGS} cat
CFLAGS+= -DNOPIE -I${SRCDIR}/sys
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
	gcc -s -static ${CFLAGS} -o $@ $<

chown: chmod.exe
	/bin/cp -vu $< $@.exe

chgrp: chmod.exe
	/bin/cp -vu $< $@.exe

ln: ${BINSRC}/ln/ln.c
	gcc -s -static ${CFLAGS} -o $@ $<

mkdir: ${BINSRC}/mkdir/mkdir.c
	gcc -s -static ${CFLAGS} -o $@ $<

mv: ${BINSRC}/mv/mv.c
	gcc -s -static ${CFLAGS} -o $@ $<

rm: ${BINSRC}/rm/rm.c
	gcc -s -static ${CFLAGS} -o $@ $<

rmdir: ${BINSRC}/rmdir/rmdir.c
	gcc -s -static ${CFLAGS} -o $@ $<

test: ${BINSRC}/test/test.c
	gcc -s -static ${CFLAGS} -o $@ $<

cat: ${BINSRC}/cat/cat.c
	gcc -s -static ${CFLAGS} -o $@ $<

ls: ls-main.c ${BINSRC}/ls/print.c ${BINSRC}/ls/cmp.c ${BINSRC}/ls/util.c \
	${BINSRC}/ls/ls.c
	gcc -s -static ${CFLAGS} -o $@ $^ -lutil

cp: ${BINSRC}/cp/utils.c ${BINSRC}/cp/cp.c
	gcc -s -static ${CFLAGS} -o $@ $^

install-local: ${BINDIR}

install: ${DESTDIR} ${OBJECTS}
	@mkdir -p ${DESTDIR}/bin
	${INSTALL} ${OBJECTS} ${DESTDIR}/bin/
	@${MAKE} -C ksh install

clean:
	/bin/rm -f *.exe *.o
	@${MAKE} -C ksh clean
