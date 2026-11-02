# Hệ thống dẫn đường khuôn viên UTC2

> Dự án môn Cấu trúc dữ liệu và Giải thuật (DSA) — Trường Đại học Giao thông Vận tải, Phân hiệu tại TP. Hồ Chí Minh.

Ứng dụng tra cứu đường đi trong khuôn viên UTC2. Người dùng chọn điểm xuất phát và điểm đến trên bản đồ tương tác, xem chỉ dẫn theo từng chặng, lập lộ trình qua nhiều điểm, đánh dấu đường bị chặn và kiểm tra thời gian di chuyển giữa hai tiết học. Backend C++17 cung cấp REST API và phục vụ giao diện web tĩnh.

## Mục lục

- [Tính năng](#tính-năng)
- [Công nghệ và thuật toán](#công-nghệ-và-thuật-toán)
- [Yêu cầu](#yêu-cầu)
- [Tải mã nguồn](#tải-mã-nguồn)
- [Biên dịch và chạy](#biên-dịch-và-chạy)
- [Sử dụng API](#sử-dụng-api)
- [Dữ liệu bản đồ](#dữ-liệu-bản-đồ)
- [Cấu trúc thư mục](#cấu-trúc-thư-mục)
- [Kiểm thử](#kiểm-thử)
- [Tài liệu liên quan](#tài-liệu-liên-quan)

## Tính năng

- Tìm đường đi ngắn nhất giữa hai địa điểm bằng Dijkstra.
- Tự chọn lối vào có tổng quãng đường ngắn nhất cho một số tòa nhà có nhiều lối vào.
- Hiển thị tuyến đường, khoảng cách, thời gian đi bộ ước tính và chỉ dẫn rẽ trên bản đồ.
- Tạo lộ trình qua nhiều điểm dừng theo thứ tự đầu vào.
- Đánh dấu các đoạn đường bị chặn để tìm tuyến thay thế.
- Kiểm tra người dùng có đủ thời gian đi từ phòng học tiết trước đến phòng học tiết sau hay không.
- So sánh Dijkstra, A* và BFS qua API benchmark.
- Phục vụ giao diện web và REST API từ máy chủ HTTP C++.

## Công nghệ và thuật toán

- **Backend:** C++17, thư viện [cpp-httplib](https://github.com/yhirose/cpp-httplib) và [nlohmann/json](https://github.com/nlohmann/json). Các header thư viện đã có trong `core-cpp/include/`, không cần tải riêng.
- **Frontend:** HTML, CSS và JavaScript thuần; bản đồ được hiển thị bằng canvas.
- **Đồ thị:** Danh sách kề tự cài đặt bằng con trỏ; trọng số cạnh biểu diễn khoảng cách.
- **Cấu trúc dữ liệu:** Min-heap tự cài đặt.
- **Thuật toán:** Dijkstra, A*, BFS; các phần điều hướng và tối ưu lịch học nằm trong `core-cpp/src/`.
- **Dữ liệu:** CSV cho đồ thị và JSON cho thông tin địa điểm trên giao diện.

## Yêu cầu

Để chạy toàn bộ ứng dụng trên Windows:

- Windows 10 trở lên.
- MinGW-w64 hoặc MSYS2 có `g++` hỗ trợ C++17 và đã được thêm vào biến môi trường `PATH`.
- Trình duyệt web hiện đại.

Để chạy máy chủ, không cần cài Node.js, Python hay tải thư viện C++ bên ngoài. Các thư viện header-only cần thiết đã nằm trong repository.

## Tải mã nguồn

Clone repository và chuyển vào thư mục dự án:

```bash
git clone <URL-repository>
cd "CAMPUS NAVIGATION SYSTEM"
```

Thay `<URL-repository>` bằng URL Git của dự án. Nếu đã tải mã nguồn dưới dạng ZIP, hãy giải nén rồi mở terminal tại thư mục có `run.bat` và `core-cpp/`.

## Biên dịch và chạy

### Cách 1: Chạy nhanh trên Windows

Mở PowerShell hoặc Command Prompt tại thư mục gốc dự án, sau đó chạy:

```bat
run.bat
```

Tập lệnh sẽ kiểm tra `g++`, biên dịch backend thành `campus_nav.exe`, khởi động máy chủ ở cổng `8080` và mở trình duyệt tại `http://localhost:8080/`. Cửa sổ máy chủ được mở riêng; giữ cửa sổ này hoạt động trong khi sử dụng ứng dụng. Nhấn `Ctrl+C` trong cửa sổ máy chủ để dừng.

Nếu Windows báo không tìm thấy `g++`, hãy cài MinGW-w64/MSYS2, thêm thư mục chứa `g++.exe` vào `PATH`, mở terminal mới và chạy lại.

### Cách 2: Biên dịch và chạy thủ công trên Windows

Từ thư mục gốc của dự án, biên dịch máy chủ web bằng lệnh tương ứng với `run.bat`:

```bat
g++ -std=c++17 -D_WIN32_WINNT=0x0A00 -DCAMPUS_NAV_SERVER -O2 core-cpp/*.cpp core-cpp/src/*.cpp -I core-cpp/include -o campus_nav.exe -lws2_32
```

Sau khi biên dịch thành công, chạy máy chủ:

```bat
campus_nav.exe 8080
```

Mở `http://localhost:8080/` trong trình duyệt. Có thể truyền cổng khác thay cho `8080`; biến môi trường `PORT` được ưu tiên nếu đã được thiết lập.

> Chạy chương trình từ **thư mục gốc dự án** để máy chủ tìm được `core-cpp/data/`, `nodes.json` và `web-ui/`.

### Biên dịch phần lõi bằng CMake hoặc Make

Các cấu hình trong `core-cpp/CMakeLists.txt` và `core-cpp/Makefile` hiện biên dịch chương trình lõi dòng lệnh (`src/main.cpp`, đồ thị và heap); chúng không biên dịch máy chủ HTTP trong `server.cpp`.

Ví dụ dùng CMake:

```bash
cmake -S core-cpp -B core-cpp/build
cmake --build core-cpp/build
```

Ví dụ dùng Make (cần có `make` và `g++`):

```bash
cd core-cpp
make
```

## Sử dụng ứng dụng

Khi máy chủ đang chạy:

- Giao diện bản đồ: <http://localhost:8080/>
- Trạng thái backend: <http://localhost:8080/api/health>
- Danh sách địa điểm: <http://localhost:8080/api/nodes>

Trong giao diện, chọn điểm đi/đến hoặc các chức năng lộ trình nhiều điểm và kiểm tra lịch học. Mã địa điểm cần dùng trong API có thể xem tại `/api/nodes`.

## Sử dụng API

Máy chủ mặc định chạy tại `http://localhost:8080`. Các API trả dữ liệu JSON. Ví dụ dưới đây dùng `curl` và có thể chạy trong môi trường cài curl.

### Trạng thái máy chủ

```http
GET /api/health
```

Trả về trạng thái, số đỉnh/cạnh đã nạp và danh sách endpoint.

### Danh sách địa điểm

```http
GET /api/nodes
```

Trả về `count` và danh sách `nodes`, bao gồm mã, tên hiển thị, mô tả và tọa độ giao diện.

### Tìm đường giữa hai điểm

```bash
curl "http://localhost:8080/api/route?start=G&end=E3"
```

`start` là mã điểm xuất phát (mặc định `G`), `end` là mã đích (bắt buộc). Có thể truyền `blocked` để tránh một hoặc nhiều cạnh, ví dụ `blocked=G-T,T-J`; khi gọi bằng URL, ký tự đặc biệt cần được URL-encode nếu cần.

Kết quả thành công có các trường như `path`, `total_distance`, `walking_time_sec`, `directions` và `turn_by_turn`. Nếu thiếu `end`, API trả mã HTTP `400`; nếu không tìm thấy đường đi, API trả `404`.

### Lộ trình qua nhiều điểm

```bash
curl -X POST "http://localhost:8080/api/multi-stop" \
  -H "Content-Type: application/json" \
  -d '{"stops":["G","T","K"],"blocked":""}'
```

`stops` phải có ít nhất hai mã địa điểm. Điểm đầu tiên là điểm xuất phát; hệ thống tự sắp xếp các điểm còn lại để tìm hành trình ngắn, dùng vét cạn khi có tối đa 5 điểm và Nearest Neighbor kết hợp 2-Opt khi có nhiều điểm hơn. Kết quả cho biết thứ tự ghé đã chọn, các chặng, tổng khoảng cách, thời gian đi bộ và chỉ dẫn.

### Kiểm tra thời gian giữa hai tiết học

```bash
curl -X POST "http://localhost:8080/api/schedule-check" \
  -H "Content-Type: application/json" \
  -d '{"room1":"K","room2":"G","break_time":300,"blocked":""}'
```

`room1` và `room2` là mã phòng/địa điểm; `break_time` là thời gian nghỉ có sẵn tính bằng giây (mặc định 300). Kết quả cho biết khoảng cách, thời gian đi bộ, trạng thái có nguy cơ trễ và tuyến đường.

### So sánh thuật toán

```bash
curl "http://localhost:8080/api/benchmark?start=G&end=E3"
```

Endpoint trả về khoảng cách, số đỉnh đã duyệt và thời gian chạy của Dijkstra, A* và BFS. BFS tối ưu số cạnh, không tối ưu theo khoảng cách thực tế.

## Dữ liệu bản đồ

- `core-cpp/data/nodes.csv`: danh sách đỉnh với các cột `id,name,description,x,y`.
- `core-cpp/data/edges.csv`: danh sách cạnh với các cột `source,target,weight,road_type`. Trọng số là khoảng cách dùng để tính tuyến đường; cạnh được nạp theo mô hình đồ thị vô hướng.
- `nodes.json`: thông tin địa điểm và tọa độ dùng cho giao diện web/API `/api/nodes`.

Khi cập nhật bản đồ, hãy giữ mã định danh giữa CSV và JSON nhất quán. Cập nhật cả dữ liệu đồ thị lẫn dữ liệu hiển thị để tuyến đường và vị trí điểm trên bản đồ khớp nhau. Nếu `nodes.json` không đọc được hoặc rỗng, backend có thể dựng danh sách địa điểm từ đồ thị đã nạp.

## Cấu trúc thư mục

```text
.
├── core-cpp/
│   ├── data/                 # nodes.csv, edges.csv và dữ liệu địa điểm
│   ├── include/              # Header dự án và thư viện header-only
│   ├── src/                  # Đồ thị, heap, thuật toán và điều hướng
│   ├── server.cpp            # HTTP server và REST API
│   ├── CMakeLists.txt        # Cấu hình build phần lõi
│   └── Makefile              # Cấu hình build phần lõi
├── tests/                    # Mã nguồn các chương trình kiểm thử
├── web-ui/
│   ├── assets/               # Bản đồ và tài nguyên giao diện
│   ├── css/                  # Kiểu dáng
│   ├── js/                   # Logic giao diện và bản đồ
│   └── index.html            # Trang chính
├── nodes.json                # Dữ liệu địa điểm cho giao diện
├── run.bat                   # Biên dịch và chạy server trên Windows
├── ARCHITECTURE.md           # Mô tả kiến trúc chi tiết
└── PROJECT_CONTEXT.md        # Quy ước dữ liệu và yêu cầu bài tập
```

## Kiểm thử

Thư mục `tests/` chứa các chương trình kiểm thử cho heap, Dijkstra và các chức năng nâng cao. Chúng là các file nguồn C++ độc lập; cách biên dịch/chạy có thể khác nhau tùy trình biên dịch và cấu hình trong nhánh đang sử dụng. Phần CMake/Make hiện tại không tự động tạo các target kiểm thử.

## Tài liệu liên quan

- [ARCHITECTURE.md](ARCHITECTURE.md): mô tả các thành phần và cách chúng phối hợp.
- [PROJECT_CONTEXT.md](PROJECT_CONTEXT.md): quy ước dữ liệu, các điểm đặc biệt trên bản đồ và yêu cầu kỹ thuật của bài tập.

## Tác giả

Dự án bài tập lớn môn Cấu trúc dữ liệu và Giải thuật tại UTC2.
