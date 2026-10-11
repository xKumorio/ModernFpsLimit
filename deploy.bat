@echo off
chcp 65001 >nul
cd /d "%~dp0"
git remote get-url origin >nul 2>&1 && goto :have_remote
set "URL="
set /p "URL=Ссылка на репозиторий GitHub (https://github.com/ник/ModernFpsLimit.git): "
if not defined URL goto :cancel
git remote add origin "%URL%" || goto :fail
:have_remote
git add -A
git status --short
set "MSG="
set /p "MSG=Имя коммита: "
if not defined MSG goto :cancel
git commit -q -m "%MSG%"
if errorlevel 1 echo Нечего коммитить - отправляю то, что уже есть.
git push -u origin main || goto :fail
echo.
echo Готово. GitHub сам соберёт ModernFpsLimit.asi: вкладка Actions, готовый файл - в Releases.
pause
exit /b 0
:cancel
echo Отменено.
pause
exit /b 1
:fail
echo Не получилось - смотри ошибку выше.
pause
exit /b 1
