if ! cd "$PKGROOT/vim-release"; then
	exit 1
fi

rm -f ./usr/bin/ex ./usr/bin/view
find . -name ex.1 | xargs rm -f
find . -name view.1 | xargs rm -f

mkdir -p ./etc
cp -p /usr/share/vim/vimrc ./usr/share/vim/vimrc
ln -sf /usr/share/vim/vimrc ./etc/

echo -n "Compressing vim81.tgz... "
tar -zcf $DISTROOT/vim81.tgz *
echo done.
