if ! cd "$PKGROOT/minc-base"; then
        exit 1
fi

copy_file /etc "$ETC"
copy_file /sbin "$SBIN"

echo -n "Compressing base61.tgz... "
tar -zcf $DISTROOT/base61.tgz *
echo done.
