#include "ScheduleOptimizer.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <unordered_map>

const double ScheduleOptimizer::WALKING_SPEED_MPS = 1.2;
const int ScheduleOptimizer::DEFAULT_BREAK_SEC = 300; // 5 phút

static std::string toUpper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

static std::string trimString(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string ScheduleOptimizer::resolveRoomToNode(const std::string& roomInput) {
    std::string clean = toUpper(trimString(roomInput));
    if (clean.empty()) return "";

    // Bảng quy đổi các alias tên phòng / tên tòa nhà thông dụng
    static const std::unordered_map<std::string, std::string> ROOM_MAP = {
        {"C2", "C2"}, {"NHÀ C2", "C2"}, {"PHÒNG C2", "C2"}, {"V5", "V5"}, {"K", "K"},
        {"C1", "C1"}, {"NHÀ C1", "C1"}, {"HỘI TRƯỜNG C1", "C1"},
        {"C3", "V4_S"}, {"NHÀ C3", "V4_S"}, {"THƯ VIỆN", "V4_S"}, {"V4_S", "V4_S"}, {"V4", "V4"},
        {"E1", "E1"}, {"NHÀ E1", "E1"}, {"KHOA CNTT", "E1"}, {"A2", "E1"},
        {"E3", "E3w"}, {"NHÀ E3", "E3w"}, {"PHÒNG E3", "E3w"}, {"E3W", "E3w"},
        {"E4", "E4w"}, {"NHÀ E4", "E4w"}, {"PHÒNG E4", "E4w"}, {"E4W", "E4w"},
        {"E5", "E5w"}, {"NHÀ E5", "E5w"}, {"PHÒNG E5", "E5w"}, {"E5W", "E5w"},
        {"E6", "E6_W"}, {"NHÀ E6", "E6_W"}, {"E6_W", "E6_W"}, {"PHÒNG E6", "E6_W"},
        {"E9", "D0"}, {"NHÀ E9", "D0"}, {"PHÒNG E9", "D0"}, {"D0", "D0"},
        {"E7", "E7"}, {"NHÀ E7", "E7"},
        {"E8", "E8"}, {"NHÀ E8", "E8"}, {"XƯỞNG THỰC TẬP", "E8"},
        {"E8B", "E8B"}, {"NHÀ E8B", "E8B"}, {"PTC-UTC2", "E8B"},
        {"E10", "E10"}, {"NHÀ E10", "E10"}, {"PHÒNG E10", "E10"},
        {"H2", "H2"}, {"ĐA NĂNG", "H2"}, {"ĐN", "H2"}, {"GIẢNG ĐƯỜNG ĐA NĂNG", "H2"},
        {"CT", "CT"}, {"CĂN TIN", "CT"}, {"CANTEEN", "CT"},
        {"P*", "P*"}, {"NHÀ XE", "P*"}, {"BÃI XE", "P*"},
        {"G", "G"}, {"CỔNG", "G"}, {"CỔNG 451", "G"}, {"CỔNG CHÍNH", "G"}
    };

    auto it = ROOM_MAP.find(clean);
    if (it != ROOM_MAP.end()) {
        return it->second;
    }

    return clean;
}

TransitionResult ScheduleOptimizer::checkTransition(
    const Graph& graph,
    const std::string& prevRoom,
    const std::string& nextRoom,
    int availableBreakSec,
    const BlockedEdgeList& blocked
) {
    TransitionResult res;
    res.fromRoom = prevRoom;
    res.toRoom = nextRoom;
    res.availableBreakSec = availableBreakSec;

    std::string uNode = resolveRoomToNode(prevRoom);
    std::string vNode = resolveRoomToNode(nextRoom);

    res.fromNode = uNode;
    res.toNode = vNode;
    res.fromName = graph.getDisplayName(uNode);
    if (res.fromName.empty()) res.fromName = prevRoom;
    res.toName = graph.getDisplayName(vNode);
    if (res.toName.empty()) res.toName = nextRoom;

    // Trường hợp học cùng một phòng
    if (uNode == vNode && !uNode.empty()) {
        res.distanceMeters = 0.0;
        res.walkingTimeSec = 0;
        res.isAtRisk = false;
        res.status = "[AN TOÀN] CÙNG PHÒNG HỌC";
        res.warningMessage = "Hai tiết học diễn ra tại cùng một phòng/tòa nhà (" + res.fromName + "). Không cần di chuyển.";
        res.path = {uNode};
        return res;
    }

    // Chạy Dijkstra tính quãng đường ngắn nhất
    DijkstraResult dij = Dijkstra::run(graph, uNode, vNode, blocked);

    if (!dij.found) {
        res.distanceMeters = 0.0;
        res.walkingTimeSec = 999999;
        res.isAtRisk = true;
        res.status = "[LỖI LỘ TRÌNH] KHÔNG TÌM THẤY ĐƯỜNG ĐI";
        res.warningMessage = "Không thể tìm thấy lộ trình nối giữa " + res.fromName + " và " + res.toName + " (do bị chặn đường hoặc điểm không hợp lệ).";
        return res;
    }

    if (!dij.startNode.empty()) {
        res.fromNode = dij.startNode;
        std::string dName = graph.getDisplayName(dij.startNode);
        if (!dName.empty()) res.fromName = dName;
    }
    if (!dij.entryNode.empty()) {
        res.toNode = dij.entryNode;
        std::string dName = graph.getDisplayName(dij.entryNode);
        if (!dName.empty()) res.toName = dName;
    }

    res.distanceMeters = dij.totalDistance;
    res.walkingTimeSec = static_cast<int>(std::ceil(dij.totalDistance / WALKING_SPEED_MPS));

    // Lấy chuỗi mã các nút trên lộ trình
    res.path.clear();
    PathStep* step = dij.path;
    while (step != nullptr) {
        res.path.push_back(step->nodeId);
        step = step->next;
    }

    // Đánh giá nguy cơ trễ học: thời gian đi bộ > thời gian giải lao cho phép (mặc định 5 phút = 300 giây)
    if (res.walkingTimeSec > availableBreakSec) {
        res.isAtRisk = true;
        res.status = "[CẢNH BÁO ĐỎ] NGUY CƠ TRỄ HỌC";
        int overtimeSec = res.walkingTimeSec - availableBreakSec;
        res.warningMessage = "[CẢNH BÁO ĐỎ] NGUY CƠ TRỄ HỌC: Quãng đường di chuyển dài " 
                           + std::to_string(static_cast<int>(res.distanceMeters)) + "m, ước tính cần "
                           + std::to_string(res.walkingTimeSec) + " giây để đi bộ (vượt quá thời gian giải lao "
                           + std::to_string(availableBreakSec) + " giây là " + std::to_string(overtimeSec)
                           + " giây). Sinh viên cần chủ động rời lớp sớm hoặc tăng tốc độ di chuyển!";
    } else {
        res.isAtRisk = false;
        res.status = "[AN TOÀN] KỊP GIỜ VÀO LỚP";
        int remainingSec = availableBreakSec - res.walkingTimeSec;
        res.warningMessage = "Lộ trình di chuyển thuận lợi: Khoảng cách " 
                           + std::to_string(static_cast<int>(res.distanceMeters)) + "m, mất khoảng "
                           + std::to_string(res.walkingTimeSec) + " giây đi bộ (dưới 5 phút). Bạn còn dư "
                           + std::to_string(remainingSec) + " giây giải lao.";
    }

    return res;
}

std::vector<TransitionResult> ScheduleOptimizer::checkDailySchedule(
    const Graph& graph,
    const std::vector<ClassPeriod>& schedule,
    int defaultBreakSec,
    const BlockedEdgeList& blocked
) {
    std::vector<TransitionResult> results;
    if (schedule.size() < 2) return results;

    for (size_t i = 0; i + 1 < schedule.size(); ++i) {
        TransitionResult r = checkTransition(graph, schedule[i].room, schedule[i + 1].room, defaultBreakSec, blocked);
        results.push_back(r);
    }

    return results;
}

void ScheduleOptimizer::printTransition(const TransitionResult& result) {
    std::cout << "\n========================================================================\n";
    std::cout << "          KIỂM TRA CHUYỂN TIẾT THỜI KHÓA BIỂU SINH VIÊN\n";
    std::cout << "========================================================================\n";
    std::cout << "  Tiết trước         : " << result.fromRoom << " (" << result.fromName << ")\n";
    std::cout << "  Tiết sau           : " << result.toRoom << " (" << result.toName << ")\n";
    std::cout << "  Khoảng cách đi bộ  : " << std::fixed << std::setprecision(1) << result.distanceMeters << " mét\n";
    std::cout << "  Vận tốc đi bộ      : " << WALKING_SPEED_MPS << " m/s (~4.3 km/h)\n";
    std::cout << "  Thời gian cần đi   : " << result.walkingTimeSec << " giây (" 
              << (result.walkingTimeSec / 60) << " phút " << (result.walkingTimeSec % 60) << " giây)\n";
    std::cout << "  Thời gian giải lao : " << result.availableBreakSec << " giây (5 phút)\n";
    std::cout << "  Trạng thái         : " << result.status << "\n";
    std::cout << "  ────────────────────────────────────────────────────────────────────\n";
    std::cout << "  THÔNG ĐIỆP HỆ THỐNG:\n  " << result.warningMessage << "\n";
    if (!result.path.empty()) {
        std::cout << "  Lộ trình đề xuất   : ";
        for (size_t i = 0; i < result.path.size(); ++i) {
            std::cout << result.path[i] << (i + 1 < result.path.size() ? " -> " : "\n");
        }
    }
    std::cout << "========================================================================\n";
}

void ScheduleOptimizer::printDailyScheduleReport(const std::vector<TransitionResult>& results) {
    std::cout << "\n========================================================================================\n";
    std::cout << "                    BÁO CÁO TOÀN DIỆN THỜI KHÓA BIỂU TRONG NGÀY\n";
    std::cout << "========================================================================================\n";
    std::cout << std::left
              << std::setw(6)  << "Chặng"
              << std::setw(20) << "Phòng trước -> sau"
              << std::setw(15) << "Khoảng cách"
              << std::setw(16) << "Thời gian đi"
              << std::setw(28) << "Đánh giá an toàn"
              << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    int riskCount = 0;
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        std::string legStr = r.fromRoom + " -> " + r.toRoom;
        std::string distStr = std::to_string(static_cast<int>(r.distanceMeters)) + " m";
        std::string timeStr = std::to_string(r.walkingTimeSec) + "s (" + std::to_string(r.walkingTimeSec / 60) + "m" + std::to_string(r.walkingTimeSec % 60) + "s)";

        std::cout << std::left
                  << std::setw(6)  << (std::to_string(i + 1))
                  << std::setw(20) << legStr
                  << std::setw(15) << distStr
                  << std::setw(16) << timeStr
                  << std::setw(28) << r.status
                  << "\n";

        if (r.isAtRisk) ++riskCount;
    }

    std::cout << "----------------------------------------------------------------------------------------\n";
    if (riskCount > 0) {
        std::cout << "  [KẾT LUẬN]: Có " << riskCount << " chặng học có [CẢNH BÁO ĐỎ: NGUY CƠ TRỄ HỌC]!\n";
    } else {
        std::cout << "  [KẾT LUẬN]: Tất cả các chặng học đều AN TOÀN, kịp giờ trong khung nghỉ 5 phút.\n";
    }
    std::cout << "========================================================================================\n";
}

