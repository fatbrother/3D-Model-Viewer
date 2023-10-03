@echo off

rem 取得當前目錄
set root_path=%cd%
cd /d %root_path%

rem 建立 build 目錄
mkdir build
mkdir build\bin

set build_path=%root_path%\build
set vcpkg_path=%root_path%\vcpkg

rem 使用 cmake 建立專案
cmake -B %build_path% -S . -DCMAKE_TOOLCHAIN_FILE=%vcpkg_path%/scripts/buildsystems/vcpkg.cmake
rem 使用 cmake 編譯專案
cmake --build ./build --config Release