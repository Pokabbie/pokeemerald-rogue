@echo off
setlocal enabledelayedexpansion
cd %~dp0../

@rem == THIS ONLY WORKS FOR FAILURES?? ==
@rem grep regex to filter out the clear command that's coming from somewhere and losing the output
@rem call C:\devkitPro\msys2\msys2_shell.bat -mingw64 -here -defterm -no-start launch_build.sh | grep 's/.\[H.\[2J//'

call C:\devkitPro\msys2\msys2_shell.bat -mingw64 -here -defterm -no-start %~dp0launch_clean.sh

if not !ERRORLEVEL!==0 (
   echo "Successful! (Output discarded)"
)
exit /b !ERRORLEVEL!
