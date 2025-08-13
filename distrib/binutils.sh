if ! cd "$PKGROOT/binutils"; then
        exit 1
fi

echo -n "Compressing binutils228.tgz... "
tar -zcf $DISTROOT/binutils228.tgz *
echo done.

