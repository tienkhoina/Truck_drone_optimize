cmake_minimum_required(VERSION 3.10)
project(ProjectGA)

# Sử dụng UTF-8 để tránh lỗi encoding trên Windows
add_compile_options(/utf-8)

# Đường dẫn tới bản prebuilt OR-Tools (đã giải nén)
set(ORTOOLS_DIR "D:/discord data set/or-tools_x64_VisualStudio2022_cpp_v9.10.4067/or-tools_x64_VisualStudio2022_cpp_v9.10.4067")

# Gán vào CMAKE_PREFIX_PATH để find_package tìm được các file .cmake của OR-Tools
set(CMAKE_PREFIX_PATH "${ORTOOLS_DIR}")

# Tìm kiếm thư viện OR-Tools (các phụ thuộc như protobuf, abseil, zlib... đã được build sẵn bên trong OR-Tools)
find_package(ortools REQUIRED)

# Thêm thư mục chứa file header vào đường dẫn tìm kiếm
include_directories(${PROJECT_SOURCE_DIR}/header)

# Tìm tất cả các file .cpp trong thư mục main
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/main/*.cpp)

# Tạo file thực thi từ source code
add_executable(ProjectGA ${SOURCE_FILES})

# Liên kết thư viện với file thực thi
target_link_libraries(ProjectGA PRIVATE ortools::ortools)
