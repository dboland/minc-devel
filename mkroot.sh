#!/bin/sh

DESTDIR="$1"

if [ -z "${DESTDIR}" ]; then
	echo "Usage: $0 DESTDIR"
	exit 1
fi

echo " ---------------------------------------------------"
echo -n "| Installing in ${DESTDIR} for "

if id -u 'NT SERVICE\TrustedInstaller' >/dev/null 2>&1; then
	echo "Windows Vista"
	/bin/mkdir -p ${DESTDIR}
else
	echo "Windows NT"
	mkdir -p ${DESTDIR}
fi
echo " ---------------------------------------------------"
