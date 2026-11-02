#ifndef MINHEAP_H
#define MINHEAP_H

#include <string>
#include <iostream>

/**
 * @brief Cấu trúc phần tử lưu trữ trong Min-Heap.
 * Đại diện cho cặp (tên nút, khoảng cách ngắn nhất) phục vụ giải thuật Dijkstra.
 */
struct HeapNode {
    std::string name;    // Tên định danh của địa điểm / nút
    double distance;     // Khoảng cách ngắn nhất hiện tại (key ưu tiên)

    HeapNode(const std::string& n = "", double d = 0.0)
        : name(n), distance(d) {}
};

/**
 * @brief Lớp MinHeap tự cài đặt trên mảng động con trỏ thuần.
 * Không sử dụng std::priority_queue hay std::vector.
 * Phục vụ hàng đợi ưu tiên cho giải thuật Dijkstra và A*.
 * Quản lý bộ nhớ nghiêm ngặt theo Rule of Three.
 */
class MinHeap {
private:
    HeapNode* heapArray; // Mảng động lưu trữ các phần tử của cây nhị phân heap
    int capacity;        // Sức chứa tối đa của mảng
    int size;            // Số lượng phần tử hiện có trong heap

    // Các hàm tính chỉ số trong cây nhị phân heap (0-indexed)
    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return 2 * i + 1; }
    int rightChild(int i) const { return 2 * i + 2; }

    // Hoán vị 2 phần tử trong mảng heap
    void swapNodes(int i, int j);

    // Vun đống lên trên khi thêm phần tử hoặc giảm khoảng cách
    void siftUp(int index);

    // Vun đống xuống dưới khi trích xuất phần tử nhỏ nhất
    void siftDown(int index);

    // Tự động mở rộng kích thước mảng động khi đầy
    void resize(int newCapacity);

    // Giải phóng bộ nhớ động
    void freeMemory();

    // Sao chép sâu từ đối tượng khác
    void copyFrom(const MinHeap& other);

public:
    // Khởi tạo Min-Heap với sức chứa ban đầu (mặc định 32)
    explicit MinHeap(int initialCapacity = 32);

    // Destructor: Giải phóng vùng nhớ heapArray
    ~MinHeap();

    // Copy Constructor & Assignment Operator (Rule of Three)
    MinHeap(const MinHeap& other);
    MinHeap& operator=(const MinHeap& other);

    // Thêm một cặp (name, distance) vào heap
    void insert(const std::string& name, double distance);

    // Trích xuất và xóa phần tử có khoảng cách nhỏ nhất (tại đỉnh heap)
    HeapNode extractMin();

    // Xem phần tử nhỏ nhất tại đỉnh heap mà không xóa
    HeapNode peek() const;

    // Giảm khoảng cách của nút đã có trong heap và vun lại đống
    // Trả về true nếu cập nhật thành công, false nếu không tìm thấy
    bool decreaseKey(const std::string& name, double newDistance);

    // Kiểm tra xem một nút có đang tồn tại trong heap hay không
    bool contains(const std::string& name) const;

    // Lấy khoảng cách hiện tại của một nút trong heap (trả về -1.0 nếu không có)
    double getDistance(const std::string& name) const;

    // Kiểm tra heap có rỗng hay không
    bool isEmpty() const;

    // Lấy số lượng phần tử hiện có
    int getSize() const;

    // Lấy sức chứa hiện tại
    int getCapacity() const;

    // Xóa toàn bộ phần tử trong heap (đưa size về 0)
    void clear();

    // In toàn bộ phần tử trong heap ra console phục vụ gỡ lỗi (debug)
    void printHeap() const;
};

#endif // MINHEAP_H

