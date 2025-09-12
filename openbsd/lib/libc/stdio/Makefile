OBJECTS= asprintf.o clrerr.o fclose.o fdopen.o feof.o ferror.o fflush.o fgetc.o \
	fgetln.o fgetpos.o fgets.o fileno.o findfp.o flags.o fmemopen.o \
	fopen.o fprintf.o fpurge.o fputc.o fputs.o fread.o freopen.o fscanf.o \
	fseek.o fsetpos.o ftell.o funopen.o fvwrite.o fwalk.o fwrite.o \
	getc.o getchar.o getw.o makebuf.o mktemp.o open_memstream.o \
	perror.o printf.o putc.o putchar.o puts.o putw.o \
	refill.o remove.o rewind.o rget.o scanf.o setbuf.o setbuffer.o \
	setvbuf.o snprintf.o sprintf.o sscanf.o stdio.o tempnam.o tmpfile.o \
	tmpnam.o ungetc.o vasprintf.o vfprintf.o vfscanf.o vprintf.o vscanf.o \
	vsnprintf.o vsprintf.o vsscanf.o wbuf.o wsetup.o flockfile.o \
	fgetwc.o fgetws.o fputwc.o fputws.o fwide.o getwc.o getwchar.o \
	putwc.o putwchar.o ungetwc.o \
	fwprintf.o swprintf.o vfwprintf.o vswprintf.o vwprintf.o wprintf.o \
	fwscanf.o swscanf.o vfwscanf.o vswscanf.o vwscanf.o wscanf.o \
	getdelim.o getline.o dprintf.o vdprintf.o

CFLAGS+= -DFLOATING_POINT -DPRINTF_WIDE_CHAR -DSCANF_WIDE_CHAR
CFLAGS+= -I${SRCDIR}/lib/libc/include

.SUFFIXES:
.DEFAULT:

LIB= ../stdio.a

all: ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/stdio/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/lib/libc/gdtoa -I${SRCDIR}/lib/libc/arch/${ARCH}/gdtoa -o $@ $<
