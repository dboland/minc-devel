OBJECTS=asctime.o difftime.o localtime.o strftime.o strptime.o wcsftime.o

.SUFFIXES:
.DEFAULT:

LIB= ../time.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/time/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I../include -o $@ $<

