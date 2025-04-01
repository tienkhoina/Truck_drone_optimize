build
    cmake .. -G "Visual Studio 17 2022" (dùng visual studio)
    mkdir ./build
    cd ./build
    cmake --build . --config Release
run
    build\Release\ProjectGA.exe

chú ý đường dẫn thư viện:
    vcpkg
    ortools

thư viện:
    google ortools
    zlib

