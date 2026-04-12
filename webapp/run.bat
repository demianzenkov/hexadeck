@echo off
cd /d "%~dp0"

py --version >nul 2>&1 && set PY=py&& goto start
python --version >nul 2>&1 && set PY=python&& goto start
python3 --version >nul 2>&1 && set PY=python3&& goto start

echo Python not found. Please install Python 3 and ensure it is in your PATH.
echo You may also need to disable the Windows Store alias for Python:
echo   Settings ^> Apps ^> Advanced app settings ^> App execution aliases
 q	pause
exit /b 1

:start
echo Starting server at http://localhost:8000
start "" "http://localhost:8000"
%PY% -m http.server 8000
