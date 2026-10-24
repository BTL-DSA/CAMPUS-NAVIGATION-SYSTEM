# Hệ thống dẫn đường khuôn viên UTC2

> Dự án bài tập lớn môn Cấu trúc dữ liệu và Giải thuật (DSA) — Trường Đại học Giao thông Vận tải, Phân hiệu tại TP. Hồ Chí Minh.

## Giới thiệu

Campus Navigation System là dự án xây dựng hệ thống hỗ trợ tìm đường trong khuôn viên UTC2. Người dùng có thể chọn điểm xuất phát và điểm đến để xem tuyến đường trên bản đồ.

Các chức năng dự kiến gồm tìm đường ngắn nhất, tránh các đoạn đường bị chặn, chọn lối vào phù hợp cho một số tòa nhà và hiển thị lộ trình trên giao diện web.

## Công nghệ dự kiến

- **C++17:** xử lý đồ thị, thuật toán tìm đường và máy chủ API.
- **HTML, CSS, JavaScript:** giao diện bản đồ tương tác.
- **Git:** quản lý mã nguồn.

## Cấu trúc dự án

```text
CampusNavigationSystem/
├── README.md
├── .gitignore
├── ARCHITECTURE.md
├── PROJECT_CONTEXT.md
├── run.bat
├── nodes.json
├── core-cpp/
│   ├── CMakeLists.txt
│   ├── Makefile
│   ├── server.cpp
│   ├── data/
│   │   ├── edges.csv
│   │   ├── nodes.csv
│   │   └── nodes.json
│   ├── include/
│   │   ├── AStar.h
│   │   ├── Dijkstra.h
│   │   ├── Graph.h
│   │   ├── MinHeap.h
│   │   ├── Navigator.h
│   │   └── ScheduleOptimizer.h
│   └── src/
│       ├── AStar.cpp
│       ├── Dijkstra.cpp
│       ├── Graph.cpp
│       ├── main.cpp
│       ├── MinHeap.cpp
│       ├── Navigator.cpp
│       └── ScheduleOptimizer.cpp
├── tests/
│   ├── test_advanced.cpp
│   ├── test_dijkstra.cpp
│   └── test_heap.cpp
└── web-ui/
    ├── index.html
    ├── assets/
    ├── css/
    └── js/
```

> Cấu trúc trên mô tả các thành phần dự kiến của dự án. Mã nguồn và dữ liệu sẽ được bổ sung vào repository sau.

## Trạng thái

Repository hiện được dùng để đăng cấu trúc và giới thiệu dự án. Các phần triển khai sẽ được cập nhật dần.

## Tác giả

Sinh viên thực hiện bài tập lớn môn Cấu trúc dữ liệu và Giải thuật tại UTC2.
