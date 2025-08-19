ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

HEADERS= ${XINCDIR}/fcntl.h ${XINCDIR}/stdarg.h ${XINCDIR}/stdint.h \
	${XINCDIR}/syslog.h ${XINCDIR}/termios.h
MACHINEDIR= ${XINCDIR}/machine

all: ${SRCDIR}
	@${MAKE} -C lib
	@${MAKE} -C bin all install-local
	@${MAKE} -C usr.bin all install-local
	@${MAKE} -C sbin all install-local
	@${MAKE} -C usr.sbin
	@${MAKE} -C etc

include ${ROOTDIR}/Errorfile.inc

${MACHINEDIR}:
	@/bin/mkdir -p ${MACHINEDIR}
	/bin/cp -ru ${SRCDIR}/include/* ${XINCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/sys ${XINCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/arch/${ARCH}/include/* ${MACHINEDIR}/
	/bin/patch -p 0 -d ${XPREFIX} <include.patch
	/bin/cp -ru ${SRCDIR}/sys/net ${XINCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/netinet ${XINCDIR}/
	/bin/cp -ru ${SRCDIR}/sys/netinet6 ${XINCDIR}/

${XINCDIR}/fcntl.h:
	@/bin/cp -vu fcntl.h ${XINCDIR}/

${XINCDIR}/stdarg.h:
	@/bin/cp -vu stdarg.h ${XINCDIR}/

${XINCDIR}/stdint.h:
	@/bin/cp -vu stdint.h ${XINCDIR}/

${XINCDIR}/syslog.h:
	@/bin/cp -vu syslog.h ${XINCDIR}/

${XINCDIR}/termios.h:
	@/bin/cp -vu termios.h ${XINCDIR}/

${_INCDIR}:
	@/bin/mkdir -p ${_INCDIR}
	/bin/cp -ru ${XINCDIR}/* ${_INCDIR}/

install-local: ${SRCDIR} ${MACHINEDIR} ${HEADERS}

install: ${INSTALL}
	@${MAKE} -C bin install
	@${MAKE} -C sbin install
	@${MAKE} -C lib install
	@${MAKE} -C usr.bin install
	@${MAKE} -C usr.sbin install
	@${MAKE} -C share install
	@${MAKE} -C etc install

install-cross: ${_INCDIR}
	@#${MAKE} -C share install
	@${MAKE} -C lib install-cross

release:
	@${MAKE} -C lib release
	@${MAKE} -C sbin release
	
bin-clean:
	@${MAKE} -C bin clean
	@${MAKE} -C usr.bin clean

clean: bin-clean
	@${MAKE} -C lib clean
	@${MAKE} -C sbin clean
	@${MAKE} -C usr.sbin clean
