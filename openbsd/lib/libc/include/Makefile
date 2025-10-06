SOURCES= thread_private.h

.SUFFIXES:
.DEFAULT:

all: ${SRCDIR}

%.h: ${SRCDIR}/lib/libc/include/%.h
	@/bin/cp -uv $< $@

source: ${SOURCES}

patch-clean:
	/bin/rm -f ${SOURCES}
