# THÔNG TIN BÀI TẬP LỚN DSA: HỆ THỐNG DẪN ĐƯỜNG UTC2

## 1. Dữ liệu đầu vào
- File danh sách nút: `core-cpp/data/nodes.csv` và `nodes.json` (Cột: id, name, description, x, y)
- File danh sách cạnh: `core-cpp/data/edges.csv` (Cột: source, target, weight, road_type)
- Điểm xuất phát mặc định: Nút `G` (Cổng số 451, Lê Văn Việt).

## 2. Quy chuẩn kỹ thuật C++ (Bắt buộc theo barem chấm BTL)
- Cài đặt Danh sách kề (Adjacency List) bằng con trỏ thuần (`EdgeNode* next`), không dùng `std::vector` hay `std::list` làm cấu trúc lưu trữ nội tại của đồ thị.
- Tự cài đặt `MinHeap` (mảng động quản lý bằng con trỏ), không dùng thư viện ngoài `std::priority_queue`.
- Quản lý bộ nhớ chặt chẽ: có Destructor giải phóng vùng nhớ (`delete`), Rule of Three chống memory leak.
- **Backend Pure C++:** Sử dụng `core-cpp/server.cpp` với `httplib.h` và `json.hpp`, phục vụ toàn bộ REST API và Web UI tĩnh, không phụ thuộc vào Python.

## 3. Các quy ước đặc biệt của bản đồ
- Nút `H2`: Giảng đường Đa năng (ĐN).
- Nút `PZ`: Nhà để xe (lối vào duy nhất phía Nam từ `HP`).
- Nút `K`: Nhà C2 (Sảnh sau); `C2_Front`: Nhà C2 (Sảnh trước).
- Các tòa nhà có 2 lối vào:
  + Nhà E3: Lối Tây (`E3w`), Lối Đông (`D3`).
  + Nhà E4: Lối Tây (`E4w`), Lối Đông (`D2`).
  + Nhà E5: Lối Tây (`E5w`), Lối Đông (`D1`).
- Khi tìm đường tới tòa nhà có 2 lối vào, thuật toán tự động chọn lối vào có tổng quãng đường ngắn nhất.
