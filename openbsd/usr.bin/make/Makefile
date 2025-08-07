ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

OBJECTS = arch.o buf.o cmd_exec.o compat.o cond.o dir.o direxpand.o dump.o \
	engine.o \
	error.o for.o init.o job.o lowparse.o main.o make.o memory.o parse.o \
	parsevar.o str.o stats.o suff.o targ.o targequiv.o timestamp.o \
	var.o varmodifiers.o varname.o
UBINSRC= ${SRCDIR}/usr.bin

HOSTCC=${XGCC}
.OBJDIR=.
LDFLAGS= -Wl,--disable-auto-import -s
LDSTATIC= -static -DNOPIE
CFLAGS+= -DNOPIE

all: lst.lib.d beforedepend make.out

CDEFS+=-DHAS_BOOL_H
CDEFS+=-DHAS_PATHS_H
CDEFS+=-DHAS_EXTENDED_GETCWD
#CDEFS+=-DHAS_STATS

DPADD += ${LIBUTIL}
LDADD += lst.lib/liblst.a -lutil
CFLAGS+=${CDEFS} -I${.OBJDIR} -g
HOSTCFLAGS+=${CDEFS} -I${.OBJDIR}

%.o: ${UBINSRC}/make/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

%.d: %
	@${MAKE} -C $< XGCC="${XGCC}" CFLAGS="${CFLAGS}" UBINSRC="${UBINSRC}"

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

make.out: ${OBJECTS}
	@echo "done."
	${HOSTCC} ${LDSTATIC} -o $@ ${OBJECTS} ${LDFLAGS} ${LDADD}

install: ${DESTDIR}
	${INSTALL} make.out ${DESTDIR}${UBINDIR}/make

clean:
	/bin/rm -f *.h *.tmp *.exe ${OBJECTS} make.out
	@${MAKE} -C lst.lib clean

