if ! cd "$PKGROOT/curl"; then
        exit 1
fi

ln -sf libcurl.so.10.0 ./usr/lib/libcurl.so

echo -n "Compressing curl772.tgz... "
tar -zcf $DISTROOT/curl772.tgz *
echo done.
