OBJECTS=htonl.o htons.o ntohl.o ntohs.o

.SUFFIXES:
.DEFAULT:

LIB= ../net.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/arch/${ARCH}/net/%.S
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

