if ! cd "$PKGROOT/rsync"; then
        exit 1
fi

echo -n "Compressing rsync322.tgz... "
tar -zcf $DISTROOT/rsync322.tgz *
echo done.
