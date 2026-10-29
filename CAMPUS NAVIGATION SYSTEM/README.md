# Hệ thống dẫn đường khuôn viên UTC2

> Dự án môn Cấu trúc dữ liệu và Giải thuật (DSA) — Trường Đại học Giao thông Vận tải, Phân hiệu tại TP. Hồ Chí Minh.

Ứng dụng hỗ trợ tra cứu đường đi trong khuôn viên UTC2. Người dùng có thể chọn điểm đi và điểm đến, xem tuyến đường trên bản đồ tương tác, tính lộ trình qua nhiều điểm, tránh các đoạn đường bị chặn và kiểm tra thời gian di chuyển giữa các lớp. Backend được viết bằng C++17 và cung cấp REST API cho giao diện web.

## Tính năng

- Tìm đường ngắn nhất bằng Dijkstra; tự chọn lối vào tối ưu cho một số tòa nhà có nhiều lối vào.
- Hiển thị tuyến đường và thông tin quãng đường, thời gian đi bộ ước tính trên bản đồ web.
- Tính tuyến đường qua nhiều điểm dừng.
- Đánh dấu đường bị chặn và tìm tuyến thay thế.
- Kiểm tra khả năng kịp giờ giữa hai lớp theo thời gian nghỉ.
- Phục vụ giao diện web và dữ liệu bản đồ bằng máy chủ HTTP C++.

## Công nghệ

- **Backend:** C++17, [cpp-httplib](https://github.com/yhirose/cpp-httplib) và [nlohmann/json](https://github.com/nlohmann/json) (các header đã có trong `core-cpp/include/`).
- **Frontend:** HTML, CSS và JavaScript thuần.
- **Dữ liệu:** CSV cho đồ thị và JSON cho thông tin hiển thị của các địa điểm.

## Yêu cầu

- Windows và MinGW-w64 `g++` có trong biến môi trường `PATH`.
- Trình duyệt web hiện đại.

## Chạy ứng dụng

Từ thư mục gốc dự án, chạy:

```bat
run.bat
```

Tập lệnh sẽ biên dịch backend, khởi động máy chủ tại cổng `8080` và mở trình duyệt. Nếu cổng này đang được sử dụng, hãy đóng tiến trình chiếm cổng hoặc đổi cổng trong lệnh chạy máy chủ.

Có thể khởi động máy chủ đã biên dịch trực tiếp:

```bat
campus_nav.exe 8080
```

Sau khi máy chủ chạy:

- Giao diện: <http://localhost:8080/>
- Kiểm tra trạng thái và số lượng nút/cạnh: <http://localhost:8080/api/health>
- Danh sách địa điểm: <http://localhost:8080/api/nodes>

Máy chủ cần được chạy từ thư mục gốc dự án để tìm thấy thư mục `web-ui/` và dữ liệu trong `core-cpp/data/`.

## Cấu trúc dự án

```text
.
├── core-cpp/
│   ├── data/                 # Dữ liệu nút và cạnh của đồ thị
│   ├── include/              # Header, cpp-httplib và nlohmann/json
│   ├── src/                  # Đồ thị, heap, thuật toán và điều hướng
│   ├── server.cpp            # HTTP server và REST API
│   ├── CMakeLists.txt
│   └── Makefile
├── tests/                    # Các chương trình kiểm thử C++
├── web-ui/                   # Giao diện, CSS, JavaScript và tài nguyên
├── nodes.json                # Dữ liệu địa điểm dùng cho giao diện
├── run.bat                   # Biên dịch và chạy ứng dụng trên Windows
├── ARCHITECTURE.md           # Mô tả kiến trúc
└── PROJECT_CONTEXT.md        # Quy ước dữ liệu và yêu cầu của bài tập
```

## Dữ liệu bản đồ

- `core-cpp/data/nodes.csv`: các nút của đồ thị, gồm mã, tên/mô tả và tọa độ.
- `core-cpp/data/edges.csv`: các cạnh nối giữa nút, trọng số và loại đường.
- `nodes.json`: thông tin địa điểm phục vụ giao diện; nếu không đọc được, backend có thể tạo danh sách từ đồ thị trong bộ nhớ.

Khi chỉnh sửa bản đồ, hãy cập nhật dữ liệu đồ thị và dữ liệu giao diện tương ứng để tuyến đường và vị trí hiển thị đồng nhất.

## Mã nguồn và đóng góp

Các thuật toán đồ thị và cấu trúc dữ liệu được cài đặt trong `core-cpp/src/` và `core-cpp/include/`. Xem `ARCHITECTURE.md` để biết cách các thành phần phối hợp và `PROJECT_CONTEXT.md` để biết các quy ước riêng của dữ liệu UTC2.

## Tác giả

Dự án bài tập lớn môn Cấu trúc dữ liệu và Giải thuật tại UTC2.
