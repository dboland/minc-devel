if ! cd "$PKGROOT/minc-test"; then
        exit 1
fi

copy_file /sbin "$SBIN"
copy_file /etc "$ETC"

echo -n "Compressing test61.tgz... "
tar -zcf $DISTROOT/test61.tgz *
echo done.
