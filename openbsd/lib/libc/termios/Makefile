OBJECTS= cfgetispeed.o cfgetospeed.o cfmakeraw.o cfsetispeed.o cfsetospeed.o \
	cfsetspeed.o tcdrain.o tcflow.o tcflush.o tcgetattr.o tcgetpgrp.o \
	tcsendbreak.o tcsetattr.o tcsetpgrp.o tcgetsid.o

.SUFFIXES:
.DEFAULT:

LIB= ../termios.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/termios/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<
