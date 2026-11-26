#!/bin/sh

echo ' ---------------------------------------------------'
echo '| MinC System Configuration'
echo ' ---------------------------------------------------'

PATH='/bin:/sbin:/usr/bin:/usr/sbin'
USER="$USERNAME"
HOME="/home/$USERNAME"
WINHOME="$USERPROFILE"

admins_only()
{
	for file in $1; do
		chown "$USER:0" $file
		chmod g+w $file
	done
}
install_conf()
{
	for file in `ls /$1/*.dist | sed 's/\.dist//g'`; do
		if ! [ -f $file ]; then
			echo "$file.dist -> $file"
			cp -p "$file.dist" "$file"
		fi
	done
}

echo -n "Creating system directories... "
mkdir -p /mnt /dev /root /home /tmp
mkdir -p /var/run /var/mail /var/empty /var/log /var/tmp
mkdir -p /usr/share/dict
chown "${USER}" /mnt /home /tmp /var/tmp
chmod 0775 /root /tmp
chgrp 0 /root /var/log
admins_only "/var/mail"
mkent group >/etc/group
mkent passwd >/tmp/passwd
pwd_mkdb -p /tmp/passwd
echo done.

echo -n "Creating /dev file system... "
sh /etc/MAKEDEV
install_conf "etc"
admins_only "/etc/fstab"
echo done.

echo -n "Configuring network... "
ln -sf "$SystemRoot\System32\drivers\etc\hosts" /etc/hosts
ln -sf "$SystemRoot\System32\drivers\etc\hosts.ics" /etc/hosts.ics
ln -sf "$SystemRoot\System32\drivers\etc\services" /etc/services
ln -sf "$SystemRoot\System32\drivers\etc\protocol" /etc/protocols
ln -sf "$SystemRoot\System32\drivers\etc\networks" /etc/networks
mkent resolv >/etc/resolv.conf
admins_only "/etc/resolv.conf"
echo done.

echo -n "Creating home directory for '$USER'... "
mkdir -p "$HOME"
ln -sn "$WINHOME\Documents" "$HOME/Documents" 2>/dev/null
ln -sn "$WINHOME\Downloads" "$HOME/Downloads" 2>/dev/null
ln -sn "$WINHOME\Desktop" "$HOME/Desktop" 2>/dev/null
ln -sn "$WINHOME\Videos" "$HOME/Videos" 2>/dev/null
ln -sn "$WINHOME\Pictures" "$HOME/Pictures" 2>/dev/null
ln -sn "$APPDATA" "$HOME/AppData" 2>/dev/null
if ! [ -f "$HOME/.profile" ]; then
	cp /etc/skel/.profile "$HOME/"
fi
touch "/var/mail/$USER"
echo done.
