@echo off
cd %~dp0../

echo == Launching mGBA ==

set MGBA_PATH=G:\My Games\ROM tools\mGBA\mGBA-0.10.0-win64\mGBA.exe
set ROM_PATH=%cd%\pokeemerald-test.elf

echo Launching: "%MGBA_PATH%" -g "%ROM_PATH%"
start "" "%MGBA_PATH%" -g "%ROM_PATH%"