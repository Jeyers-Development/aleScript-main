@echo off

IF EXIST build (
    rmdir /s /q build
)

mkdir build
cd build

xcopy /e /i /y ..\resources resources

cmake -G "MinGW Makefiles" ..

cmake --build . --config Release