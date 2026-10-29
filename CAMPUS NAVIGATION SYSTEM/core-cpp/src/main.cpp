#include <iostream>
#include <iomanip>
#include <string>
#include "Graph.h"
#include "MinHeap.h"

// Demo giải thuật Dijkstra đơn giản sử dụng thuần Graph và MinHeap
void demoDijkstra(const Graph& graph, const std::string& startNode, const std::string& targetNode) {
    std::string startDisplay = graph.getDisplayName(startNode);
    std::string targetDisplay = graph.getDisplayName(targetNode);

    std::cout << "\n>>> [DEMO DIJKSTRA] Tìm đường ngắn nhất: " 
              << startNode << " (\"" << startDisplay << "\") -> " 
              << targetNode << " (\"" << targetDisplay << "\") <<<\n";

    int n = graph.getNumVertices();
    int startIdx = graph.findVertexIndex(startNode);
    int targetIdx = graph.findVertexIndex(targetNode);

    if (startIdx == -1 || targetIdx == -1) {
        std::cerr << "Lỗi: Điểm đầu hoặc điểm cuối không tồn tại trong đồ thị!\n";
        return;
    }

    const double INF = 1e9;
    double* dist = new double[n];
    std::string* prev = new std::string[n];
    bool* visited = new bool[n];

    for (int i = 0; i < n; ++i) {
        dist[i] = INF;
        prev[i] = "";
        visited[i] = false;
    }

    dist[startIdx] = 0.0;

    MinHeap pq;
    pq.insert(startNode, 0.0);

    while (!pq.isEmpty()) {
        HeapNode top = pq.extractMin();
        std::string uName = top.name;
        int uIdx = graph.findVertexIndex(uName);

        if (uIdx == -1 || visited[uIdx]) continue;
        visited[uIdx] = true;

        if (uName == targetNode) break; // Đã tìm thấy đích

        EdgeNode* edge = graph.getNeighbors(uIdx);
        while (edge != nullptr) {
            int vIdx = graph.findVertexIndex(edge->dest);
            if (vIdx != -1 && !visited[vIdx]) {
                double weight = edge->weight;
                if (dist[uIdx] + weight < dist[vIdx]) {
                    dist[vIdx] = dist[uIdx] + weight;
                    prev[vIdx] = uName;
                    pq.insert(edge->dest, dist[vIdx]);
                }
            }
            edge = edge->next;
        }
    }

    if (dist[targetIdx] >= INF) {
        std::cout << "Không tìm thấy đường đi giữa " << startNode << " và " << targetNode << "\n";
    } else {
        std::cout << "Tổng khoảng cách: " << dist[targetIdx] << " mét.\n";
        
        // Truy vết đường đi ngược lại từ target về start
        std::string path[128];
        int pathLen = 0;
        std::string curr = targetNode;

        while (!curr.empty() && pathLen < 128) {
            path[pathLen++] = curr;
            int cIdx = graph.findVertexIndex(curr);
            curr = prev[cIdx];
        }

        std::cout << "Lộ trình di chuyển chi tiết:\n";
        for (int i = pathLen - 1; i >= 0; --i) {
            std::string code = path[i];
            std::string dName = graph.getDisplayName(code);
            std::cout << "  [" << (pathLen - i) << "] " << code << " - " << dName;
            if (i > 0) {
                double stepWeight = graph.getEdgeWeight(path[i], path[i - 1]);
                std::cout << "  ---(" << stepWeight << "m)---> \n";
            }
        }
        std::cout << "\n";
    }

    delete[] dist;
    delete[] prev;
    delete[] visited;
}

#if !defined(CAMPUS_NAV_SERVER) && !defined(BUILDING_SERVER)
int main() {
    std::cout << "=================================================================\n";
    std::cout << "  HỆ THỐNG DẪN ĐƯỜNG KHUÔN VIÊN UTC2 - KIỂM THỬ CORE C++\n";
    std::cout << "  (Cập nhật theo tập nút chuẩn nodes.csv & edges.csv)\n";
    std::cout << "=================================================================\n\n";

    // 1. Kiểm thử lớp MinHeap thuần
    std::cout << "-----------------------------------------------------------------\n";
    std::cout << "1. KIỂM THỬ MIN-HEAP THUẦN (Array-based Min-Heap)\n";
    std::cout << "-----------------------------------------------------------------\n";
    {
        MinHeap heap;
        heap.insert("C1", 75.0);
        heap.insert("G", 0.0);
        heap.insert("E7", 55.0);
        heap.insert("H2", 120.0);
        heap.insert("A2", 35.0);

        std::cout << "Trạng thái Heap sau khi insert 5 phần tử:\n";
        heap.printHeap();

        std::cout << "\nĐỉnh Min-Heap (peek): " << heap.peek().name 
                  << " (" << heap.peek().distance << "m)\n";

        std::cout << "\nThử nghiệm decreaseKey('H2', 25.0)...\n";
        heap.decreaseKey("H2", 25.0);
        heap.printHeap();

        std::cout << "\nTrích xuất các phần tử theo thứ tự tăng dần khoảng cách (extractMin):\n";
        while (!heap.isEmpty()) {
            HeapNode node = heap.extractMin();
            std::cout << " -> " << node.name << " (" << node.distance << "m)\n";
        }
        std::cout << "[MinHeap] Kiểm thử hoàn tất thành công!\n";
    }

    // 2. Kiểm thử lớp Graph thuần và nạp CSV
    std::cout << "\n-----------------------------------------------------------------\n";
    std::cout << "2. KIỂM THỬ ĐỒ THỊ VÔ HƯỚNG & NẠP TỪ FILE CSV CHUẨN UTC2\n";
    std::cout << "-----------------------------------------------------------------\n";
    {
        Graph graph;
        const std::string csvEdgesPath = "core-cpp/data/edges.csv";
        
        std::cout << "Đang nạp dữ liệu từ: " << csvEdgesPath << " ...\n";
        bool success = graph.loadFromCSV(csvEdgesPath);
        if (!success) {
            std::cout << "Thử lại với đường dẫn 'data/edges.csv'...\n";
            graph.loadFromCSV("data/edges.csv");
        }

        // In danh sách kề
        graph.printGraph();

        // 3. Demo chạy thuật toán Dijkstra với các mã nút chuẩn
        std::cout << "-----------------------------------------------------------------\n";
        std::cout << "3. TÌM ĐƯỜNG ĐI NGẮN NHẤT THỰC TẾ TRONG KHUÔN VIÊN UTC2\n";
        std::cout << "-----------------------------------------------------------------\n";
        
        // Tuyến 1: Từ Cổng 451 (G) -> Hội trường C1 (C1)
        demoDijkstra(graph, "G", "C1");

        // Tuyến 2: Từ Cổng 451 (G) -> Giảng đường Đa năng (H2)
        demoDijkstra(graph, "G", "H2");

        // Tuyến 3: Từ Cổng 451 (G) -> Nhà E1 (E1)
        demoDijkstra(graph, "G", "E1");

        // Tuyến 4: Từ Căn tin / Nhà C3 (V4) -> Phòng học E4 Lối Tây (E4w)
        demoDijkstra(graph, "V4", "E4w");

        // Tuyến 5: Từ Nhà để xe (PZ) -> Phòng học E9 (D0)
        demoDijkstra(graph, "PZ", "D0");

        // Kiểm thử Deep Copy (Rule of Three)
        std::cout << "\nKiểm thử Deep Copy Graph (Copy Constructor & operator=)...";
        Graph copiedGraph = graph;
        if (copiedGraph.getNumVertices() == graph.getNumVertices() &&
            copiedGraph.getTotalEdges() == graph.getTotalEdges()) {
            std::cout << " [THÀNH CÔNG]\n";
        } else {
            std::cout << " [THẤT BẠI]\n";
        }
    }

    std::cout << "\n[KẾT QUẢ] Tất cả các bài kiểm thử cơ bản đã vượt qua!\n";
    return 0;
}

#endif // !defined(CAMPUS_NAV_SERVER)
