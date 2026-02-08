if ! cd "$PKGROOT/perl"; then
        exit 1
fi

DOCROOT="$PKGROOT/perl-doc"

LIBDIR='usr/lib/perl5/5.30.0'
MODDIR="${LIBDIR}/OpenBSD.i386-openbsd-multi"
PODDIR="${LIBDIR}/Pod"
SHDIR='usr/share'

mv -f usr/lib/perl5/5.30.0/OpenBSD.i386-openbsd-multi/CORE/libperl.so usr/lib/

mkdir -p "${DOCROOT}/${PODDIR}"
mv ${PODDIR}/*.pod ${DOCROOT}/${PODDIR}/ 2>/dev/null

mkdir -p "${DOCROOT}/${SHDIR}"
mv "${SHDIR}/man" "${DOCROOT}/${SHDIR}/" 2>/dev/null

echo -n "Compressing perl53-base.tgz... "
tar -zcf $DISTROOT/perl53-base.tgz *
echo done.

if ! cd "${DOCROOT}"; then
	exit 1
fi
echo -n "Compressing perl53-doc.tgz... "
tar -zcf $DISTROOT/perl53-doc.tgz *
echo done.

