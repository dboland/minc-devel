if ! cd "$PKGROOT/cyrus-sasl"; then
        exit 1
fi

find ./usr/lib/sasl2 -name '*.a' | xargs rm -f
ln -sf libsasl2.so.3.0 ./usr/lib/libsasl2.so

echo -n "Compressing sasl2128.tgz... "
tar -zcf $DISTROOT/sasl2128.tgz *
echo done.
