#!/bin/sh

DEVROOT=$(dirname $0)

. ${DEVROOT}/config.inc

if [ -z "${DESTDIR}" ]; then
	DESTDIR=${SYSTEMDRIVE}/minc-release
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

make -C openbsd install
make -C libposix install

rm -f ${DESTDIR}/dev/*
cp ./distrib/setup.cmd ${DESTDIR}/

cd ${DESTDIR}

cmd //c setup.cmd

rm ${DESTDIR}/setup.cmd
