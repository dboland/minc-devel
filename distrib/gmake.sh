if ! cd "$PKGROOT/make-release"; then
        exit 1
fi

echo -n "Inpakken gmake42.tgz... "
tar -zcf $DISTROOT/gmake42.tgz *
echo gereed.

