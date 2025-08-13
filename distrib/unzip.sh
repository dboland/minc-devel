if ! cd "$PKGROOT/unzip"; then
        exit 1
fi

echo -n "Compressing unzip552.tgz... "
tar -zcf $DISTROOT/unzip552.tgz *
echo done.

