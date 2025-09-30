ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

PROG= ksh
#CFLAGS+=-DDEBUG
#CFLAGS+=-DDEBUG_JOBS
#CFLAGS+=-DDEBUG_SUBST
#CFLAGS+=-DDEBUG_EXIT
#CFLAGS+=-DKSH_DEBUG
CFLAGS+=-DSMALL
#CFLAGS+=-DJOBS
KSHSRC= ${SRCDIR}/bin/ksh
MANFILES=  ${KSHSRC}/sh.1 ${KSHSRC}/ksh.1
CFLAGS+= -I.

# wait for signals from child executable
#CFLAGS+=-Dfork=vfork

HEADERS= config.h table.h tree.h expand.h lex.h proto.h c_test.h ksh_limval.h

# emacs.c and vi.c were taken from 2021 (v7.4) distro for utf8 handling
#
OBJECTS= alloc.o c_ksh.o c_sh.o c_test.o c_ulimit.o edit.o emacs.o eval.o \
	exec.o expr.o history.o io.o jobs.o lex.o mail.o main.o mknod.o \
	misc.o path.o shf.o syn.o table.o trap.o tree.o tty.o var.o \
	version.o vi.o

# temporary debugging code
#OBJECTS+= debug.o

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

all: ${TMPDIR} ${HEADERS} ${PROG}

%.h: ${KSHSRC}/%.h
	@/bin/cp -uv $< $@

%.c: ${KSHSRC}/%.c
	@/bin/cp -uv $< $@

%.o: %.c
	@#echo -n "$@ "
	${XGCC} -c -g ${CFLAGS} -DNOPIE -o $@ $<

misc.o: misc.c
	@#echo -n "$@ "
	${XGCC} -c -g ${CFLAGS} -DNOPIE -I${SRCDIR}/lib/libc/gen -o $@ $<

${PROG}: ${OBJECTS}
	@#echo "done."
	${XGCC} -static -s ${CFLAGS} -o $@ ${OBJECTS}

install: ${DESTDIR} ${PROG}
	${INSTALL} -m ${BINMODE} ${PROG} ${DESTDIR}/bin/
	${LN} -f ${DESTDIR}/bin/ksh ${DESTDIR}/bin/sh
	@${MKDIR} -p ${DESTDIR}/usr/share/man/man1
	${INSTALL} ${MANFILES} ${DESTDIR}/usr/share/man/man1/

clean:
	/bin/rm -f *.o ${PROG}

