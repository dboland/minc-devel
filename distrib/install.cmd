@ECHO OFF

SET ROOT=%cd%
:: SET Path=%ROOT%;%Path%

miniroot\gzip -dc %~1 | miniroot\tar -C / -pxf -

DEL %~1
