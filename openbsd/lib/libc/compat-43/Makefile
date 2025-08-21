OBJECTS= creat.o getdtablesize.o getwd.o \
	setpgrp.o sigcompat.o killpg.o gethostid.o sethostid.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/compat-43/%.c
	@/bin/echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

