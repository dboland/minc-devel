if ! cd "$PKGROOT/bzip"; then
        exit 1
fi

echo -n "Compressing bzip210.tgz... "
tar -zcf $DISTROOT/bzip210.tgz *
echo done.

