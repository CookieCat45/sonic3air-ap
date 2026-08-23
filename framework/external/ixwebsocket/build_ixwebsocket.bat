@echo on

call ../get_msbuild_path.bat


:: Build ixwebsocket

@echo.
@echo.
@echo === Building ixwebsocket ===

if not exist .\ixwebsocket\build  mkdir .\ixwebsocket\build
cmake -DCMAKE_GENERATOR_PLATFORM=x64 -S .\ixwebsocket -B .\ixwebsocket\build

pushd ixwebsocket\build
%msbuildPath% ixwebsocket.sln /target:ixwebsocket /property:Configuration=Debug   /property:Platform=x64 -verbosity:minimal
%msbuildPath% ixwebsocket.sln /target:ixwebsocket /property:Configuration=Release /property:Platform=x64 -verbosity:minimal
popd

if not exist .\ixwebsocket\buildWin32  mkdir .\ixwebsocket\buildWin32
cmake -DCMAKE_GENERATOR_PLATFORM=Win32 -S .\ixwebsocket -B .\ixwebsocket\buildWin32

pushd ixwebsocket\buildWin32
%msbuildPath% ixwebsocket.sln /target:ixwebsocket /property:Configuration=Debug   /property:Platform=Win32 -verbosity:minimal
%msbuildPath% ixwebsocket.sln /target:ixwebsocket /property:Configuration=Release /property:Platform=Win32 -verbosity:minimal
popd

call copy_ixwebsocket.bat no_pause


:: Done
echo.
if "%1"=="" pause
