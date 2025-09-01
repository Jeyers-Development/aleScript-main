@echo off

IF EXIST build (
    rmdir /s /q build
)

mkdir build
cd build

cmake -G "MinGW Makefiles" ..

cmake --build . --config Release

"C:\Program Files (x86)\Resource Hacker\ResourceHacker.exe" -open "alescript.exe" -save "alescript.exe" -action addoverwrite -res "C:/Programming/aleScript-main/ale.ico" -mask ICONGROUP,MAINICON >NUL 2>&1

"C:\Program Files (x86)\Resource Hacker\ResourceHacker.exe" -open "aleIDE.exe" -save "aleIDE.exe" -action addoverwrite -res "../aleIDE.ico" -mask ICONGROUP,MAINICON >NUL 2>&1

alescript.exe ../program.ale