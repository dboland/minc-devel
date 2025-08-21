OBJECTS=crypt.o crypt2.o cryptutil.o blowfish.o bcrypt.o arc4random.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@echo "done."

%.o: ${SRCDIR}/lib/libc/crypt/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I../include -o $@ $<

