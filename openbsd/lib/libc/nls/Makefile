OBJECTS=catclose.o catgets.o catopen.o
OBJECTS+=_catclose.o _catgets.o _catopen.o

.SUFFIXES:
.DEFAULT:

LIB= ../nls.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/nls/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

