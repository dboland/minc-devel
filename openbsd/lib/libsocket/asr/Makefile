OBJECTS= asr.c.o asr_debug.o asr_utils.o \
	getrrsetbyname_async.o \
	gethostnamadr_async.o getnetnamadr_async.o \
	getaddrinfo_async.o getnameinfo_async.o \
	res_send_async.o res_search_async.o res_query.o
OBJECTS+= getaddrinfo.o gethostnamadr.o getnameinfo.o getnetnamadr.o \
	getrrsetbyname.o res_debug.o res_init.c.o res_mkquery.o \
	sethostent.o

SHOBJECTS= asr.c.so asr_debug.so asr_utils.so \
	getrrsetbyname_async.so \
	gethostnamadr_async.so getnetnamadr_async.so \
	getaddrinfo_async.so getnameinfo_async.so \
	res_send_async.so res_search_async.so res_query.so
SHOBJECTS+= getaddrinfo.so gethostnamadr.so getnameinfo.so getnetnamadr.so \
	getrrsetbyname.so res_debug.so res_init.c.so res_mkquery.so \
	sethostent.so

LIB= ../asr.a

all: ${OBJECTS} ${SHOBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.o: ${SRCDIR}/lib/libc/asr/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DNOPIE -DASR_OPT_HOSTALIASES -o $@ $<

%.c.o: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DNOPIE -DASR_OPT_HOSTALIASES -I${SRCDIR}/lib/libc/asr -o $@ $<

%.so: ${SRCDIR}/lib/libc/asr/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DASR_OPT_HOSTALIASES -o $@ $<

%.c.so: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -DASR_OPT_HOSTALIASES -I${SRCDIR}/lib/libc/asr -o $@ $<
