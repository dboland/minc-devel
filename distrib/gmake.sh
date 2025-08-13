if ! cd "$PKGROOT/make"; then
        exit 1
fi

echo -n "Compressing gmake42.tgz... "
tar -zcf $DISTROOT/gmake42.tgz *
echo done.

