if ! cd "$PKGROOT/git"; then
        exit 1
fi

mkdir -p etc
cp -p /etc/gitconfig ./etc/

mkdir -p ./usr/share/man
echo -n "Uncompressing man pages... "
xz --decompress --stdout ../git-manpages-2.31.1.tar.xz | tar -C ./usr/share/man -xf -
echo done.

echo -n "Compressing git231.tgz... "
tar -zcf $DISTROOT/git231.tgz *
echo done.
