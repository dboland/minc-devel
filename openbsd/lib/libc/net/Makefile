OBJECTS= send.o recv.o

.SUFFIXES:
.DEFAULT:

LIB= ../net.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/net/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

