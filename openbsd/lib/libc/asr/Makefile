SOURCES= asr.c

.SUFFIXES:
.DEFAULT:

all: ${SRCDIR}

%.c: ${SRCDIR}/lib/libc/asr/%.c
	@/bin/cp -uv $< $@

source: ${SOURCES}

patch-clean:
	/bin/rm -f ${SOURCES}

