OBJECTS=dlfcn_stubs.o

.SUFFIXES:
.DEFAULT:

LIB= ../dlfcn.a

all: ${OBJECTS} ${LIB}

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/dlfcn/%.c
	${XGCC} -c ${CFLAGS} -o $@ $<

