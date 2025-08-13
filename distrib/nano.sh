if ! cd "$PKGROOT/nano"; then
        exit 1
fi

mkdir -p ./etc
cp -p /etc/nanorc ./etc/nanorc

echo -n "Compressing nano64.tgz... "
tar -zcf $DISTROOT/nano64.tgz *
echo done.
