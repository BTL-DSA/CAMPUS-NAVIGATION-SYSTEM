# HỆ THỐNG DẪN ĐƯỜNG KHUÔN VIÊN UTC2
> **Bài tập lớn môn Cấu trúc Dữ liệu & Giải thuật (DSA)**  
> **Trường Đại học Giao thông Vận tải - Phân hiệu tại TP. Hồ Chí Minh (UTC2)**

---

## 📌 Giới thiệu dự án
Hệ thống dẫn đường thông minh hỗ trợ sinh viên, giảng viên và khách tham quan tìm kiếm lộ trình di chuyển tối ưu nhất trong khuôn viên UTC2 (450-451 Lê Văn Việt, TP. Thủ Đức). Hệ thống hỗ trợ tìm đường ngắn nhất, tự động chọn cửa vào tối ưu cho các tòa nhà có 2 lối vào (E3, E4, E5), né các đoạn đường đang bị chặn/thi công, và hiển thị lộ trình trực quan trên bản đồ tương tác.

---

## ⚙️ Công nghệ sử dụng

1. **Lõi thuật toán & Web Server C++ thuần (`core-cpp/`):**
   - **Đồ thị (Graph):** Biểu diễn bằng Danh sách kề (Adjacency List) sử dụng con trỏ thuần (`EdgeNode*`), hoàn toàn không dùng `std::vector` hay `std::list`. Quản lý bộ nhớ nghiêm ngặt theo **Rule of Three** chống rò rỉ bộ nhớ.
   - **Hàng đợi ưu tiên (Min-Heap):** Tự cài đặt Min-Heap trên mảng động thuần, không sử dụng `std::priority_queue`.
   - **Giải thuật Dijkstra:** Tìm đường đi ngắn nhất, hỗ trợ tập cạnh bị chặn (`BlockedEdgeList`) và tự động giải quyết tòa nhà 2 lối vào.
   - **Dẫn đường & Đa điểm (Navigator):** Tìm lộ trình đa điểm ghé thăm nhiều trạm (Multi-stop TSP) với tối ưu hóa 2-Opt cục bộ, tự động sinh hướng dẫn Turn-by-turn bằng tích có hướng (Cross Product).
   - **Tối ưu lịch học (Schedule Optimizer):** Cảnh báo nguy cơ trễ học giữa 2 tiết học kế tiếp.
   - **So sánh hiệu năng (A* Heuristic & Benchmark):** So sánh tốc độ và số đỉnh duyệt giữa Dijkstra, A* và BFS.
   - **Pure C++ Web Server (`core-cpp/server.cpp`):** Sử dụng thư viện header-only `httplib.h` (cpp-httplib) và `json.hpp` (nlohmann/json), phục vụ toàn bộ REST API và giao diện Web UI, loại bỏ hoàn toàn phụ thuộc vào Python.

2. **Giao diện Web tương tác (`web-ui/`):**
   - HTML5 Canvas với đầy đủ tính năng phóng to/thu nhỏ (zoom), kéo rê (pan), chạm đa điểm trên mobile.
   - Vẽ đường đi phát sáng (yellow glow) với hiệu ứng nét đứt chuyển động (marching dashes) và điểm đến nhấp nháy phát sáng.

---

## 🚀 Hướng dẫn khởi động đồ án (Dành cho chấm điểm BTL)

### Cách 1: Khởi động tự động bằng 1 cú click (Khuyên dùng trên Windows)
1. **Bước 1:** Nhấp đúp chuột vào file **`run.bat`** tại thư mục gốc của đồ án.
2. **Bước 2:** Hệ thống sẽ tự động biên dịch C++ Server bằng MinGW và mở trình duyệt tại:
   ```
   http://localhost:8080/
   ```

---

### Cách 2: Khởi động thủ công bằng dòng lệnh

**Bước 1: Biên dịch và chạy C++ Web Server**
```powershell
g++ -std=c++17 -D_WIN32_WINNT=0x0A00 -DCAMPUS_NAV_SERVER -O2 core-cpp/*.cpp core-cpp/src/*.cpp -I core-cpp/include -o campus_nav.exe -lws2_32
.\campus_nav.exe 8080
```

**Bước 2: Mở giao diện Web**
- Mở trình duyệt và truy cập: **`http://localhost:8080/`**
- Hoặc mở trực tiếp file: **`web-ui/index.html`**

---

## 🧪 Chạy kiểm thử tự động (Unit Tests)

```powershell
# 1. Chạy bộ kiểm thử C++ thuần Dijkstra & Cấu trúc dữ liệu
.\tests\test_dijkstra.exe

# 2. Chạy bộ kiểm thử các tính năng nâng cao (Multi-stop, Turn-by-turn, Schedule, A* Benchmark)
.\tests\test_advanced.exe
```
