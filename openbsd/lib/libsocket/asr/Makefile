OBJECTS= asr_debug.o asr_utils.o \
	getrrsetbyname_async.o \
	gethostnamadr_async.o getnetnamadr_async.o \
	getaddrinfo_async.o getnameinfo_async.o \
	res_init.o res_send_async.o res_search_async.o res_query.o
OBJECTS+= getaddrinfo.o gethostnamadr.o getnameinfo.o getnetnamadr.o \
	getrrsetbyname.o res_debug.o res_mkquery.o \
	sethostent.o

# patched sources
OBJECTS+= asr.c.o

SHOBJECTS= asr_debug.so asr_utils.so \
	getrrsetbyname_async.so \
	gethostnamadr_async.so getnetnamadr_async.so \
	getaddrinfo_async.so getnameinfo_async.so \
	res_init.so res_send_async.so res_search_async.so res_query.so
SHOBJECTS+= getaddrinfo.so gethostnamadr.so getnameinfo.so getnetnamadr.so \
	getrrsetbyname.so res_debug.so res_mkquery.so \
	sethostent.so

# patched sources
SHOBJECTS+= asr.c.so

LIB= ../asr.a
CFLAGS+= -DASR_OPT_HOSTALIASES
#CFLAGS+= -Wno-attributes
CFLAGS+= -DNETDB_IMPORT=
LIBCDIR= ../../libc/asr

.SUFFIXES:
.DEFAULT:

all: ${OBJECTS} ${SHOBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.c: ${SRCDIR}/lib/libc/asr/%.c
	@/bin/cp -uv $< $@

%.o: ${SRCDIR}/lib/libc/asr/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DNOPIE -o $@ $<

%.c.o: ${LIBCDIR}/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DNOPIE -I${SRCDIR}/lib/libc/asr -o $@ $<

%.so: ${SRCDIR}/lib/libc/asr/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

%.c.so: ${LIBCDIR}/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -I${SRCDIR}/lib/libc/asr -o $@ $<
