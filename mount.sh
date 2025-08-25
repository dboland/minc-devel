#!/bin/sh

ROOTDIR=$(dirname $0)

. ${ROOTDIR}/config.inc

# Hello Earnie Boyd #

MINGWROOT=$(cmd //c echo ${MINGW})
MINCROOT=$(cmd //c echo ${PREFIX})
XMINCROOT=$(cmd //c echo ${XPREFIX})
FSTAB=/etc/fstab

if ! [ -d ${MINGWROOT} ]; then
	echo "${MINGWROOT}: No such directory"
elif [ "$1" == "mingw" ]; then
	echo "${MINGWROOT} /mingw" >$FSTAB
elif ! [ -d ${MINCROOT} ]; then
	echo "${MINCROOT}: No such directory"
elif [ "$1" == "minc" ]; then
	echo "${MINGWROOT} /mingw" >$FSTAB
	echo "${MINCROOT} /usr/local" >>$FSTAB
elif ! [ -d ${XMINCROOT} ]; then
	echo "${XMINCROOT}: No such directory"
elif [ "$1" == "cross" ]; then
	echo "${MINGWROOT} /mingw" >$FSTAB
	echo "${XMINCROOT} /usr/local" >>$FSTAB
else
	echo "Usage $0 COMPILER"
	echo
	echo "Compilers"
	printf " mingw\t\t\tmount MinGW default compiler\n"
	printf " cross\t\t\tmount poor man's cross compiler\n"
	printf " minc\t\t\tmount OpenBSD system\n"
	echo
	echo "Currently mounted:"
	/bin/cat $FSTAB
fi
