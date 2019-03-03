@echo off
REM Setup for compiling with Borland C++ Builder 5.0 in 32 bit Windows mode.

if .%CHECKED%==.1 goto checked_build
SET LIB=%SCITECH_LIB%\LIB\RELEASE\SNAP\BCB5;%BCB5_PATH%\LIB;.
echo Release build enabled.
goto setvars

:checked_build
SET LIB=%SCITECH_LIB%\LIB\DEBUG\SNAP\BCB5;%BCB5_PATH%\LIB;.
echo Checked debug build enabled.
goto setvars

:setvars
SET INCLUDE=INCLUDE;%SCITECH%\INCLUDE;%PRIVATE%\INCLUDE
SET MAKESTARTUP=%SCITECH%\MAKEDEFS\BC32.MK
SET USE_WIN16=
SET USE_WIN32=
SET USE_VXD=
SET USE_TNT=
SET USE_SMX32=
SET USE_SMX16=
SET USE_BC5=1
SET WIN32_GUI=
SET USE_SNAP=1
SET BC_LIBBASE=BC5
PATH %SCITECH_BIN%;%BCB5_PATH%\BIN;%DEFPATH%%BC5_CD_PATH%

REM: Create Borland compile/link configuration scripts
echo -I%INCLUDE% > %BCB5_PATH%\BIN\bcc32.cfg
echo -L%LIB% >> %BCB5_PATH%\BIN\bcc32.cfg
echo -L%LIB% > %BCB5_PATH%\BIN\tlink32.cfg

echo Borland C++ Builder 5.0 Snap compilation configuration set up.
