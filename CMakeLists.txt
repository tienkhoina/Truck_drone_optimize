cmake_minimum_required(VERSION 3.10)
project(ProjectGA)

# Sử dụng UTF-8 để tránh lỗi encoding trên Windows
add_compile_options(/utf-8)
# Chỉ định toolchain của vcpkg (đảm bảo vcpkg được sử dụng để tìm thư viện)
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Chỉ định đường dẫn OR-Tools
set(ORTOOLS_DIR "D:/discord data set/or-tools_x64_VisualStudio2022_cpp_v9.10.4067/or-tools_x64_VisualStudio2022_cpp_v9.10.4067/lib/cmake/ortools")

# Đảm bảo CMake có thể tìm thấy OR-Tools và ZLIB
set(CMAKE_PREFIX_PATH "${ORTOOLS_DIR};C:/vcpkg/installed/x64-windows")

# Tìm kiếm thư viện OR-Tools và ZLIB
find_package(ortools REQUIRED)
find_package(ZLIB REQUIRED)

# Thêm thư mục chứa file header vào đường dẫn tìm kiếm
include_directories(${PROJECT_SOURCE_DIR}/header)

# Tìm tất cả các file .cpp trong thư mục main
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/main/*.cpp)

# Tạo file thực thi từ source code
add_executable(ProjectGA ${SOURCE_FILES})

# Liên kết thư viện với file thực thi
target_link_libraries(ProjectGA PRIVATE ortools::ortools ZLIB::ZLIB)
