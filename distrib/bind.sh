if ! cd "$PKGROOT/bind"; then
	exit 1
fi

echo -n "Compressing bind93.tgz... "
tar -zcf $DISTROOT/bind93.tgz usr/bin usr/share/man/man1
echo done.
