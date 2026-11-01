#ifndef ASTAR_H
#define ASTAR_H

#include "Graph.h"
#include "MinHeap.h"
#include "Dijkstra.h"
#include <string>
#include <vector>
#include <chrono>

/**
 * @brief Kết quả của thuật toán A* tìm đường ngắn nhất.
 */
struct AStarResult {
    bool found;                      // Tìm thấy đường hay không
    double totalDistance;            // Tổng khoảng cách (mét)
    std::string startNode;           // Nút xuất phát thực tế được chọn (hỗ trợ đa sảnh C2: V5 hoặc K)
    std::string entryNode;           // Nút cửa vào được chọn (cho tòa nhà 2 lối)
    std::vector<std::string> path;   // Danh sách mã nút trên lộ trình
    int visitedNodes;                // Số đỉnh đã duyệt (mở/thăm)
    double executionTimeUs;          // Thời gian thực thi (microsecond)

    AStarResult()
        : found(false), totalDistance(0.0), startNode(""), entryNode(""), visitedNodes(0), executionTimeUs(0.0) {}
};

/**
 * @brief Thống kê hiệu năng của một thuật toán cụ thể phục vụ Benchmark.
 */
struct AlgorithmMetric {
    std::string algorithmName;       // Tên thuật toán: "Dijkstra", "A*", "BFS"
    bool found;                      // Tìm thấy đường hay không
    double totalDistance;            // Tổng quãng đường (mét)
    int visitedCount;                // Số đỉnh đã duyệt (visited nodes)
    double executionTimeUs;          // Thời gian chạy (microsecond)
    std::vector<std::string> path;   // Chuỗi lộ trình

    AlgorithmMetric()
        : algorithmName(""), found(false), totalDistance(0.0), visitedCount(0), executionTimeUs(0.0) {}
};

/**
 * @brief Kết quả so sánh Benchmark giữa 3 thuật toán: Dijkstra, A*, BFS.
 */
struct BenchmarkResult {
    std::string startNode;           // Nút xuất phát
    std::string destNode;            // Nút đích
    AlgorithmMetric dijkstra;        // Kết quả Dijkstra
    AlgorithmMetric astar;           // Kết quả A*
    AlgorithmMetric bfs;             // Kết quả BFS
};

/**
 * @brief Lớp AStar cài đặt thuật toán A* và bộ so sánh Benchmark hiệu năng.
 *
 * Hàm Heuristic Euclid:
 *   h(u) = scale * sqrt((x_u - x_v)^2 + (y_u - y_v)^2)
 * Với tọa độ pixel (x, y) trên bản đồ trường UTC2.
 */
class AStar {
public:
    /**
     * @brief Tính khoảng cách Euclid giữa 2 tọa độ pixel.
     *   d = sqrt((x1 - x2)^2 + (y1 - y2)^2)
     */
    static double euclideanDistance(double x1, double y1, double x2, double y2);

    /**
     * @brief Tính giá trị Heuristic h(u) tới đích v trên đồ thị.
     * @param g Đồ thị
     * @param u Mã đỉnh hiện tại
     * @param v Mã đỉnh đích
     * @param scale Hệ số chuyển đổi pixel sang mét (mặc định 0.12 để đảm bảo admissible)
     */
    static double heuristic(const Graph& g, const std::string& u, const std::string& v, double scale = 0.12);

    /**
     * @brief Chạy thuật toán A* tìm đường đi ngắn nhất từ src đến dest.
     * Tự động xử lý tòa nhà có 2 lối vào (E3, E4, E5).
     *
     * @param graph           Đồ thị UTC2
     * @param src             Mã nút xuất phát (ví dụ: "G")
     * @param dest            Mã nút hoặc logical name đích (ví dụ: "K", "E3")
     * @param blocked         Danh sách các cạnh bị chặn
     * @param heuristicScale  Hệ số tỉ lệ Heuristic (mặc định 0.12)
     * @return AStarResult kết quả tìm đường và thống kê số đỉnh duyệt
     */
    static AStarResult run(
        const Graph& graph,
        const std::string& src,
        const std::string& dest,
        const BlockedEdgeList& blocked = BlockedEdgeList(),
        double heuristicScale = 0.12
    );

    /**
     * @brief Thực hiện Benchmark so sánh đồng thời Dijkstra, A* và BFS
     * trên cùng một cặp đỉnh xuất phát - đích đến.
     */
    static BenchmarkResult benchmark(
        const Graph& graph,
        const std::string& src,
        const std::string& dest,
        const BlockedEdgeList& blocked = BlockedEdgeList(),
        double heuristicScale = 0.12
    );

    /**
     * @brief In bảng so sánh Benchmark định dạng đẹp mắt ra console.
     */
    static void printBenchmark(const BenchmarkResult& bench);

    /**
     * @brief Chạy riêng BFS để đo thời gian và số đỉnh duyệt (phục vụ đối sánh).
     */
    static AlgorithmMetric runBFSMetric(
        const Graph& graph,
        const std::string& src,
        const std::string& dest,
        const BlockedEdgeList& blocked = BlockedEdgeList()
    );

    /**
     * @brief Chạy riêng Dijkstra đo thời gian và số đỉnh duyệt (phục vụ đối sánh).
     */
    static AlgorithmMetric runDijkstraMetric(
        const Graph& graph,
        const std::string& src,
        const std::string& dest,
        const BlockedEdgeList& blocked = BlockedEdgeList()
    );

private:
    static const int MAX_VERTICES = 256;
    static const double INF;

    static bool runAStarSingleTarget(
        const Graph& graph,
        const std::string& src,
        const std::string& target,
        const BlockedEdgeList& blocked,
        double heuristicScale,
        AStarResult& outResult
    );

    static void resolveTarget(const std::string& dest, std::string& out1, std::string& out2);
    static void resolveOrigin(const std::string& src, std::string& out1, std::string& out2);

    static void getNodeCoord(const Graph& g, const std::string& name, double& x, double& y);
};

#endif // ASTAR_H

