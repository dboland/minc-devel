OBJECTS=db.o

.SUFFIXES:
.DEFAULT:

CFLAGS+= -D__DBINTERFACE_PRIVATE
LIB= ../db.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/db/db/%.c
	${XGCC} -c ${CFLAGS} -o $@ $<

