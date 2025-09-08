HELPER=md5hl.c rmd160hl.c sha1hl.c sha224hl.c sha256hl.c sha384hl.c sha512hl.c
OBJECTS=md5hl.o rmd160hl.o sha1hl.o sha224hl.o sha256hl.o sha384hl.o sha512hl.o \
	md5.o rmd160.o sha1.o sha2.o

.SUFFIXES:
.DEFAULT:

LIB= ../hash.a

all: ${HELPER} ${OBJECTS} ${LIB}
	@echo "done."

${LIB}:
	@ar cru $@ ${OBJECTS}

%.c: ${SRCDIR}/lib/libc/hash/%.c
	/bin/cp -u $< $@

%.o: %.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

clean:
	rm -f ${HELPER} *.o *.dll *.a

md5hl.c:	${SRCDIR}/lib/libc/hash/helper.c
	sed -e 's/hashinc/md5.h/g' -e 's/HASH/MD5/g' $< > $@

rmd160hl.c:	${SRCDIR}/lib/libc/hash/helper.c
	sed -e 's/hashinc/rmd160.h/g' -e 's/HASH/RMD160/g' $< > $@

sha1hl.c:	${SRCDIR}/lib/libc/hash/helper.c
	sed -e 's/hashinc/sha1.h/g' -e 's/HASH/SHA1/g' $< > $@

sha224hl.c:	${SRCDIR}/lib/libc/hash/helper.c
	sed -e 's/hashinc/sha2.h/g' \
	    -e 's/HASH/SHA224/g' \
	    -e 's/SHA[0-9][0-9][0-9]_CTX/SHA2_CTX/g' $< > $@

sha256hl.c:	${SRCDIR}/lib/libc/hash/helper.c
	sed -e 's/hashinc/sha2.h/g' \
	    -e 's/HASH/SHA256/g' \
	    -e 's/SHA[0-9][0-9][0-9]_CTX/SHA2_CTX/g' $< > $@

sha384hl.c:	${SRCDIR}/lib/libc/hash/helper.c
	sed -e 's/hashinc/sha2.h/g' \
	    -e 's/HASH/SHA384/g' \
	    -e 's/SHA[0-9][0-9][0-9]_CTX/SHA2_CTX/g' $< > $@

sha512hl.c:	${SRCDIR}/lib/libc/hash/helper.c
	sed -e 's/hashinc/sha2.h/g' \
	    -e 's/HASH/SHA512/g' \
	    -e 's/SHA[0-9][0-9][0-9]_CTX/SHA2_CTX/g' $< > $@
