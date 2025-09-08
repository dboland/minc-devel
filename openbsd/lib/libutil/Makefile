ROOTDIR= ../../..

include ${ROOTDIR}/Makefile.inc

OBJECTS= bcrypt_pbkdf.o duid.o \
	login_tty.o logout.o logwtmp.o pty.o \
	uucplock.o fparseln.o opendisk.o pidfile.o \
	fmt_scaled.o pkcs5_pbkdf2.o \
	getrawpartition.o readlabel.o opendev.o
OBJECTS+= ohash_create_entry.o ohash_delete.o ohash_do.o ohash_entries.o \
	ohash_enum.o ohash_init.o ohash_interval.o \
	ohash_lookup_interval.o ohash_lookup_memory.o \
	ohash_qlookup.o ohash_qlookupi.o
OBJECTS+= getmaxpartitions.o
# BSD Auth
OBJECTS+=passwd.o check_expire.o login_fbtab.o imsg.o imsg-buffer.o login.o

HEADERS= ${XINCDIR}/util.h ${XINCDIR}/imsg.h ${XINCDIR}/ohash.h
CFLAGS+= -DNOPIE -I${SRCDIR}/sys
ULIBDIR= /usr/lib

all: ${HEADERS} libutil.a

%.o: ${SRCDIR}/lib/libutil/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

${XINCDIR}/%.h: ${SRCDIR}/lib/libutil/%.h
	@/bin/cp -vu $< $@

pidfile.o: pidfile.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

libutil.a: ${OBJECTS}
	@echo "done."
	@echo "Making static library $@"
	@ar cru $@ ${OBJECTS}

clean:
	/bin/rm -f libutil.a

all-clean: clean
	/bin/rm -f *.o

install-local:
	/bin/cp -vu libutil.a ${XLIBDIR}/

install:
	/bin/cp -vu libutil.a ${DESTDIR}${ULIBDIR}/