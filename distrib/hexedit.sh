if ! cd "$PKGROOT/hexedit"; then
        exit 1
fi

echo -n "Compressing hexedit12.tgz... "
tar -zcf $DISTROOT/hexedit12.tgz *
echo done.

