@ECHO OFF

miniroot\sh -c /sbin/setup.sh
regedit /S .\etc\console.reg
regedit /S .\etc\color\%~1.reg
