@echo off
rem ModernFpsLimit - Copyright (c) 2026 Softik Inquisitor (https://vk.ru/miurawr). All rights reserved.
rem Copying, redistribution or reuse of this file without the author's permission is prohibited.
where cl >nul 2>&1 && goto :vs_ready
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSDIR=%%i"
if not defined VSDIR (echo Visual Studio with C++ not found & exit /b 1)
call "%VSDIR%\VC\Auxiliary\Build\vcvars32.bat" >nul
:vs_ready
cd /d "%~dp0"
if not exist build\obj mkdir build\obj
cl /nologo /c /O2 /MT /EHsc /W4 /std:c++20 /utf-8 /DNDEBUG /Fobuild\obj\ src\*.cpp src\core\*.cpp src\limiter\*.cpp src\game\*.cpp src\samp\*.cpp || exit /b 1
link /nologo /DLL /OUT:build\ModernFpsLimit.asi build\obj\*.obj user32.lib kernel32.lib winmm.lib || exit /b 1
echo OK: build\ModernFpsLimit.asi
