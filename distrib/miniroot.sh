MINIROOT=${DISTROOT}/miniroot

if ! [ -d "${MINIROOT}" ]; then
        echo "${MINIROOT}: No such directory"
        exit 1
fi

echo -n "Creating miniroot... "
cp -p /bsd.dll "$DISTROOT/"
cp -p /bin/tar "${MINIROOT}/tar.exe"
cp -p /bin/chmod "${MINIROOT}/chmod.exe"
cp -p /bin/sh "${MINIROOT}/sh.exe"
if ! cp ${BINDIR}/gzip.exe "${MINIROOT}/"; then
	exit 1
fi
echo "done."
