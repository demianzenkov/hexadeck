@echo off
cd /d "%~dp0"

where python3 >nul 2>&1
if %errorlevel%==0 (
    set PY=python3
    goto start
)

where python >nul 2>&1
if %errorlevel%==0 (
    set PY=python
    goto start
)

echo Python not found. Please install Python 3.
pause
exit /b 1

:start
echo Starting server at http://localhost:8000
start "" "http://localhost:8000"
%PY% -m http.server 8000
