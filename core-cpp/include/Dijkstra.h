#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "Graph.h"
#include "MinHeap.h"
#include <string>
#include <iostream>

// ============================================================
//  Cấu trúc dữ liệu thuần cho tập cạnh bị chặn (Blocked Edges)
//  Dùng danh sách liên kết đơn, không dùng std::set / std::vector
// ============================================================

/**
 * @brief Một cạnh bị chặn (không thể đi qua).
 * Cặp (u, v) — vô hướng, thuật toán kiểm tra cả (u→v) lẫn (v→u).
 */
struct BlockedEdge {
    std::string u;
    std::string v;
    BlockedEdge* next;

    BlockedEdge(const std::string& a, const std::string& b, BlockedEdge* n = nullptr)
        : u(a), v(b), next(n) {}
};

/**
 * @brief Danh sách liên kết đơn quản lý tập cạnh bị chặn.
 * Quản lý bộ nhớ theo Rule of Three.
 */
class BlockedEdgeList {
private:
    BlockedEdge* head;
    int count;

    void freeMemory();
    void copyFrom(const BlockedEdgeList& other);

public:
    BlockedEdgeList();
    ~BlockedEdgeList();
    BlockedEdgeList(const BlockedEdgeList& other);
    BlockedEdgeList& operator=(const BlockedEdgeList& other);

    // Thêm một cạnh bị chặn
    void block(const std::string& u, const std::string& v);

    // Gỡ chặn một cạnh
    void unblock(const std::string& u, const std::string& v);

    // Kiểm tra cạnh (u,v) có bị chặn không (vô hướng)
    bool isBlocked(const std::string& u, const std::string& v) const;

    // Xóa toàn bộ
    void clear();

    // Số cạnh đang bị chặn
    int getCount() const;
};

// ============================================================
//  Cấu trúc kết quả trả về của Dijkstra
// ============================================================

/**
 * @brief Một bước trong lộ trình di chuyển.
 */
struct PathStep {
    std::string nodeId;          // Mã nút (ví dụ: "G", "T", "K")
    std::string displayName;     // Tên hiển thị tiếng Việt
    double cumulativeDist;       // Khoảng cách tích lũy từ điểm xuất phát (mét)
    double stepDist;             // Khoảng cách của đoạn này (mét)
    PathStep* next;

    PathStep(const std::string& id, const std::string& dName,
             double cumDist, double step, PathStep* n = nullptr)
        : nodeId(id), displayName(dName), cumulativeDist(cumDist), stepDist(step), next(n) {}
};

/**
 * @brief Kết quả đầy đủ của một truy vấn Dijkstra.
 * Sở hữu vùng nhớ của chuỗi PathStep — giải phóng trong destructor.
 */
struct DijkstraResult {
    bool found;                  // Có tìm được đường không
    double totalDistance;        // Tổng khoảng cách (mét)
    std::string startNode;       // Nút xuất phát thực tế được chọn (hỗ trợ đa sảnh C2: V5 hoặc K)
    std::string entryNode;       // Nút cửa vào được chọn (cho các tòa nhà 2 lối)
    PathStep* path;              // Danh sách các bước lộ trình (từ src đến dest)
    int stepCount;               // Số bước trong lộ trình

    DijkstraResult();
    ~DijkstraResult();

    // Rule of Three
    DijkstraResult(const DijkstraResult& other);
    DijkstraResult& operator=(const DijkstraResult& other);

    // In lộ trình ra console
    void print(const std::string& srcId = "", const std::string& destId = "") const;

private:
    void freeMemory();
    void copyFrom(const DijkstraResult& other);
};

// ============================================================
//  Lớp chính: Dijkstra
// ============================================================

/**
 * @brief Hiện thực giải thuật Dijkstra tìm đường ngắn nhất trên đồ thị UTC2.
 *
 * Tính năng nổi bật:
 *  1. Hỗ trợ danh sách cạnh bị chặn (blocked edges) — bỏ qua cạnh trong quá trình relaxation.
 *  2. Tự động chọn cửa vào gần nhất cho các tòa nhà có 2 lối:
 *       - "E3": so sánh E3w (lối Tây) và D3 (lối Đông)
 *       - "E4": so sánh E4w (lối Tây) và D2 (lối Đông)
 *       - "E5": so sánh E5w (lối Tây) và D1 (lối Đông)
 *  3. Trả về DijkstraResult chứa đầy đủ danh sách bước lộ trình.
 */
class Dijkstra {
public:
    /**
     * @brief Tìm đường ngắn nhất từ src đến dest trên đồ thị graph.
     *
     * @param graph       Đồ thị UTC2 đã nạp dữ liệu
     * @param src         Mã nút xuất phát (ví dụ: "G")
     * @param dest        Mã nút đích trực tiếp (ví dụ: "K")
     *                    Hoặc logical name tòa nhà: "E3", "E4", "E5"
     * @param blocked     Danh sách cạnh bị chặn (có thể rỗng)
     * @return DijkstraResult chứa lộ trình và tổng khoảng cách
     */
    static DijkstraResult run(
        const Graph& graph,
        const std::string& src,
        const std::string& dest,
        const BlockedEdgeList& blocked = BlockedEdgeList(),
        bool isMultiStop = false
    );

private:
    // Kích thước tối đa mảng nội bộ (bằng số đỉnh tối đa dự kiến)
    static const int MAX_VERTICES = 256;
    static const double INF;

    /**
     * @brief Hạt nhân thuật toán Dijkstra — chạy trên một đỉnh đích cụ thể.
     * @param graph     Đồ thị
     * @param src       Mã nút xuất phát
     * @param target    Mã nút đích thực sự (đã giải quyết dual-entry)
     * @param blocked   Cạnh bị chặn
     * @param distOut   [out] Mảng khoảng cách (phải được cấp phát ngoài, kích thước numVertices)
     * @param prevOut   [out] Mảng chỉ số nút cha (phải được cấp phát ngoài, kích thước numVertices)
     * @param isMultiStop Nếu true, không áp dụng các ràng buộc phân luồng đơn điểm
     * @return true nếu tìm thấy đường đến target
     */
    static bool runCore(
        const Graph& graph,
        const std::string& src,
        const std::string& target,
        const BlockedEdgeList& blocked,
        double* distOut,
        int* prevOut,
        bool isMultiStop = false
    );

    /**
     * @brief Truy vết đường đi và xây dựng chuỗi PathStep từ mảng prev[].
     * @param graph     Đồ thị (để lấy displayName)
     * @param targetIdx Chỉ số nút đích
     * @param distArr   Mảng khoảng cách
     * @param prevArr   Mảng nút cha
     * @return Con trỏ đầu danh sách PathStep (cấp phát bằng new, caller phải giải phóng)
     */
    static PathStep* buildPath(
        const Graph& graph,
        int targetIdx,
        const double* distArr,
        const int* prevArr,
        int& outStepCount,
        double* outTotalDist = nullptr
    );

    /**
     * @brief Xử lý logical name của các tòa nhà 2 lối vào.
     * Trả về danh sách (tối đa 2) mã nút vật lý cần kiểm tra.
     * Ví dụ: "E3" => {"E3w", "D3"}; "K" => {"K"}
     * @param dest      Tên đích nhập vào
     * @param out1      [out] Nút vật lý thứ nhất
     * @param out2      [out] Nút vật lý thứ hai (rỗng nếu không có)
     */
    static void resolveTarget(
        const std::string& dest,
        std::string& out1,
        std::string& out2
    );

    /**
     * @brief Xử lý logical name của các tòa nhà 2 sảnh khi làm điểm xuất phát.
     * Ví dụ: "C2" => {"V5", "K"}
     * @param src       Tên điểm xuất phát nhập vào
     * @param out1      [out] Nút sảnh thứ nhất
     * @param out2      [out] Nút sảnh thứ hai (rỗng nếu không có)
     */
    static void resolveOrigin(
        const std::string& src,
        std::string& out1,
        std::string& out2
    );
};

#endif // DIJKSTRA_H

