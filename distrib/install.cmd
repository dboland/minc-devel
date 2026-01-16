@ECHO OFF

SET ROOT=%cd%
SET Path=%ROOT%\miniroot

gzip -dc %~1 | tar -C / -pxf -

DEL %~1
