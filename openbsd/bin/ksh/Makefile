ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

# wait for signals from child executable
#CFLAGS+=-DUSEVFORK
#CFLAGS+=-DDEBUG
#CFLAGS+=-DDEBUG_JOBS
#CFLAGS+=-DDEBUG_SUBST
#CFLAGS+=-DDEBUG_EXIT
#CFLAGS+=-DKSH_DEBUG
CFLAGS+=-DSMALL
#CFLAGS+=-DJOBS

OBJECTS= alloc.o c_ksh.o c_sh.o c_test.o c_ulimit.o edit.o 2021_emacs.o eval.o \
	exec.o expr.o history.o io.o jobs.o lex.o mail.o main.o mknod.o \
	misc.o path.o shf.o syn.o table.o trap.o tree.o tty.o var.o \
	version.o 2021_vi.o debug.o

# Flags for using gmon. MinGW uses libgmon.a, which contains the 
# mcount() function. In OpenBSD, this function is inluded in libc.
# see also: https://ftp.gnu.org/old-gnu/Manuals/gprof-2.9.1/html_node/gprof_25.html
# GNU provides the __builtin_return_address() function for the mcount() params
# OpenBSD provides the parameters via an inline function,
# defined in <machine/profile.h>
# after compiling with below flags, execute once, then:
# gprof -q ksh.exe (gprof.exe comes with the GNU binutils suite)

#CFLAGS+=-pg
#OBJECTS+=/usr/lib/gcrt0.o

all: ksh.exe

%.o: %.c
	@echo -n "$@ "
	@gcc -c ${CFLAGS} -g -DNOPIE -o $@ $<

misc.o: misc.c
	@echo -n "$@ "
	@gcc -c ${CFLAGS} -g -DNOPIE -I${SRCDIR}/lib/libc/gen -o $@ $<

ksh.exe: ${OBJECTS}
	@echo "done."
	gcc -static -s ${CFLAGS} -o $@ ${OBJECTS}

install: ${DESTDIR}
	@mkdir -p ${DESTDIR}/bin
	${INSTALL} ksh ${DESTDIR}/bin/
	ln -f ${DESTDIR}/bin/ksh ${DESTDIR}/bin/sh
	@mkdir -p ${DESTDIR}/usr/share/man/man1
	${INSTALL} sh.1 ksh.1 ${DESTDIR}/usr/share/man/man1/

clean:
	rm -f *.o *.exe

