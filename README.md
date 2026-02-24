# cementplantsimulator
System requirements:
    Operational system: MS Windows 10,11, Gentoo Linux 2025.0 and higher, Calculate Linux 16 and higher.
    Software: Qt 6.10, cmake 3.20

Compile with debug support.

mkdir build
cd build

!rm -rf *; cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED=ON ../; cmake --build .
