@echo off
setlocal

call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1

REM Paths are resolved relative to this script's own folder (portable — clone anywhere).
set SRC=%~dp0
if "%SRC:~-1%"=="\" set SRC=%SRC:~0,-1%
set BUILD=%SRC%\build\release
set CMAKE=C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe

if exist "%BUILD%" rmdir /s /q "%BUILD%"

"%CMAKE%" -B "%BUILD%" -S "%SRC%" -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_MAKE_PROGRAM=ninja.exe -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static -DCMAKE_PREFIX_PATH=C:/vcpkg/installed/x64-windows-static -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF

if %ERRORLEVEL% neq 0 ( echo CONFIGURE FAILED & exit /b 1 )

"%CMAKE%" --build "%BUILD%"

if %ERRORLEVEL% neq 0 ( echo BUILD FAILED & exit /b 1 )

echo BUILD SUCCEEDED
