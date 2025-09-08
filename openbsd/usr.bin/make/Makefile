ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

OBJECTS = arch.o buf.o cmd_exec.o compat.o cond.o dir.o direxpand.o dump.o \
	engine.o \
	error.o for.o init.o job.o lowparse.o main.o make.o memory.o parse.o \
	parsevar.o str.o stats.o suff.o targ.o targequiv.o timestamp.o \
	var.o varmodifiers.o varname.o
UBINSRC= ${SRCDIR}/usr.bin
UBINDIR= /usr/bin

PROG= make.exe
HOSTCC=${XGCC}
.OBJDIR=.
LDFLAGS= -s
LDSTATIC= -static -DNOPIE
#CFLAGS+= -DNOPIE

all: liblst.a beforedepend ${PROG}

CDEFS+=-DHAS_BOOL_H
CDEFS+=-DHAS_PATHS_H
CDEFS+=-DHAS_EXTENDED_GETCWD
#CDEFS+=-DHAS_STATS

DPADD += ${LIBUTIL}
LDADD += liblst.a -lutil
#CFLAGS+=${CDEFS}
HOSTCFLAGS+= ${CFLAGS} ${CDEFS}

%.o: ${UBINSRC}/make/%.c
	@#echo -n "$@ "
	@${XGCC} -c ${HOSTCFLAGS} -o $@ $<

liblst.a:
	@${MAKE} -C lst.lib XGCC="${XGCC}" CFLAGS="${CFLAGS}" UBINSRC="${UBINSRC}"

beforedepend: varhashconsts.h condhashconsts.h nodehashconsts.h
# may need tweaking if you add variable synonyms or change the hash function
MAGICVARSLOTS=77
MAGICCONDSLOTS=65

varhashconsts.h: generate
	${.OBJDIR}/generate 1 ${MAGICVARSLOTS} >$@.tmp && mv $@.tmp $@

condhashconsts.h: generate
	${.OBJDIR}/generate 2 ${MAGICCONDSLOTS} >$@.tmp && mv $@.tmp $@

nodehashconsts.h: generate
	${.OBJDIR}/generate 3 0 >$@.tmp && mv $@.tmp $@

generate: ${UBINSRC}/make/generate.c ${UBINSRC}/make/stats.c ${UBINSRC}/make/memory.c
	${HOSTCC} ${HOSTCFLAGS} ${LDSTATIC} -o $@ $? ${LDFLAGS} ${LDADD}

${PROG}: ${OBJECTS}
	@echo "done."
	${XGCC} ${CFLAGS} -o $@ ${OBJECTS} ${LDFLAGS} ${LDADD}

install: ${DESTDIR} ${PROG}
	${INSTALL} -m ${BINMODE} ${PROG} ${DESTDIR}${UBINDIR}/make

bin-clean:
	/bin/rm -f ${PROG}

clean: bin-clean
	/bin/rm -f *.h *.tmp *.exe ${OBJECTS}
	@${MAKE} -C lst.lib clean

