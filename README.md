# Hướng dẫn Cài đặt và Sử dụng ProjectGA

## Yêu cầu hệ thống

- Visual Studio 2022
- CMake
- Thư viện Google OR-Tools C++ (khuyến nghị phiên bản v9.10: [or-tools_x64_VisualStudio2022_cpp_v9.10.4067.zip](https://github.com/google/or-tools/releases))

## Các bước cài đặt

1. **Cài đặt các phần mềm cần thiết**
   - Cài đặt Visual Studio 2022.
   - Cài đặt CMake.
   - Tải và giải nén OR-Tools C++ vào một thư mục trên máy.

2. **Cấu hình dự án**
   - Mở file [`CMakeLists.txt`](CMakeLists.txt) và chỉnh sửa đường dẫn OR-Tools cho phù hợp với vị trí bạn đã giải nén.

3. **Build dự án**
   ```sh
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022"
   cmake --build . --config Release
   ```

4. **Chạy ứng dụng**
   - Chạy file thực thi: `Release\ProjectGA.exe`          

## Lưu ý cấu hình:
- Chỉnh sửa file main.cpp: thay đổi đường dẫn trong file main.cpp với đường dẫn thực tế
- Tách từng bộ dữ liệu chạy, tránh chạy đồng thời nhiều bộ gây trần bộ nhớ (hiện file main.cpp đang cấu hình để chạy toàn bộ dữ liệu với cả 4 thuật toán)
- Có thể tự tạo bộ dữ liệu mới bàng cách thực thi Gendata.cpp có trong data
- Các chương trình để so sánh kết quả trong Result compare đang chạy với đường dẫn tuyệt đối cần cầu hình chuẩn với máy
