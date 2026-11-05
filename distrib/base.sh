if ! cd "$PKGROOT/minc-base"; then
        exit 1
fi

copy_dev etc "$ETC"
copy_dev sbin "$SBIN"

cp -p /minc.exe .
ln -s /usr/share/zoneinfo/Europe/Amsterdam ./etc/localtime
ln -s opencvs ./usr/bin/cvs

ln ./bin/ksh ./bin/sh

echo -n "Compressing base61.tgz... "
tar -zcf $DISTROOT/base61.tgz *
echo done.
