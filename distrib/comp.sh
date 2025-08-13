if ! cd "$PKGROOT/minc-comp"; then
        exit 1
fi

cp -p /usr/lib/crt0.o ./usr/lib/
cp -p /usr/lib/crt1.o ./usr/lib/
cp -p /usr/lib/gcrt0.o ./usr/lib/

cp -p /usr/lib/libc.a ./usr/lib/
cp -p /usr/lib/libsocket.a ./usr/lib/

# don't overwrite base libs
rm -f ./usr/lib/libcrypto.so.*
rm -f ./usr/lib/libssl.so.*
rm -f ./usr/lib/libutil.so.*
rm -f ./usr/lib/libedit.so.*

echo -n "Compressing comp61.tgz... "
tar -zcf $DISTROOT/comp61.tgz *
echo done.
