#include <iostream>
#include <cassert>
#include "MinHeap.h"

int main() {
    std::cout << "[UNIT TEST] Kiểm thử lớp MinHeap với các mã nút UTC2...\n";

    MinHeap heap;
    assert(heap.isEmpty());
    assert(heap.getSize() == 0);

    // Insert các mã nút UTC2
    heap.insert("H2", 120.0);
    heap.insert("G", 0.0);
    heap.insert("A2", 35.0);
    heap.insert("E7", 55.0);
    heap.insert("C1", 75.0);

    assert(heap.getSize() == 5);
    assert(!heap.isEmpty());

    // Đỉnh heap phải là G (0.0)
    assert(heap.peek().name == "G");
    assert(heap.peek().distance == 0.0);

    // decreaseKey: Giảm khoảng cách của H2 từ 120.0 xuống 25.0
    bool decRes = heap.decreaseKey("H2", 25.0);
    assert(decRes == true);
    assert(heap.contains("H2"));
    assert(heap.getDistance("H2") == 25.0);

    // Trích xuất lần lượt: G (0.0) -> H2 (25.0) -> A2 (35.0) -> E7 (55.0) -> C1 (75.0)
    HeapNode n1 = heap.extractMin();
    assert(n1.name == "G" && n1.distance == 0.0);

    HeapNode n2 = heap.extractMin();
    assert(n2.name == "H2" && n2.distance == 25.0);

    HeapNode n3 = heap.extractMin();
    assert(n3.name == "A2" && n3.distance == 35.0);

    HeapNode n4 = heap.extractMin();
    assert(n4.name == "E7" && n4.distance == 55.0);

    HeapNode n5 = heap.extractMin();
    assert(n5.name == "C1" && n5.distance == 75.0);

    assert(heap.isEmpty());
    assert(heap.getSize() == 0);

    std::cout << "[UNIT TEST] MinHeap: Tất cả các asserts đều vượt qua!\n";
    return 0;
}
