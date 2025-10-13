if ! cd "$PKGROOT/minc-comp"; then
        exit 1
fi

# Make base libraries static-only
rm -f ./usr/lib/libz.so.*
rm -f ./usr/lib/libutil.so.*
rm -f ./usr/lib/libedit.so.*
rm -f ./usr/lib/libkvm.so.*
rm -f ./usr/lib/libtermcap.so.*
rm -f ./usr/lib/libtermlib.so.*

ln -sf libc.so.75.0 ./usr/lib/libc.so
ln -sf libsocket.so.75.0 ./usr/lib/libsocket.so
ln -sf libcrypto.so.27.0 ./usr/lib/libcrypto.so
ln -sf libssl.so.24.1 ./usr/lib/libssl.so
ln -sf libsqlite3.so.27.0 ./usr/lib/libsqlite3.so
ln -sf libcurses.so.14.0 ./usr/lib/libcurses.so
ln -sf libncurses.so.14.0 ./usr/lib/libncurses.so
ln -sf libncursesw.so.14.0 ./usr/lib/libncursesw.so

echo -n "Compressing comp61.tgz... "
tar -zcf $DISTROOT/comp61.tgz *
echo done.
