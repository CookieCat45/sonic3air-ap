@echo on

call ../get_msbuild_path.bat


:: Build ixwebsocket

@echo.
@echo.
@echo === Building ixwebsocket ===

if not exist .\ixwebsocket\build  mkdir .\ixwebsocket\build
cmake build

pushd ixwebsocket\build
::%msbuildPath% ixwebsocket.sln /target:ixwebsocket /property:Configuration=Debug   /property:Platform=x86 -verbosity:minimal
::%msbuildPath% ixwebsocket.sln /target:ixwebsocket /property:Configuration=Release /property:Platform=x86 -verbosity:minimal
%msbuildPath% ixwebsocket.sln /target:ixwebsocket /property:Configuration=Debug   /property:Platform=x64 -verbosity:minimal
%msbuildPath% ixwebsocket.sln /target:ixwebsocket /property:Configuration=Release /property:Platform=x64 -verbosity:minimal
popd

call copy_ixwebsocket.bat no_pause


:: Done
echo.
if "%1"=="" pause
