OBJECTS=bt_close.o bt_conv.o bt_debug.o bt_delete.o bt_get.o bt_open.o \
	bt_overflow.o bt_page.o bt_put.o bt_search.o bt_seq.o bt_split.o \
	bt_utils.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/db/btree/%.c
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -o $@ $<

