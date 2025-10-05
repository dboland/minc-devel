SOURCES= mcount.c
OBJECTS=gmon.o
OBJECTS+=mcount.c.o

.SUFFIXES:
.DEFAULT:

LIB= ../gmon.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.c: ${SRCDIR}/lib/libc/gmon/%.c
	@/bin/cp -uv $< $@

%.o: ${SRCDIR}/lib/libc/gmon/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

%.c.o: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

source: ${SOURCES}
