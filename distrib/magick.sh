if ! cd "$PKGROOT/ImageMagick"; then
        exit 1
fi

echo -n "Compressing magick710.tgz... "
tar -zcf $DISTROOT/magick710.tgz etc usr/bin usr/share/man
echo done.

