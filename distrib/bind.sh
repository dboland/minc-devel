if ! cd "$PKGROOT/bind-release"; then
	exit 1
fi

echo -n "Inpakken bind93.tgz... "
tar -zcf $DISTROOT/bind93.tgz usr/bin usr/share/man/man1
echo gereed.
