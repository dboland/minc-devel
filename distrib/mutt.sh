if ! cd "$PKGROOT/mutt"; then
        exit 1
fi

cp /etc/Muttrc ./etc/Muttrc

echo -n "Compressing mutt20.tgz... "
tar -zcf $DISTROOT/mutt20.tgz *
echo done.

