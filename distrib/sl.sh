if ! cd "$PKGROOT/sl"; then
	exit 1
fi

echo -n "Compressing sl.tgz... "
tar -zcf $DISTROOT/sl.tgz *
echo done.

