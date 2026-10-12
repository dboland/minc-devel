@ECHO OFF

miniroot\gzip -dc %~1 | miniroot\tar -C / -pxf -

DEL %~1
