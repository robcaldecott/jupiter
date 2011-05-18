@echo off
set CURDIR=%CD%
md ..\Jupiter-build-desktop
cd ..\Jupiter-build-desktop
qmake -r %CURDIR%\Jupiter.pro -spec win32-g++ CONFIG+=release
mingw32-make -w release
iscc %CURDIR%\Reporter\ReporterViewer\ReporterViewer.iss /O"."
iscc %CURDIR%\Reporter\ReporterApp\ReporterApp.iss /O"."
cd %CURDIR%
