@echo off

set InputDir=.
set OutputDir=..\..

echo.
echo Creating output directories...
if not exist %OutputDir%\include\ixwebsocket          mkdir %OutputDir%\include\ixwebsocket
if not exist %OutputDir%\lib\x86\ixwebsocket          mkdir %OutputDir%\lib\x86\ixwebsocket
if not exist %OutputDir%\lib\x86d\ixwebsocket         mkdir %OutputDir%\lib\x86d\ixwebsocket
if not exist %OutputDir%\lib\x64\ixwebsocket          mkdir %OutputDir%\lib\x64\ixwebsocket
if not exist %OutputDir%\lib\x64d\ixwebsocket         mkdir %OutputDir%\lib\x64d\ixwebsocket

echo.
echo Copying includes...
copy %InputDir%\ixwebsocket\ixwebsocket\*.h %OutputDir%\include\ixwebsocket

echo.
echo Copying LIBs...
copy %InputDir%\ixwebsocket\buildWin32\Debug\ixwebsocket.lib %OutputDir%\lib\x86d\ixwebsocket
copy %InputDir%\ixwebsocket\buildWin32\mbedtls_bin\library\Debug\*.lib %OutputDir%\lib\x86d\ixwebsocket
copy %InputDir%\ixwebsocket\buildWin32\Release\ixwebsocket.lib %OutputDir%\lib\x86\ixwebsocket
copy %InputDir%\ixwebsocket\buildWin32\mbedtls_bin\library\Release\*.lib %OutputDir%\lib\x86\ixwebsocket
copy %InputDir%\ixwebsocket\build\Debug\ixwebsocket.lib %OutputDir%\lib\x64d\ixwebsocket
copy %InputDir%\ixwebsocket\build\mbedtls_bin\library\Debug\*.lib %OutputDir%\lib\x64d\ixwebsocket
copy %InputDir%\ixwebsocket\build\Release\ixwebsocket.lib %OutputDir%\lib\x64\ixwebsocket
copy %InputDir%\ixwebsocket\build\mbedtls_bin\library\Release\*.lib %OutputDir%\lib\x64\ixwebsocket

:: Done
echo.
if "%1"=="" pause
