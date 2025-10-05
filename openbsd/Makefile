ROOTDIR= ..

include ${ROOTDIR}/Makefile.inc

all: ${SRCDIR} ${TMPDIR}
	@${MAKE} -C lib
	@${MAKE} -C bin all install-local
	@${MAKE} -C usr.bin all install-local
	@${MAKE} -C sbin all install-local
	@${MAKE} -C share all install-local
	@${MAKE} -C libexec all install-local
	@${MAKE} -C usr.sbin
	@${MAKE} -C etc

include ${ROOTDIR}/Errorfile.inc

build-complete: ${MKDIR} ${INSTALL} ${ID} ${CP} ${LN}

install-local: ${TMPDIR}
	@${MAKE} -C include all install-local
	@${MAKE} -C sys all install-local

install: ${DESTDIR}
	@${MAKE} -C lib install
	@${MAKE} -C bin install
	@${MAKE} -C sbin install
	@${MAKE} -C usr.bin install
	@${MAKE} -C usr.sbin install
	@${MAKE} -C share install
	@${MAKE} -C libexec install
	@${MAKE} -C etc install

release:
	@${MAKE} -C lib release
	@${MAKE} -C sbin release
	@${MAKE} -C libexec release
	
bin-clean:
	@${MAKE} -C bin clean
	@${MAKE} -C usr.bin clean

clean: bin-clean
	@${MAKE} -C lib clean
	@${MAKE} -C sbin clean
	@${MAKE} -C usr.sbin clean
	@${MAKE} -C libexec clean
