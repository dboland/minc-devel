OBJECTS=gmon.o
OBJECTS+=mcount.c.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/gmon/%.c
	${XGCC} -c ${CFLAGS} -o $@ $<

%.c.o: %.c
	${XGCC} -c ${CFLAGS} -o $@ $<
