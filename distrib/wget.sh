if ! cd "$PKGROOT/wget"; then
        exit 1
fi

echo -n "Compressing wget118.tgz... "
tar -zcf $DISTROOT/wget118.tgz *
echo done.

