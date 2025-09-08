OBJECTS= creat.o getdtablesize.o getwd.o \
	setpgrp.o sigcompat.o killpg.o gethostid.o sethostid.o

.SUFFIXES:
.DEFAULT:

LIB= ../compat-43.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/compat-43/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<
