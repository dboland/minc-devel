OBJECTS= crypt.o crypt2.o cryptutil.o blowfish.o bcrypt.o arc4random.o
CFLAGS+= -I${SRCDIR}/lib/libc/include

.SUFFIXES:
.DEFAULT:

LIB= ../crypt.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/crypt/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

