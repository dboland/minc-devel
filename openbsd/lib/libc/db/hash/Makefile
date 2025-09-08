OBJECTS=hash.o hash_bigkey.o hash_buf.o hash_func.o hash_log2.o \
	hash_page.o ndbm.o

.SUFFIXES:
.DEFAULT:

CFLAGS+= -D__DBINTERFACE_PRIVATE
LIB= ../hash.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/db/hash/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

