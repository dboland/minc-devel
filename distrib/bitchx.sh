if ! cd "$PKGROOT/BitchX"; then
        exit 1
fi

bunzip2 ./usr/share/man/man1/BitchX.1.bz2 2>/dev/null

echo -n "Compressing bitchx12.tgz... "
tar -zcf $DISTROOT/bitchx12.tgz *
echo done.
