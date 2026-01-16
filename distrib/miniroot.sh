MINIROOT=${DISTROOT}/miniroot

if ! [ -d "${MINIROOT}" ]; then
        echo "${MINIROOT}: No such directory"
        exit 1
fi

echo -n "Creating miniroot... "
cp /bin/tar "${MINIROOT}/tar.exe"
cp /bin/chmod "${MINIROOT}/chmod.exe"
cp /bin/sh "${MINIROOT}/sh.exe"
cp /bsd.dll "${MINIROOT}/"
if ! cp ${BINDIR}/gzip.exe "${MINIROOT}/"; then
	exit 1
fi
echo "done."
