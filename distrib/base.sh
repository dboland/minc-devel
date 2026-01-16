if ! cd "$PKGROOT/minc-base"; then
        exit 1
fi

copy_src()
{
        for file in $2; do
                cp "${DEVROOT}/openbsd/$1/$file" "./$1/$file"
        done
}

cp -p /bsd.dll /minc.exe .

copy_src etc "$ETC"

copy_file /sbin "$SBIN"
copy_file /usr/bin "$UBIN"

echo -n "Compressing base61.tgz... "
tar -zcf $DISTROOT/base61.tgz *
echo done.
