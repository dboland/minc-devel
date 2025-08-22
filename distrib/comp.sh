if ! cd "$PKGROOT/minc-comp"; then
        exit 1
fi

cp -p /usr/lib/crt0.o ./usr/lib/
cp -p /usr/lib/crt1.o ./usr/lib/
cp -p /usr/lib/gcrt0.o ./usr/lib/

cp -p /usr/lib/libc.a ./usr/lib/
cp -p /usr/lib/libsocket.a ./usr/lib/
cp -p /usr/lib/libdl.a ./usr/lib/

ln -sf libc.so.75.0 ./usr/lib/libc.so
ln -sf libcrypto.so.27.0 ./usr/lib/libcrypto.so
ln -sf libedit.so.5.1 ./usr/lib/libedit.so
ln -sf libkvm.so.16.0 ./usr/lib/libkvm.so
ln -sf libsocket.so.75.0 ./usr/lib/libsocket.so
ln -sf libsqlite3.so.27.0 ./usr/lib/libsqlite3.so
ln -sf libssl.so.24.1 ./usr/lib/libssl.so
ln -sf libutil.so.12.1 ./usr/lib/libutil.so

echo -n "Compressing comp61.tgz... "
tar -zcf $DISTROOT/comp61.tgz *
echo done.
