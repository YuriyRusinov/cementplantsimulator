Compile with debug support.

mkdir build
cd build

!rm -rf *; cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED=ON ../; cmake --build .   
