OBJECTS=mpool.o

.SUFFIXES:
.DEFAULT:

CFLAGS+= -D__DBINTERFACE_PRIVATE
LIB= ../mpool.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/db/mpool/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

