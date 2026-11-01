if ! cd "$PKGROOT/lynx"; then
	exit 1
fi

echo -n "Compressing lynx28.tgz... "
tar -zcf $DISTROOT/lynx28.tgz *
echo done.

