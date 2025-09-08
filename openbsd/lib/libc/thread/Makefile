OBJECTS=unithread_malloc_lock.o unithread_mutex.o unithread_tag.o

.SUFFIXES:
.DEFAULT:

LIB= ../thread.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/thread/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I../include -o $@ $<

