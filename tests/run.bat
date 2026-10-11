@echo off
rem ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
rem Copying, redistribution or reuse of this file without the author's permission is prohibited.
where cl >nul 2>&1 && goto :vs_ready
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSDIR=%%i"
if not defined VSDIR (echo Visual Studio with C++ not found & exit /b 1)
call "%VSDIR%\VC\Auxiliary\Build\vcvars32.bat" >nul
:vs_ready
cd /d "%~dp0.."
if not exist build\test mkdir build\test
cl /nologo /O2 /MT /EHsc /W4 /std:c++20 /utf-8 /Fobuild\test\ /Febuild\test\limiter_test.exe tests\limiter_test.cpp src\limiter\*.cpp kernel32.lib winmm.lib || exit /b 1
build\test\limiter_test.exe
