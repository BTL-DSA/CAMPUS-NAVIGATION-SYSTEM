#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "Graph.h"
#include "Dijkstra.h"
#include <string>
#include <vector>

/**
 * @brief Phân loại hướng rẽ theo góc giữa hai vector cạnh liên tiếp.
 */
enum class TurnAction {
    START,          // Bắt đầu xuất phát
    STRAIGHT,       // Đi thẳng
    SLIGHT_LEFT,    // Rẽ chếch trái
    LEFT,           // Rẽ trái
    SHARP_LEFT,     // Rẽ ngoặt trái
    SLIGHT_RIGHT,   // Rẽ chếch phải
    RIGHT,          // Rẽ phải
    SHARP_RIGHT,    // Rẽ ngoặt phải
    U_TURN,         // Quay đầu
    ARRIVE          // Đã tới đích
};

/**
 * @brief Chi tiết một bước hướng dẫn di chuyển (Turn-by-turn).
 */
struct TurnStep {
    int stepIndex;               // Thứ tự bước (1, 2, 3...)
    std::string fromNode;        // Mã nút hiện tại
    std::string toNode;          // Mã nút tiếp theo
    std::string fromName;        // Tên địa điểm hiện tại
    std::string toName;          // Tên địa điểm tiếp theo
    double distance;             // Khoảng cách đoạn này (mét)
    TurnAction action;           // Loại hành động rẽ
    std::string actionText;      // "Đi thẳng", "Rẽ trái", "Rẽ phải", "Rẽ chếch trái", "Rẽ chếch phải"...
    std::string instruction;     // Lời hướng dẫn thoại chi tiết tiếng Việt
    double angleDeg;             // Góc quay (độ) tính từ tích có hướng (Cross Product)

    TurnStep()
        : stepIndex(0), distance(0.0), action(TurnAction::STRAIGHT), angleDeg(0.0) {}
};

/**
 * @brief Kết quả tìm đường đa điểm (Multi-stop Routing).
 */
struct MultiStopResult {
    bool success;                        // Có tìm được lộ trình hợp lệ qua tất cả các điểm không
    std::vector<std::string> originalStops; // Danh sách điểm nhập vào ban đầu
    std::vector<std::string> orderedStops;  // Thứ tự ghé thăm tối ưu nhất
    std::vector<std::string> fullPath;      // Toàn bộ chuỗi các nút đi qua trên toàn lộ trình
    double totalDistance;                // Tổng quãng đường (mét)
    int walkingTimeSec;                  // Tổng thời gian đi bộ (vận tốc 1.2 m/s)
    std::string algorithmUsed;           // Thuật toán: "Brute-force (Vét cạn)" hoặc "Nearest Neighbor"
    std::vector<DijkstraResult> segments;// Chi tiết từng chặng di chuyển

    MultiStopResult()
        : success(false), totalDistance(0.0), walkingTimeSec(0) {}
};

/**
 * @brief Lớp Navigator cung cấp các tính năng dẫn đường nâng cao:
 *   1. Tìm đường qua nhiều điểm (Multi-stop Routing) tối ưu tổng quãng đường.
 *   2. Sinh hướng dẫn rẽ bằng lời (Turn-by-turn) dựa trên tích có hướng và tọa độ pixel.
 */
class Navigator {
public:
    /**
     * @brief Tìm thứ tự ghé thăm các điểm dừng tối ưu nhất và trả về toàn bộ lộ trình.
     *
     * Quy tắc giải thuật:
     *   - Số điểm <= 5: Dùng thuật toán Vét cạn (Brute-force) duyệt toàn bộ hoán vị để tìm đường ngắn nhất tuyệt đối.
     *   - Số điểm > 5: Dùng thuật toán Tham lam láng giềng gần nhất (Nearest Neighbor) để tối ưu thời gian tính toán.
     *
     * @param graph           Đồ thị UTC2
     * @param stops           Danh sách mã nút cần ghé (ví dụ: ["G", "CT", "E7", "E3"])
     * @param fixDestination  Nếu true: giữ cố định điểm cuối cùng là đích kết thúc;
     *                        Nếu false: điểm đầu tiên là điểm xuất phát, các điểm còn lại được tự do sắp xếp tối ưu.
     * @param blocked         Danh sách các cạnh bị chặn
     */
    static MultiStopResult findMultiStopPath(
        const Graph& graph,
        const std::vector<std::string>& stops,
        bool fixDestination = false,
        const BlockedEdgeList& blocked = BlockedEdgeList()
    );

    /**
     * @brief Sinh danh sách các bước chỉ đường Turn-by-turn dựa trên tích có hướng (Cross Product).
     *
     * @param graph Đồ thị UTC2 (chứa tên hiển thị và tọa độ pixel x, y)
     * @param path  Mảng mã các nút trên lộ trình (ví dụ: ["G", "T", "J", "K"])
     * @return std::vector<TurnStep> danh sách từng bước hướng dẫn cụ thể
     */
    static std::vector<TurnStep> generateDirections(
        const Graph& graph,
        const std::vector<std::string>& path
    );

    /**
     * @brief In danh sách hướng dẫn Turn-by-turn ra console một cách rõ ràng, trực quan.
     */
    static void printDirections(const std::vector<TurnStep>& directions);

    /**
     * @brief In tóm tắt kết quả lộ trình đa điểm ra console.
     */
    static void printMultiStopResult(const MultiStopResult& res);

    /**
     * @brief Tính tích có hướng (Cross Product) giữa 2 vector liên tiếp AB và BC.
     *   u = B - A; v = C - B
     *   cross = u.x * v.y - u.y * v.x
     */
    static double crossProduct(double ax, double ay, double bx, double by, double cx, double cy);

    /**
     * @brief Xác định hướng rẽ và góc quay (độ) giữa 2 vector liên tiếp.
     */
    static TurnAction determineTurnAction(
        double ax, double ay,
        double bx, double by,
        double cx, double cy,
        double& outAngleDeg
    );

private:
    static void getNodeCoord(const Graph& g, const std::string& name, double& x, double& y);
};

#endif // NAVIGATOR_H

