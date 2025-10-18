#!/bin/sh
# Hello Rob Landley #

. config.inc

if [ -z "${DESTDIR}" ]; then
	DESTDIR=${SYSTEMDRIVE}/minc-release
fi

if ! /bin/make -C openbsd build-complete; then
	exit 1
fi

echo " ---------------------------------------------------"
echo -n "| Installing in ${DESTDIR} for "

if ${ID} -u 'NT SERVICE\TrustedInstaller' >/dev/null 2>&1; then
	echo "Windows Vista"
	/bin/mkdir -p ${DESTDIR}
else
	echo "Windows NT"
	${MKDIR} -p ${DESTDIR}
fi
echo " ---------------------------------------------------"

/bin/rm -f ${DESTDIR}/dev/*

if ! /bin/make install; then
	exit 1
fi

PATH=${DESTDIR}/sbin

cd ${DESTDIR}

./bin/sh -c ./sbin/setup.sh
