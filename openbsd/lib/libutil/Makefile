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

HEADERS=util.h imsg.h ohash.h
CFLAGS+= -I${SRCDIR}/sys

all: ${HEADERS} libutil.a

%.o: ${SRCDIR}/lib/libutil/%.c
	@/bin/echo -n "$@ "
	@gcc -c ${CFLAGS} -o $@ $<

%.h: ${SRCDIR}/lib/libutil/%.h
	@/bin/cp -vu $< ${INCDIR}/

pidfile.o: pidfile.c
	gcc -c ${CFLAGS} -o $@ $<

libutil.a: ${OBJECTS}
	@/bin/echo "done."
	@/bin/echo "Making static library $@"
	@ar cru $@ ${OBJECTS}

clean:
	/bin/rm -f *.o *.a ${LIBUTIL}

install-local:
	/bin/cp -vu libutil.a ${LIBDIR}/
