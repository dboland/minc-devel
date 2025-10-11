SOURCES= getservent.c
OBJECTS= send.o recv.o

.SUFFIXES:
.DEFAULT:

LIB= ../net.a

all: ${OBJECTS} ${LIB}
	@echo "done."

%.c: ${SRCDIR}/lib/libc/net/%.c
	@/bin/cp -uv $< $@

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/net/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

source: ${SOURCES}

patch-clean:
	/bin/rm -f ${SOURCES}
