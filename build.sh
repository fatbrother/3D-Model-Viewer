root_path=$(cd `dirname $0`; pwd)

mkdir -p $root_path/build
mkdir -p $root_path/build/bin
cd $root_path/build
cmake cmake -B . -S . -DCMAKE_TOOLCHAIN_FILE=$root_path/vcpkg/scripts/buildsystems/vcpkg.cmake ../
cmake --build .
