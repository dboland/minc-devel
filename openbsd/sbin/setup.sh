#!/bin/sh

echo ' ---------------------------------------------------'
echo '| MinC System Configuration'
echo ' ---------------------------------------------------'

PATH='/bin:/sbin:/usr/bin:/usr/sbin'
USER="$USERNAME"
HOME="/home/$USERNAME"
WINHOME="$USERPROFILE"

function admins_only
{
	for file in $1; do
		chown "$USER:0" $file
		chmod g+w $file
	done
}

echo -n "Creating system directories... "
mkdir -p /mnt /dev /root /home /tmp /var/run /var/mail
chown "${USER}" /mnt /tmp /home
chmod 0775 /root /tmp
chgrp 0 /root
mkent group >/etc/group
mkent passwd >/tmp/passwd
pwd_mkdb -p /tmp/passwd
echo done.

echo -n "Creating /dev file system... "
sh /etc/MAKEDEV
mkent -p fstab >/etc/fstab
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
if ! [ -f "$HOME/.profile" ]; then
	echo "mkent resolv 2>/dev/null >/etc/resolv.conf" >"$HOME/.profile"
fi
touch "/var/mail/$USER"
echo done.
