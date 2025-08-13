if ! cd "$PKGROOT/eMail"; then
        exit 1
fi

echo -n "Compressing email32.tgz... "
tar -zcf $DISTROOT/email32.tgz *
echo done.

