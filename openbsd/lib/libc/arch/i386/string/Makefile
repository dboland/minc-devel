OBJECTS=bcmp.o bcopy.o bzero.o ffs.o index.o memchr.o memcmp.o memset.o \
	rindex.o strcat.o strcmp.o strcpy.o strlen.c.o \
	strncmp.o strpbrk.c.o strsep.c.o \
	strstr.c.o swab.o
OBJECTS+= memcpy.o memmove.o strchr.o strrchr.o
OBJECTS+= strncat.c.o strncpy.c.o strcspn.c.o strlcpy.c.o strspn.c.o

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS}
	@/bin/echo "done."

%.o: ${SRCDIR}/lib/libc/arch/${ARCH}/string/%.S
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -o $@ $<

%.c.o: ${SRCDIR}/lib/libc/string/%.c
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -o $@ $<
