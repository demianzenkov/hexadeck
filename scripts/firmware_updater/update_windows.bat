@echo off
REM Install dependencies if not present

REM Check for Python
where python >nul 2>nul
if errorlevel 1 (
    echo Python not found. Please install Python 3 and add it to your PATH.
    exit /b 1
)

REM Check for pip
python -m pip --version >nul 2>nul
if errorlevel 1 (
    echo pip not found. Please install pip for Python 3.
    exit /b 1
)

REM Check for pyusb
python -m pip show pyusb >nul 2>nul
if errorlevel 1 (
    echo Installing pyusb for Python 3...
    python -m pip install --user pyusb
)

REM Check for pyfu-usb
python -m pip show pyfu-usb >nul 2>nul
if errorlevel 1 (
    echo Installing pyfu-usb for Python 3...
    python -m pip install --user pyfu-usb
)

REM Parse arguments
set BIN_DIR=scripts\firmware_updater\binaries
set BIN_FILE=
set ADDRESS=0x08000000

:parse_args
setlocal enabledelayedexpansion
for %%A in (%*) do (
    set arg=%%A
    if "!arg:~0,2!"=="-D" (
        set val=!arg:~2!
        if not "!val!"=="" (
            set BIN_FILE=!val!
        ) else (
            set next=1
        )
    ) else if defined next (
        set BIN_FILE=!arg!
        set next=
    )
)
endlocal & set BIN_FILE=%BIN_FILE%

REM Default binary path if not provided
if "%BIN_FILE%"=="" set BIN_FILE=%BIN_DIR%\8dof_controller.bin

REM If only a filename is provided, prepend BIN_DIR
if not exist "%BIN_FILE%" (
    if exist "%BIN_DIR%\%BIN_FILE%" set BIN_FILE=%BIN_DIR%\%BIN_FILE%
)

REM Check if binary exists
if not exist "%BIN_FILE%" (
    echo Firmware binary not found: %BIN_FILE%
    exit /b 1
)

REM Check for DFU devices
python -m pyfu_usb -l | findstr /R "ID [0-9a-fA-F]*:[0-9a-fA-F]*" >nul
if errorlevel 1 (
    echo No DFU devices found. Please connect your Hexadeck controller and send /fw/update/ command.
    exit /b 1
)

REM Upload firmware
python -m pyfu_usb -D "%BIN_FILE%" -a %ADDRESS%
