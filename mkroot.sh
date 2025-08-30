#!/bin/sh
# Hello Rob Landley

DEVROOT=$(dirname $0)

. ${DEVROOT}/config.inc

if [ -z "${DESTDIR}" ]; then
	DESTDIR=${SYSTEMDRIVE}/minc-release
fi

if ! /bin/make -C openbsd build-check; then
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

/bin/make -C openbsd install
/bin/make -C libposix install

PATH=${DESTDIR}/sbin

cd ${DESTDIR}

./bin/sh -c ./sbin/setup.sh
