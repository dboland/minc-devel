OBJECTS=rec_close.o rec_delete.o rec_get.o rec_open.o rec_put.o rec_search.o \
	rec_seq.o rec_utils.o

.SUFFIXES:
.DEFAULT:

CFLAGS+= -D__DBINTERFACE_PRIVATE
LIB= ../recno.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/db/recno/%.c
	@/bin/echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

