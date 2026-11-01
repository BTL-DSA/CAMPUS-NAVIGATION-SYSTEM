#ifndef SCHEDULE_OPTIMIZER_H
#define SCHEDULE_OPTIMIZER_H

#include "Graph.h"
#include "Dijkstra.h"
#include <string>
#include <vector>

/**
 * @brief Thông tin một tiết học trong thời khóa biểu sinh viên.
 */
struct ClassPeriod {
    int periodNumber;            // Tiết học (ví dụ: 1, 2, 3, 4, 5...)
    std::string subjectName;     // Tên môn học (ví dụ: "Cấu trúc dữ liệu & Giải thuật")
    std::string room;            // Phòng học hoặc tòa nhà (ví dụ: "C2", "E3", "E8", "E7")
    std::string timeSlot;        // Khung giờ (ví dụ: "07:00 - 09:15")

    ClassPeriod() : periodNumber(0), subjectName(""), room(""), timeSlot("") {}
    ClassPeriod(int num, const std::string& subj, const std::string& r, const std::string& time = "")
        : periodNumber(num), subjectName(subj), room(r), timeSlot(time) {}
};

/**
 * @brief Kết quả kiểm tra chuyển tiếp giữa 2 tiết học.
 */
struct TransitionResult {
    std::string fromRoom;        // Phòng học tiết trước
    std::string toRoom;          // Phòng học tiết sau
    std::string fromNode;        // Mã nút đồ thị xuất phát
    std::string toNode;          // Mã nút đồ thị đích đến
    std::string fromName;        // Tên đầy đủ phòng trước
    std::string toName;          // Tên đầy đủ phòng sau
    double distanceMeters;       // Quãng đường đi bộ (mét)
    int walkingTimeSec;          // Thời gian đi bộ tính theo vận tốc 1.2 m/s (giây)
    int availableBreakSec;       // Thời gian nghỉ giữa tiết cho phép (mặc định 300 giây / 5 phút)
    bool isAtRisk;               // true nếu walkingTimeSec > availableBreakSec
    std::string status;          // Nhãn trạng thái: "AN TOÀN" hoặc "NGUY CƠ TRỄ HỌC"
    std::string warningMessage;  // Chi tiết cảnh báo đỏ hoặc thông báo an toàn
    std::vector<std::string> path; // Lộ trình di chuyển chi tiết

    TransitionResult()
        : distanceMeters(0.0), walkingTimeSec(0), availableBreakSec(300), isAtRisk(false) {}
};

/**
 * @brief Lớp ScheduleOptimizer tối ưu và cảnh báo lộ trình thời khóa biểu sinh viên UTC2.
 * Vận tốc đi bộ chuẩn: 1.2 m/s.
 * Ngưỡng cảnh báo trễ học: 5 phút (300 giây).
 */
class ScheduleOptimizer {
public:
    static const double WALKING_SPEED_MPS; // 1.2 m/s
    static const int DEFAULT_BREAK_SEC;    // 300 giây (5 phút)

    /**
     * @brief Chuyển đổi tên phòng/tên tòa nhà thông dụng sang mã nút đồ thị chuẩn UTC2.
     * Ví dụ: "C2" -> "K", "E1" -> "A2", "Đa năng" -> "H2", "Căn tin" -> "CT"
     */
    static std::string resolveRoomToNode(const std::string& roomInput);

    /**
     * @brief Kiểm tra việc di chuyển giữa phòng học Tiết trước và Tiết sau.
     *
     * @param graph              Đồ thị khuôn viên UTC2
     * @param prevRoom           Phòng học Tiết trước (ví dụ: "C2", "E7", "Nhà C2")
     * @param nextRoom           Phòng học Tiết sau (ví dụ: "E8", "E3", "Căn tin")
     * @param availableBreakSec  Thời gian giải lao giữa 2 tiết (mặc định 300 giây = 5 phút)
     * @param blocked            Danh sách các đoạn đường bị chặn (nếu có)
     * @return TransitionResult  Kết quả phân tích kèm cảnh báo nếu vượt quá 5 phút
     */
    static TransitionResult checkTransition(
        const Graph& graph,
        const std::string& prevRoom,
        const std::string& nextRoom,
        int availableBreakSec = 300,
        const BlockedEdgeList& blocked = BlockedEdgeList()
    );

    /**
     * @brief Kiểm tra toàn bộ thời khóa biểu một ngày học của sinh viên.
     */
    static std::vector<TransitionResult> checkDailySchedule(
        const Graph& graph,
        const std::vector<ClassPeriod>& schedule,
        int defaultBreakSec = 300,
        const BlockedEdgeList& blocked = BlockedEdgeList()
    );

    /**
     * @brief In kết quả kiểm tra chuyển tiếp ra console với cảnh báo màu sắc rõ ràng.
     */
    static void printTransition(const TransitionResult& result);

    /**
     * @brief In tổng quan kiểm tra toàn bộ lịch học trong ngày.
     */
    static void printDailyScheduleReport(const std::vector<TransitionResult>& results);
};

#endif // SCHEDULE_OPTIMIZER_H

