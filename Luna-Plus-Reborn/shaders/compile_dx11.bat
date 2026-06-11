@echo off
REM Compile shaders for Windows D3D11 — run this on Windows
REM Requires: shaderc.exe in PATH or set SHADERC_PATH
SET SHADERC=shaderc.exe
SET SHADERS=%~dp0
SET INCLUDE=%~dp0..\external\bgfx\src
SET VDEF=%SHADERS%varying.def.sc

IF NOT EXIST "%SHADERS%dx11" mkdir "%SHADERS%dx11"

SET OK=0
SET FAIL=0

FOR %%f IN ("%SHADERS%vs_*.sc") DO (
    SET name=%%~nf
    "%SHADERC%" -f "%%f" -o "%SHADERS%dx11\%%~nf.bin" --type vertex --platform windows -p s_5_0 --varyingdef "%VDEF%" -i "%INCLUDE%"
    IF ERRORLEVEL 1 (echo FAIL: %%~nf) ELSE (echo OK: %%~nf)
)

FOR %%f IN ("%SHADERS%fs_*.sc") DO (
    SET name=%%~nf
    "%SHADERC%" -f "%%f" -o "%SHADERS%dx11\%%~nf.bin" --type fragment --platform windows -p s_5_0 --varyingdef "%VDEF%" -i "%INCLUDE%"
    IF ERRORLEVEL 1 (echo FAIL: %%~nf) ELSE (echo OK: %%~nf)
)

echo Done. Run from shaders/ directory on Windows with shaderc in PATH.
