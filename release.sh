#!/bin/sh

DEVROOT=$(pwd)
DISTROOT=${DEVROOT}/distrib

if ! [ -d "${DISTROOT}" ]; then
	echo "${DISTROOT}: No such directory"
	exit 1
fi

. ${DEVROOT}/config.inc

SBIN='libposix-6.1.0.dll bsd.exe mkent wrc setup.sh wtrace iptables color terminal.cmd mount_refs'
ETC='man.conf login.conf magic profile MAKEDEV console.reg syslog.conf ttys vi.exrc'

copy_file()
{
	for file in $2; do
		cp -p "$1/$file" ".$1/$file"
	done
}

if ! [ -d "${PKGROOT}" ]; then
	echo "Package root (${PKGROOT}) not found. Make sure "
	echo "the 'PKGROOT' variable is defined correctly in config.inc"
	exit 1
fi

. $DISTROOT/miniroot.sh
. $DISTROOT/base.sh
. $DISTROOT/comp.sh
#. $DISTROOT/test.sh

#. $DISTROOT/vista.sh
. $DISTROOT/vim.sh
. $DISTROOT/nano.sh
. $DISTROOT/lynx.sh
. $DISTROOT/wget.sh
. $DISTROOT/curl.sh
. $DISTROOT/unzip.sh
. $DISTROOT/bzip2.sh
. $DISTROOT/email.sh
. $DISTROOT/hexedit.sh
. $DISTROOT/xz.sh
. $DISTROOT/sasl.sh
. $DISTROOT/mutt.sh
. $DISTROOT/bitchx.sh
. $DISTROOT/git.sh
. $DISTROOT/magick.sh
#. $DISTROOT/rsync.sh
. $DISTROOT/binutils.sh
. $DISTROOT/gcc.sh
. $DISTROOT/gmake.sh
#. $DISTROOT/perl.sh
. $DISTROOT/bind.sh

if ! cd ${DISTROOT}; then
	exit 1
fi

wrc makensis /DVERSION=$VERSION minc.nsi
wrc makensis /DVERSION=$VERSION buildtools.nsi
#wrc makensis /DVERSION=$VERSION test.nsi
#git archive --format=zip --prefix=minc-devel/ -o release/source/minc-devel.zip HEAD
