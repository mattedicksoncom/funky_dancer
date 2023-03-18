@echo off
if "%1"=="" (
    echo add sdl2 version number
    echo eg. download_sdl2_mingw.bat 2.2.16
    exit /B 1
)

set SDL2_VERSION=%1

echo downloading SDL %SDL2_VERSION% for Win32 x64
powershell -Command "Invoke-WebRequest -Uri ('https://libsdl.org/release/SDL2-' + $env:SDL2_VERSION + '-win32-x64.zip') -OutFile ('SDL2-' + $env:SDL2_VERSION + '-win32-x64.zip')"
echo downloading SDL2 for mingw
powershell -Command "Invoke-WebRequest -Uri ('https://libsdl.org/release/SDL2-devel-' + $env:SDL2_VERSION + '-mingw.tar.gz') -OutFile ('SDL2-devel-' + $env:SDL2_VERSION + '-mingw.tar.gz')"
echo extracting
powershell -Command "Expand-Archive -Path ('.\SDL2-' + $env:SDL2_VERSION + '-win32-x64.zip') -DestinationPath ."
powershell -Command "tar -xzf ('.\SDL2-devel-' + $env:SDL2_VERSION + '-mingw.tar.gz') -C ."
echo move x86_64-w64-mingw32 to SDL2 folder
powershell -Command "if (-not (Test-Path -Path '.\SDL2')) { New-Item -Path '.\SDL2' -ItemType Directory }"
powershell -Command "Move-Item -Path ('.\SDL2-' + $env:SDL2_VERSION + '\x86_64-w64-mingw32\*') -Destination '.\SDL2'"
echo move SDL2.dll to build folder
powershell -Command "if (-not (Test-Path -Path '.\builds\windows\')) { New-Item -Path '.\builds\windows' -ItemType Directory }"
powershell -Command "Move-Item -Path ('.\SDL2.dll') -Destination '.\builds\windows\'"
echo clean up the archives and folders
powershell -Command "Remove-Item -Path ('.\SDL2-' + $env:SDL2_VERSION + '-win32-x64.zip') -Force"
powershell -Command "Remove-Item -Path ('.\SDL2-devel-' + $env:SDL2_VERSION + '-mingw.tar.gz') -Force"
powershell -Command "Remove-Item -Path ('.\SDL2-' + $env:SDL2_VERSION) -Recurse -Force"
echo SDL2 set up success