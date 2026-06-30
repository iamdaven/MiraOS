@echo off
setlocal
set ZIG=%LOCALAPPDATA%\Microsoft\WinGet\Packages\zig.zig_Microsoft.Winget.Source_8wekyb3d8bbwe\zig-x86_64-windows-0.16.0\zig.exe
if not exist "%ZIG%" (
    for /f "delims=" %%i in ('where zig 2^>nul') do set ZIG=%%i
)
set PATH=C:\msys64\usr\bin;%PATH%
cd /d %~dp0
C:\msys64\usr\bin\bash.exe -lc "export PATH='/c/Users/dchua/AppData/Local/Microsoft/WinGet/Packages/zig.zig_Microsoft.Winget.Source_8wekyb3d8bbwe/zig-x86_64-windows-0.16.0':$PATH && cd /c/Users/dchua/MiraOS && make clean && make iso"
if exist miraos.iso echo Built miraos.iso successfully
endlocal
