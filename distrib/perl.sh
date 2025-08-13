if ! cd "$PKGROOT/perl"; then
        exit 1
fi

MODS='B Compress Encode POSIX PerlIO Storable Unicode re'

function rm_modules {
	for mod in $1; do
		echo -n "${mod} "
		rm -rf ./usr/lib/perl5/5.30.0/OpenBSD.i386-openbsd-multi/auto/${mod}
	done
}

cp ./usr/lib/perl5/5.30.0/OpenBSD.i386-openbsd-multi/CORE/libperl.so ./usr/lib/

echo -n "Verwijderen modules... "
rm_modules "${MODS}"
rm -rf ./usr/lib/perl5/5.30.0/Pod
echo gereed.

echo -n "Inpakken perl53.tgz... "
tar -zcf $DISTROOT/perl53.tgz *
echo gereed.

