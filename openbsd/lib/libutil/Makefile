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
OBJECTS+= passwd.o check_expire.o login_fbtab.o imsg.o imsg-buffer.o login.o

HEADERS= ${INCDIR}/util.h ${INCDIR}/imsg.h ${INCDIR}/ohash.h
CFLAGS+= -DNOPIE
ULIBDIR= /usr/lib
LIBSRC= ${SRCDIR}/lib/libutil
IMPLIB= ${LIBSRC}/libutil.dll.a

all: ${HEADERS} libutil.a

%.o: ${LIBSRC}/%.c
	@echo -n "$@ "
	@${XGCC} -c ${CFLAGS} -o $@ $<

${INCDIR}/%.h: ${LIBSRC}/%.h
	@/bin/cp -vu $< $@

libutil.a: ${OBJECTS}
	@echo "done."
	@echo "Making static library $@"
	@ar cru $@ ${OBJECTS}

install-local:
	/bin/cp -vu libutil.a ${LIBDIR}/

install-cross: ${IMPLIB}
	/bin/cp -uv ${IMPLIB} ${XLIBDIR}/libutil.a

install:
	${INSTALL} -m ${BINMODE} libutil.a ${DESTDIR}${ULIBDIR}/

lib-clean:
	/bin/rm -f libutil.a

all-clean: lib-clean
	/bin/rm -f *.o

clean: lib-clean
