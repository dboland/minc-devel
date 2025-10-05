SOURCES= citrus_utf8.c
OBJECTS= citrus_ctype.o citrus_none.o citrus_utf8.o

.SUFFIXES:
.DEFAULT:

LIB= ../citrus.a

all: ${OBJECTS} ${LIB}
	@echo "done"

${LIB}:
	@ar cru $@ ${OBJECTS}

%.c: ${SRCDIR}/lib/libc/citrus/%.c
	@/bin/cp -uv $< $@

%.o: ${SRCDIR}/lib/libc/citrus/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

citrus_utf8.o: citrus_utf8.c
	@echo "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/lib/libc/citrus -o $@ $<

source: ${SOURCES}
