if ! cd "$PKGROOT/minc-comp"; then
        exit 1
fi

ln -sf libc.so.75.0 ./usr/lib/libc.so
ln -sf libsocket.so.75.0 ./usr/lib/libsocket.so
ln -sf libcrypto.so.27.0 ./usr/lib/libcrypto.so
ln -sf libssl.so.24.1 ./usr/lib/libssl.so
ln -sf libsqlite3.so.27.0 ./usr/lib/libsqlite3.so
ln -sf libedit.so.5.1 ./usr/lib/libedit.so
ln -sf libutil.so.12.1 ./usr/lib/libutil.so
ln -sf libcurses.so.14.0 ./usr/lib/libcurses.so
ln -sf libncurses.so.14.0 ./usr/lib/libncurses.so
ln -sf libncursesw.so.14.0 ./usr/lib/libncursesw.so
ln -sf libtermcap.so.14.0 ./usr/lib/libtermcap.so
ln -sf libtermlib.so.14.0 ./usr/lib/libtermlib.so
ln -sf libkvm.so.16.0 ./usr/lib/libkvm.so

echo -n "Compressing comp61.tgz... "
tar -zcf $DISTROOT/comp61.tgz *
echo done.
