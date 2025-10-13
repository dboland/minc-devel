ROOTDIR= ../..

include ${ROOTDIR}/Makefile.inc

OBJECTS= crt0.o crt1.o gcrt0.o crtbegin.o crtend.o
LIBS= libgcc.a libgcc_s.a libgcc_eh.a
GCCOBJ= _mulsc3.o _muldc3.o _mulxc3.o _ctzdi2.o \
	_divdi3.o _udivdi3.o _clrsbdi2.o _clrsbsi2.o \
	_chkstk.o _chkstk_ms.o
GCCDIR= /lib/gcc/mingw32/${GCCVER}
ULIBDIR= /usr/lib

all: ${MINGW}${GCCDIR} ${OBJECTS} ${LIBS}

${MINGW}${GCCDIR}:
	@echo " -----------------------------------------------"
	@echo "| GCC libexec directory ($@) not found. Make sure"
	@echo "| the 'GCCVER' variable is defined correctly in config.inc"
	@echo "| Run 'gcc -v' to find out the right version number."
	@echo " -----------------------------------------------"
	@exit 1

%.o: %.c
	${CC} -c ${CFLAGS} -I${ROOTDIR}/include -o $@ $<

%.a: %.def
	dlltool -d $< -l $@ -k

# we need alloca(), which belongs in libgcc.a
# btw: there is a chkstk() in ntdll.dll

libgcc.a: emutls.o
	ar xv ${MINGW}${GCCDIR}/libgcc.a ${GCCOBJ}
	ar cru $@ ${GCCOBJ} $^

# use -r switch to combine objects:

crt0_tls.o: crt0.o win_tls.o
	ld -r -o $@ crt0.o win_tls.o

#${XPREFIX}${GCCDIR}/include:
#	@/bin/mkdir -p $@
#	/bin/cp -ru ${MINGW}${GCCDIR}/include/c++ $@/
#	/bin/cp -ru ${MINGW}${GCCDIR}/include/ssp $@/

#${XPREFIX}${GCCDIR}: ${MINGW}${GCCDIR}
#	@/bin/mkdir -p $@
#	/bin/cp -vu ${MINGW}${GCCDIR}/libgcc.a ${XPREFIX}${GCCDIR}/
#	/bin/cp -vu ${MINGW}${GCCDIR}/libssp.a ${XPREFIX}${GCCDIR}/
#	/bin/cp -vu ${MINGW}${GCCDIR}/libstdc++.a ${XPREFIX}${GCCDIR}/

install-cross:
	@/bin/mkdir -p ${XLIBDIR}
	@/bin/cp -vu ${OBJECTS} ${XLIBDIR}/

install-local: install-cross
	@/bin/mkdir -p ${PREFIX}${GCCDIR}
	@/bin/cp -vu ${LIBS} ${PREFIX}${GCCDIR}/
	@/bin/cp -vu crtbegin.o crtend.o ${PREFIX}${GCCDIR}/
	@/bin/cp -vu crt0.o ${LIBDIR}/crt2.o
	@/bin/cp -vu crt1.o ${LIBDIR}/dllcrt2.o

install: ${DESTDIR}${ULIBDIR}
	${INSTALL} -m ${NONBINMODE} ${OBJECTS} ${DESTDIR}${ULIBDIR}/

clean:
	/bin/rm -f *.o *.a
