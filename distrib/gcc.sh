if ! cd "$PKGROOT/gcc"; then
        exit 1
fi

ln -sf gcc ./usr/bin/cc

echo -n "Compressing gcc481.tgz... "
tar -zcf $DISTROOT/gcc481.tgz *
echo done.
