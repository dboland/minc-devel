if ! cd "$PKGROOT/xz"; then
        exit 1
fi

echo -n "Compressing xz52.tgz... "
tar -zcf $DISTROOT/xz52.tgz usr/bin usr/share/man
echo done.

