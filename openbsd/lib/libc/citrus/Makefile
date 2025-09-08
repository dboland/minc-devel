OBJECTS= citrus_ctype.o citrus_none.o citrus_utf8.o

.SUFFIXES:
.DEFAULT:

LIB= ../citrus.a

all: ${OBJECTS} ${LIB}
	@echo "done"

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/citrus/%.c
	@echo "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

citrus_utf8.o: citrus_utf8.c
	@echo "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/lib/libc/citrus -o $@ $<
