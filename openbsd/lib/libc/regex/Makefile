OBJECTS = regcomp.o regerror.o regexec.o regfree.o

.SUFFIXES:
.DEFAULT:

LIB= ../regex.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/regex/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<
