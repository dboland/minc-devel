if ! cd "$PKGROOT/bzip2"; then
        exit 1
fi

echo -n "Compressing bzip210.tgz... "
tar -zcf $DISTROOT/bzip210.tgz *
echo done.

