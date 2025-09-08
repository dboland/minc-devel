OBJECTS=bt_close.o bt_conv.o bt_debug.o bt_delete.o bt_get.o bt_open.o \
	bt_overflow.o bt_page.o bt_put.o bt_search.o bt_seq.o bt_split.o \
	bt_utils.o

.SUFFIXES:
.DEFAULT:

CFLAGS+= -D__DBINTERFACE_PRIVATE
LIB= ../btree.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/db/btree/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

