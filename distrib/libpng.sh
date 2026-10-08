if ! cd "$PKGROOT/libpng"; then
        exit 1
fi

echo -n "Compressing libpng16.tgz... "
tar -zcf $DISTROOT/libpng16.tgz *
echo done.
