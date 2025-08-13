if ! cd "$PKGROOT/lynx"; then
	exit 1
fi

mkdir -p bin
ln -sf /usr/bin/gzip ./bin/gzip

echo -n "Compressing lynx28.tgz... "
tar -zcf $DISTROOT/lynx28.tgz *
echo done.

