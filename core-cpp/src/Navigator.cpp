#include "Navigator.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <unordered_map>

// Bảng tọa độ dự phòng đầy đủ 48 nút chuẩn hóa theo ảnh map-utc2.png (1451 x 1084)
static const std::unordered_map<std::string, std::pair<double, double>> FALLBACK_COORDS = {
    {"G",   {200.0, 980.0}}, {"T",   {200.0, 840.0}}, {"W1",  {200.0, 630.0}},
    {"W2",  {200.0, 492.0}}, {"W3",  {200.0, 315.0}}, {"W4",  {200.0, 110.0}},
    {"J",   {430.0, 840.0}}, {"K",   {580.0, 840.0}}, {"C2",  {580.0, 745.0}}, {"C1_J",{780.0, 840.0}}, {"C1_Front", {780.0, 840.0}}, {"C1", {890.0, 780.0}}, {"C1_East", {890.0, 780.0}},
    {"V1",  {430.0, 745.0}}, {"C2_Front",{615.0, 745.0}}, {"V5",  {665.0, 745.0}}, {"V3",  {780.0, 745.0}},
    {"V4",  {920.0, 745.0}}, {"V4_S",{920.0, 840.0}}, {"C3",  {970.0, 710.0}}, {"CT",  {920.0, 710.0}}, {"H1",  {405.0, 630.0}},
    {"H2",  {665.0, 630.0}}, {"DN",  {665.0, 600.0}}, {"DN_SE", {820.0, 630.0}}, {"DN_East", {820.0, 560.0}}, {"DN_NE", {820.0, 492.0}}, {"H3",  {920.0, 630.0}}, {"HP",  {1040.0, 630.0}}, {"HE8B",{1225.0, 630.0}},
    {"P*",  {1040.0, 560.0}},{"PZ",  {1040.0, 560.0}},{"E8B", {1225.0, 590.0}},{"E8",  {1130.0, 342.0}},
    {"S1",  {405.0, 492.0}}, {"S2",  {575.0, 492.0}}, {"Q",   {705.0, 492.0}},
    {"E7",  {440.0, 315.0}}, {"A1",  {575.0, 110.0}}, {"A2",  {575.0, 315.0}}, {"E1",  {630.0, 315.0}},
    {"XE",  {410.0, 110.0}}, {"N1",  {575.0, 160.0}}, {"E10", {410.0, 70.0}}, {"A0", {575.0, 70.0}}, {"Z6", {705.0, 70.0}},
    {"Z3",  {705.0, 440.0}}, {"E3w", {740.0, 440.0}}, {"Z4",  {705.0, 342.0}},
    {"E4w", {740.0, 342.0}}, {"Z5",  {705.0, 190.0}}, {"E5w", {740.0, 190.0}},
    {"R2",  {920.0, 492.0}}, {"D3",  {920.0, 440.0}}, {"X1",  {920.0, 342.0}},
    {"D2",  {920.0, 315.0}}, {"D1",  {920.0, 190.0}}, {"D0",  {920.0, 70.0}},
    {"E6_W",{575.0, 35.0}}
};

void Navigator::getNodeCoord(const Graph& g, const std::string& name, double& x, double& y) {
    x = g.getX(name);
    y = g.getY(name);
    if (x == 0.0 && y == 0.0) {
        auto it = FALLBACK_COORDS.find(name);
        if (it != FALLBACK_COORDS.end()) {
            x = it->second.first;
            y = it->second.second;
        }
    }
}

double Navigator::crossProduct(double ax, double ay, double bx, double by, double cx, double cy) {
    double ux = bx - ax;
    double uy = by - ay;
    double vx = cx - bx;
    double vy = cy - by;
    return ux * vy - uy * vx;
}

TurnAction Navigator::determineTurnAction(
    double ax, double ay,
    double bx, double by,
    double cx, double cy,
    double& outAngleDeg
) {
    double ux = bx - ax;
    double uy = by - ay;
    double vx = cx - bx;
    double vy = cy - by;

    double cross = ux * vy - uy * vx;
    double dot = ux * vx + uy * vy;

    double rad = std::atan2(cross, dot);
    const double PI = 3.14159265358979323846;
    double deg = rad * (180.0 / PI);
    outAngleDeg = deg;

    // Trong hệ tọa độ màn hình (trục Y hướng xuống dưới):
    // deg > 0 là rẽ phải (quay thuận chiều kim đồng hồ)
    // deg < 0 là rẽ trái (quay ngược chiều kim đồng hồ)
    if (std::abs(deg) <= 25.0) {
        return TurnAction::STRAIGHT;
    } else if (deg > 0) {
        if (deg <= 65.0)       return TurnAction::SLIGHT_RIGHT;
        else if (deg <= 125.0) return TurnAction::RIGHT;
        else if (deg <= 165.0) return TurnAction::SHARP_RIGHT;
        else                   return TurnAction::U_TURN;
    } else {
        if (deg >= -65.0)       return TurnAction::SLIGHT_LEFT;
        else if (deg >= -125.0) return TurnAction::LEFT;
        else if (deg >= -165.0) return TurnAction::SHARP_LEFT;
        else                    return TurnAction::U_TURN;
    }
}

static std::string formatDistance(double m) {
    int val = static_cast<int>(std::round(m));
    return std::to_string(val) + "m";
}

std::vector<TurnStep> Navigator::generateDirections(
    const Graph& graph,
    const std::vector<std::string>& path
) {
    std::vector<TurnStep> result;
    if (path.empty()) return result;

    if (path.size() == 1) {
        TurnStep s;
        s.stepIndex = 1;
        s.fromNode = path[0];
        s.toNode = path[0];
        s.fromName = graph.getDisplayName(path[0]);
        s.toName = s.fromName;
        s.distance = 0.0;
        s.action = TurnAction::ARRIVE;
        s.actionText = "Đến nơi";
        s.instruction = "Bạn đang ở ngay " + s.fromName + "!";
        s.angleDeg = 0.0;
        result.push_back(s);
        return result;
    }

    int stepNum = 1;

    // Bước 1: Xuất phát từ node đầu tiên sang node thứ hai
    {
        TurnStep step1;
        step1.stepIndex = stepNum++;
        step1.fromNode = path[0];
        step1.toNode = path[1];
        step1.fromName = graph.getDisplayName(path[0]);
        step1.toName = graph.getDisplayName(path[1]);
        step1.distance = graph.getEdgeWeight(path[0], path[1]);
        if (step1.distance < 0) step1.distance = 0;
        step1.action = TurnAction::START;
        step1.actionText = "Xuất phát";
        step1.instruction = "Từ " + step1.fromName + " xuất phát đi đến " + step1.toName + " (" + formatDistance(step1.distance) + ")";
        step1.angleDeg = 0.0;
        result.push_back(step1);
    }

    // Các bước rẽ tiếp theo dựa trên 3 đỉnh liên tiếp: path[i-1] -> path[i] -> path[i+1]
    for (size_t i = 1; i + 1 < path.size(); ++i) {
        std::string a = path[i - 1];
        std::string b = path[i];
        std::string c = path[i + 1];

        double ax = 0, ay = 0, bx = 0, by = 0, cx = 0, cy = 0;
        getNodeCoord(graph, a, ax, ay);
        getNodeCoord(graph, b, bx, by);
        getNodeCoord(graph, c, cx, cy);

        double angleDeg = 0.0;
        TurnAction act = determineTurnAction(ax, ay, bx, by, cx, cy, angleDeg);

        std::string actText;
        std::string actVerb;
        switch (act) {
            case TurnAction::STRAIGHT:
                actText = "Đi thẳng";
                actVerb = "đi thẳng";
                break;
            case TurnAction::SLIGHT_RIGHT:
                actText = "Rẽ chếch phải";
                actVerb = "rẽ chếch sang phải";
                break;
            case TurnAction::RIGHT:
                actText = "Rẽ phải";
                actVerb = "rẽ phải";
                break;
            case TurnAction::SHARP_RIGHT:
                actText = "Rẽ ngoặt phải";
                actVerb = "rẽ ngoặt sang phải";
                break;
            case TurnAction::SLIGHT_LEFT:
                actText = "Rẽ chếch trái";
                actVerb = "rẽ chếch sang trái";
                break;
            case TurnAction::LEFT:
                actText = "Rẽ trái";
                actVerb = "rẽ trái";
                break;
            case TurnAction::SHARP_LEFT:
                actText = "Rẽ ngoặt trái";
                actVerb = "rẽ ngoặt sang trái";
                break;
            case TurnAction::U_TURN:
                actText = "Quay đầu";
                actVerb = "quay đầu xe/đi ngược lại";
                break;
            default:
                actText = "Đi tiếp";
                actVerb = "tiếp tục đi";
                break;
        }

        TurnStep step;
        step.stepIndex = stepNum++;
        step.fromNode = b;
        step.toNode = c;
        step.fromName = graph.getDisplayName(b);
        step.toName = graph.getDisplayName(c);
        step.distance = graph.getEdgeWeight(b, c);
        if (step.distance < 0) step.distance = 0;
        step.action = act;
        step.actionText = actText;
        step.angleDeg = angleDeg;
        step.instruction = "Tại " + step.fromName + ", " + actVerb + " sang " + step.toName + " (" + formatDistance(step.distance) + ")";
        result.push_back(step);
    }

    // Bước cuối: Thông báo đã đến đích
    {
        TurnStep finalStep;
        finalStep.stepIndex = stepNum++;
        finalStep.fromNode = path.back();
        finalStep.toNode = path.back();
        finalStep.fromName = graph.getDisplayName(path.back());
        finalStep.toName = finalStep.fromName;
        finalStep.distance = 0.0;
        finalStep.action = TurnAction::ARRIVE;
        finalStep.actionText = "Đến nơi";
        finalStep.instruction = "Đã đến điểm đích: " + finalStep.fromName + "!";
        finalStep.angleDeg = 0.0;
        result.push_back(finalStep);
    }

    return result;
}

void Navigator::printDirections(const std::vector<TurnStep>& directions) {
    std::cout << "\n========================================================================\n";
    std::cout << "          HƯỚNG DẪN LỘ TRÌNH TỪNG BƯỚC (TURN-BY-TURN VOICE)\n";
    std::cout << "========================================================================\n";
    for (const auto& step : directions) {
        if (step.action == TurnAction::START) {
            std::cout << "  [BƯỚC " << step.stepIndex << "] " << step.instruction << "\n";
        } else if (step.action == TurnAction::ARRIVE) {
            std::cout << "  [ĐÍCH ĐẾN] " << step.instruction << "\n";
        } else {
            std::cout << "  [BƯỚC " << step.stepIndex << "] (" << step.actionText << ") " 
                      << step.instruction << " [Góc: " << std::fixed << std::setprecision(1) << step.angleDeg << "°]\n";
        }
    }
    std::cout << "========================================================================\n";
}

MultiStopResult Navigator::findMultiStopPath(
    const Graph& graph,
    const std::vector<std::string>& stops,
    bool fixDestination,
    const BlockedEdgeList& blocked
) {
    MultiStopResult res;
    res.originalStops = stops;

    if (stops.size() < 2) {
        res.success = false;
        return res;
    }

    std::string origin = stops[0];
    std::vector<std::string> bestOrder;
    double bestDistance = 1e15;

    // Phân loại giải thuật theo số lượng điểm ghé
    if (stops.size() <= 5) {
        res.algorithmUsed = "Brute-force (Vét cạn tối ưu tuyệt đối)";

        if (fixDestination) {
            std::string dest = stops.back();
            std::vector<std::string> intermediates(stops.begin() + 1, stops.end() - 1);
            std::sort(intermediates.begin(), intermediates.end());

            do {
                std::vector<std::string> currentOrder;
                currentOrder.push_back(origin);
                for (const auto& s : intermediates) currentOrder.push_back(s);
                currentOrder.push_back(dest);

                double totalDist = 0.0;
                bool valid = true;
                for (size_t i = 0; i + 1 < currentOrder.size(); ++i) {
                    DijkstraResult seg = Dijkstra::run(graph, currentOrder[i], currentOrder[i + 1], blocked, true);
                    if (!seg.found) { valid = false; break; }
                    totalDist += seg.totalDistance;
                }

                if (valid && totalDist < bestDistance) {
                    bestDistance = totalDist;
                    bestOrder = currentOrder;
                }
            } while (std::next_permutation(intermediates.begin(), intermediates.end()));

        } else {
            // Điểm xuất phát cố định ở stops[0], các điểm còn lại permute tự do
            std::vector<std::string> remaining(stops.begin() + 1, stops.end());
            std::sort(remaining.begin(), remaining.end());

            do {
                std::vector<std::string> currentOrder;
                currentOrder.push_back(origin);
                for (const auto& s : remaining) currentOrder.push_back(s);

                double totalDist = 0.0;
                bool valid = true;
                for (size_t i = 0; i + 1 < currentOrder.size(); ++i) {
                    DijkstraResult seg = Dijkstra::run(graph, currentOrder[i], currentOrder[i + 1], blocked, true);
                    if (!seg.found) { valid = false; break; }
                    totalDist += seg.totalDistance;
                }

                if (valid && totalDist < bestDistance) {
                    bestDistance = totalDist;
                    bestOrder = currentOrder;
                }
            } while (std::next_permutation(remaining.begin(), remaining.end()));
        }

    } else {
        // Nearest Neighbor (Tham lam láng giềng gần nhất cho số điểm > 5)
        res.algorithmUsed = "Nearest Neighbor (Tham lam láng giềng gần nhất)";

        std::string destFixed = fixDestination ? stops.back() : "";
        std::vector<std::string> pool;
        size_t endIdx = fixDestination ? stops.size() - 1 : stops.size();
        for (size_t i = 1; i < endIdx; ++i) {
            pool.push_back(stops[i]);
        }

        bestOrder.push_back(origin);
        std::string curr = origin;
        double accumulatedDist = 0.0;

        while (!pool.empty()) {
            double minDist = 1e15;
            size_t bestIdx = 0;

            for (size_t i = 0; i < pool.size(); ++i) {
                DijkstraResult seg = Dijkstra::run(graph, curr, pool[i], blocked, true);
                if (seg.found && seg.totalDistance < minDist) {
                    minDist = seg.totalDistance;
                    bestIdx = i;
                }
            }

            accumulatedDist += minDist;
            curr = pool[bestIdx];
            bestOrder.push_back(curr);
            pool.erase(pool.begin() + bestIdx);
        }

        if (fixDestination) {
            DijkstraResult lastSeg = Dijkstra::run(graph, curr, destFixed, blocked, true);
            accumulatedDist += lastSeg.totalDistance;
            bestOrder.push_back(destFixed);
        }

        bestDistance = accumulatedDist;

        // 2-Opt local search để tối ưu hóa thứ tự các điểm ghé, tránh zic-zắc
        auto calcRouteDistance = [&](const std::vector<std::string>& r) -> double {
            double sum = 0.0;
            for (size_t k = 0; k + 1 < r.size(); ++k) {
                DijkstraResult seg = Dijkstra::run(graph, r[k], r[k + 1], blocked, true);
                if (!seg.found) return 1e15;
                sum += seg.totalDistance;
            }
            return sum;
        };

        bool improved = true;
        size_t n = bestOrder.size();
        size_t maxJ = fixDestination ? n - 1 : n;
        int iterCnt = 0;
        while (improved && iterCnt < 50) {
            improved = false;
            iterCnt++;
            for (size_t i = 1; i + 1 < maxJ; ++i) {
                for (size_t j = i + 1; j < maxJ; ++j) {
                    std::vector<std::string> candidate = bestOrder;
                    std::reverse(candidate.begin() + i, candidate.begin() + j + 1);
                    double candDist = calcRouteDistance(candidate);
                    if (candDist < bestDistance - 1e-4) {
                        bestOrder = candidate;
                        bestDistance = candDist;
                        improved = true;
                        break;
                    }
                }
                if (improved) break;
            }
        }
        res.algorithmUsed = "Nearest Neighbor + 2-Opt (Tối ưu hóa hành trình)";
    }

    if (bestOrder.empty() || bestDistance >= 1e14) {
        res.success = false;
        return res;
    }

    res.success = true;
    res.orderedStops = bestOrder;
    res.totalDistance = bestDistance;
    res.walkingTimeSec = static_cast<int>(std::ceil(bestDistance / 1.2));

    // Tổng hợp toàn bộ các nút trên lộ trình (fullPath) và lưu chi tiết từng chặng
    res.fullPath.clear();
    res.segments.clear();

    for (size_t i = 0; i + 1 < bestOrder.size(); ++i) {
        DijkstraResult seg = Dijkstra::run(graph, bestOrder[i], bestOrder[i + 1], blocked, true);
        res.segments.push_back(seg);

        // Duyệt chuỗi PathStep của segment
        PathStep* currStep = seg.path;
        bool isFirstLeg = (i == 0);

        while (currStep != nullptr) {
            // Nếu không phải chặng đầu, bỏ qua nút đầu tiên của chặng để tránh trùng lặp
            if (!isFirstLeg && currStep == seg.path) {
                currStep = currStep->next;
                continue;
            }
            res.fullPath.push_back(currStep->nodeId);
            currStep = currStep->next;
        }
    }

    return res;
}

void Navigator::printMultiStopResult(const MultiStopResult& res) {
    std::cout << "\n========================================================================\n";
    std::cout << "         KẾT QUẢ ĐIỀU HƯỚNG ĐA ĐIỂM (MULTI-STOP ROUTING)\n";
    std::cout << "========================================================================\n";
    std::cout << "  Thuật toán áp dụng : " << res.algorithmUsed << "\n";
    std::cout << "  Điểm ban đầu       : ";
    for (size_t i = 0; i < res.originalStops.size(); ++i) {
        std::cout << res.originalStops[i] << (i + 1 < res.originalStops.size() ? " -> " : "\n");
    }
    std::cout << "  Thứ tự tối ưu nhất : ";
    for (size_t i = 0; i < res.orderedStops.size(); ++i) {
        std::cout << res.orderedStops[i] << (i + 1 < res.orderedStops.size() ? " -> " : "\n");
    }
    std::cout << "  Tổng quãng đường   : " << res.totalDistance << " mét\n";
    std::cout << "  Thời gian đi bộ    : " << res.walkingTimeSec << " giây (~" 
              << (res.walkingTimeSec / 60) << " phút " << (res.walkingTimeSec % 60) << " giây)\n";
    std::cout << "  Toàn bộ chuỗi nút  : ";
    for (size_t i = 0; i < res.fullPath.size(); ++i) {
        std::cout << res.fullPath[i] << (i + 1 < res.fullPath.size() ? " -> " : "\n");
    }
    std::cout << "========================================================================\n";
}

