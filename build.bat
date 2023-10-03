@echo off

rem 取得當前目錄
set root_path=%cd%
cd /d %root_path%

rem 建立 build 目錄
mkdir build
mkdir build\bin
cd build

rem 使用 cmake 建立專案
cmake . -B . -S . -DCMAKE_TOOLCHAIN_FILE=%root_path%\vcpkg\scripts\buildsystems\vcpkg.cmake ../

rem 使用 cmake 編譯專案
cmake --build .