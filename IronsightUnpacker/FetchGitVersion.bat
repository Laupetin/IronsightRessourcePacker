@echo off
for /f %%i in ('git rev-list --count master') do set gitversion=%%i
echo #define GIT_VERSION_I %gitversion% > GitVersion.h
echo #define GIT_VERSION "r%gitversion%" >> GitVersion.h

for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "year=%dt:~0,4%"

echo #define BUILD_YEAR "%year%" >> GitVersion.h
)
EXIT /b 0