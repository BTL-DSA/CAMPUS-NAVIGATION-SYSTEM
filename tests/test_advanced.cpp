/**
 * @file test_advanced.cpp
 * @brief Kiểm thử các tính năng nâng cao trong C++ Core của Hệ thống Dẫn đường UTC2:
 *   1. Navigator: Multi-stop Routing (Brute-force <= 5 điểm, Nearest Neighbor > 5 điểm)
 *   2. Navigator: Turn-by-turn Navigation bằng tích có hướng (Cross Product)
 *   3. ScheduleOptimizer: Tối ưu thời khóa biểu & cảnh báo đỏ "NGUY CƠ TRỄ HỌC" (> 300s)
 *   4. AStar & Benchmark: Thuật toán A* Heuristic Euclid so sánh với Dijkstra và BFS
 *
 * Biên dịch:
 *   g++ -std=c++17 -Wall -Wextra -I core-cpp/include \
 *       core-cpp/src/Graph.cpp core-cpp/src/MinHeap.cpp core-cpp/src/Dijkstra.cpp \
 *       core-cpp/src/Navigator.cpp core-cpp/src/ScheduleOptimizer.cpp core-cpp/src/AStar.cpp \
 *       tests/test_advanced.cpp -o tests/test_advanced.exe
 *
 * Chạy:
 *   .\tests\test_advanced.exe
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>

#include "Graph.h"
#include "MinHeap.h"
#include "Dijkstra.h"
#include "Navigator.h"
#include "ScheduleOptimizer.h"
#include "AStar.h"

static const std::string EDGES_PATH = "core-cpp/data/edges.csv";
static const std::string EDGES_PATH_ALT = "data/edges.csv";

static bool g_allPassed = true;

static void printResult(const char* testName, bool passed, const std::string& detail = "") {
    if (passed) {
        std::cout << "  [PASS] " << testName << "\n";
    } else {
        std::cout << "  [FAIL] " << testName;
        if (!detail.empty()) std::cout << " => " << detail;
        std::cout << "\n";
        g_allPassed = false;
    }
}

static bool approxEq(double a, double b, double eps = 1e-4) {
    return std::abs(a - b) < eps;
}

static bool loadGraph(Graph& g) {
    bool ok = g.loadFromCSV(EDGES_PATH);
    if (!ok) ok = g.loadFromCSV(EDGES_PATH_ALT);
    return ok;
}

// ============================================================================
//  NHÓM 1: MULTI-STOP ROUTING (ĐA ĐIỂM)
// ============================================================================
static void test_multiStopRouting() {
    std::cout << "\n[TEST NHÓM 1] Multi-stop Routing (Brute-force & Nearest Neighbor)\n";
    Graph g;
    loadGraph(g);

    // Kịch bản 1.1: 4 điểm ghé (<= 5 điểm) -> Dùng Brute-force
    // Xuất phát G -> ghé H3, E7, kết thúc ở E3
    {
        std::vector<std::string> stops = {"G", "H3", "E7", "E3w"};
        MultiStopResult res = Navigator::findMultiStopPath(g, stops, false);

        printResult("Multi-stop 4 điểm: success", res.success);
        printResult("Multi-stop dùng Brute-force", res.algorithmUsed.find("Brute-force") != std::string::npos);
        printResult("Đủ số điểm ghé (4)", res.orderedStops.size() == 4);
        printResult("Bắt đầu tại G", !res.orderedStops.empty() && res.orderedStops[0] == "G");
        printResult("Tổng quãng đường hợp lệ (> 0m)", res.totalDistance > 0.0);
        printResult("Thời gian đi bộ tính đúng (v=1.2 m/s)", 
                    res.walkingTimeSec == static_cast<int>(std::ceil(res.totalDistance / 1.2)));

        // Kiểm tra tính liên tục của fullPath (mỗi cặp cạnh liên tiếp phải tồn tại trong đồ thị)
        bool pathContinuous = true;
        for (size_t i = 0; i + 1 < res.fullPath.size(); ++i) {
            double w = g.getEdgeWeight(res.fullPath[i], res.fullPath[i + 1]);
            if (w < 0) {
                pathContinuous = false;
                break;
            }
        }
        printResult("Lộ trình fullPath liên tục và hợp lệ", pathContinuous);

        Navigator::printMultiStopResult(res);
    }

    // Kịch bản 1.2: > 5 điểm ghé -> Tự động chuyển sang Nearest Neighbor
    {
        std::vector<std::string> stops6 = {"G", "K", "H3", "H2", "E7", "E3w"};
        MultiStopResult res6 = Navigator::findMultiStopPath(g, stops6, false);

        printResult("Multi-stop 6 điểm: success", res6.success);
        printResult("Multi-stop tự động dùng Nearest Neighbor", 
                    res6.algorithmUsed.find("Nearest Neighbor") != std::string::npos);
        printResult("Đủ số điểm ghé (6)", res6.orderedStops.size() == 6);
    }
}

// ============================================================================
//  NHÓM 2: TURN-BY-TURN DIRECTIONS (TÍCH CÓ HƯỚNG CROSS PRODUCT)
// ============================================================================
static void test_turnByTurnDirections() {
    std::cout << "\n[TEST NHÓM 2] Turn-by-turn Navigation (Cross Product)\n";
    Graph g;
    loadGraph(g);

    // Tuyến đường chuẩn: G -> T -> J -> K (Cổng 451 lên Nhà C2)
    // - Từ G lên T: đi hướng Bắc (y giảm từ 980 xuống 878)
    // - Từ T sang J: đi hướng Đông (x tăng từ 145 sang 390)
    // => Tại T, nhìn từ G lên thì J nằm bên tay PHẢI. Cần rẽ PHẢI!
    {
        std::vector<std::string> path = {"G", "T", "J", "K"};
        std::vector<TurnStep> dirs = Navigator::generateDirections(g, path);

        printResult("Số bước hướng dẫn đúng (4 bước)", dirs.size() == 4);
        printResult("Bước 1: Xuất phát từ G", dirs[0].action == TurnAction::START);
        printResult("Bước 2: Rẽ PHẢI tại T sang J", dirs[1].action == TurnAction::RIGHT,
                    "Nhận được: " + dirs[1].actionText + " (" + std::to_string(dirs[1].angleDeg) + "°)");
        printResult("Bước 3: Đi thẳng từ J sang K", 
                    (dirs[2].action == TurnAction::STRAIGHT || dirs[2].action == TurnAction::SLIGHT_RIGHT),
                    "Nhận được: " + dirs[2].actionText);
        printResult("Bước 4: Đến nơi tại K", dirs[3].action == TurnAction::ARRIVE);

        Navigator::printDirections(dirs);
    }

    // Tuyến ngược lại: K -> J -> T -> G
    // Từ J về T đi hướng Tây, tại T rẽ xuống G (hướng Nam)
    // => Tại T, nhìn từ J sang thì G nằm bên tay TRÁI. Cần rẽ TRÁI!
    {
        std::vector<std::string> revPath = {"K", "J", "T", "G"};
        std::vector<TurnStep> revDirs = Navigator::generateDirections(g, revPath);

        printResult("Tuyến ngược lại: Rẽ TRÁI tại T sang G", revDirs[2].action == TurnAction::LEFT,
                    "Nhận được: " + revDirs[2].actionText + " (" + std::to_string(revDirs[2].angleDeg) + "°)");
    }
}

// ============================================================================
//  NHÓM 3: SCHEDULE OPTIMIZER (THỜI KHÓA BIỂU & CẢNH BÁO TRỄ HỌC)
// ============================================================================
static void test_scheduleOptimizer() {
    std::cout << "\n[TEST NHÓM 3] Schedule Optimizer & Cảnh báo đỏ 'NGUY CƠ TRỄ HỌC'\n";
    Graph g;
    loadGraph(g);

    // Kịch bản 3.1: Chuyển phòng gần (Nhà C2 -> Nhà C1)
    // Khoảng cách ngắn (~28m), thời gian đi bộ ~24 giây < 300 giây (5 phút) => AN TOÀN
    {
        TransitionResult rSafe = ScheduleOptimizer::checkTransition(g, "C2", "C1", 300);

        printResult("C2 -> C1: Không có nguy cơ trễ học", !rSafe.isAtRisk);
        printResult("C2 -> C1: Trạng thái AN TOÀN", rSafe.status.find("AN TOÀN") != std::string::npos);
        printResult("C2 -> C1: Thời gian < 300 giây", rSafe.walkingTimeSec < 300,
                    std::to_string(rSafe.walkingTimeSec) + " giây");

        ScheduleOptimizer::printTransition(rSafe);
    }

    // Kịch bản 3.2a: Chuyển phòng G -> E8 với thời gian giải lao nhanh 150 giây (2.5 phút) < 174 giây cần đi
    // => PHẢI CÓ CẢNH BÁO ĐỎ: NGUY CƠ TRỄ HỌC!
    {
        TransitionResult rRisk = ScheduleOptimizer::checkTransition(g, "G", "E8", 150);

        printResult("G -> E8 (nghỉ 150s): BẬT CẢNH BÁO NGUY CƠ TRỄ HỌC", rRisk.isAtRisk);
        printResult("G -> E8: Trạng thái chứa 'NGUY CƠ TRỄ HỌC'", 
                    rRisk.status.find("NGUY CƠ TRỄ HỌC") != std::string::npos);
        printResult("G -> E8: Thông điệp chứa [CẢNH BÁO ĐỎ]",
                    rRisk.warningMessage.find("[CẢNH BÁO ĐỎ]") != std::string::npos);
        printResult("G -> E8: Thời gian đi bộ (174s) vượt quá giờ nghỉ (150s)", rRisk.walkingTimeSec > 150,
                    std::to_string(rRisk.walkingTimeSec) + " giây (" + std::to_string(rRisk.distanceMeters) + "m)");

        ScheduleOptimizer::printTransition(rRisk);
    }

    // Kịch bản 3.2b: Kiểm tra ngưỡng vượt quá 5 phút (300 giây) theo đúng yêu cầu đề bài
    {
        Graph gLong = g;
        gLong.addVertex("SAN_TAP", "Sân tập thể thao", "Khu thể chất mở rộng");
        gLong.addEdge("D0", "SAN_TAP", 200.0); // Tuyến G -> SAN_TAP = 211m + 200m = 411m -> 343 giây > 300 giây

        TransitionResult rOver300 = ScheduleOptimizer::checkTransition(gLong, "G", "SAN_TAP", 300);

        printResult("Tuyến dài > 5 phút (343s): BẬT CẢNH BÁO NGUY CƠ TRỄ HỌC", rOver300.isAtRisk);
        printResult("Thời gian đi bộ vượt quá 5 phút (300s)", rOver300.walkingTimeSec > 300,
                    std::to_string(rOver300.walkingTimeSec) + " giây (" + std::to_string(rOver300.distanceMeters) + "m)");
        printResult("Thông điệp đúng [CẢNH BÁO ĐỎ]", 
                    rOver300.warningMessage.find("[CẢNH BÁO ĐỎ]") != std::string::npos);
    }

    // Kịch bản 3.3: Kiểm tra toàn diện thời khóa biểu 3 tiết trong một ngày
    {
        std::vector<ClassPeriod> daySchedule = {
            ClassPeriod(1, "Giải tích 1", "C2", "07:00 - 09:15"),
            ClassPeriod(2, "Vật lý đại cương", "C1", "09:25 - 11:40"),
            ClassPeriod(3, "Thực hành viễn thông", "E8", "12:30 - 15:45")
        };

        std::vector<TransitionResult> dayReport = ScheduleOptimizer::checkDailySchedule(g, daySchedule, 300);
        printResult("Thời khóa biểu ngày: Đúng 2 chặng chuyển tiếp", dayReport.size() == 2);
        printResult("Chặng 1 (C2->C1) an toàn", !dayReport[0].isAtRisk);

        ScheduleOptimizer::printDailyScheduleReport(dayReport);
    }
}

// ============================================================================
//  NHÓM 4: A* HEURISTIC & BENCHMARK (DIJKSTRA vs A* vs BFS)
// ============================================================================
static void test_aStarAndBenchmark() {
    std::cout << "\n[TEST NHÓM 4] Thuật toán A* Heuristic Euclid & Benchmark Đối sánh\n";
    Graph g;
    loadGraph(g);

    // Kịch bản 4.1: Kiểm tra tính chuẩn xác của A* trên tuyến G -> K (60m)
    {
        AStarResult ares = AStar::run(g, "G", "K");

        printResult("A* tìm thấy đường G -> K", ares.found);
        printResult("A* cho khoảng cách chuẩn xác 60m", approxEq(ares.totalDistance, 60.0),
                    "Nhận được: " + std::to_string(ares.totalDistance) + "m");
        printResult("A* có lộ trình G -> T -> J -> K", 
                    ares.path.size() == 4 && ares.path[0] == "G" && ares.path[3] == "K");
    }

    // Kịch bản 4.2: A* tự động chọn cửa Đông (D3) khi đến Nhà E3 từ Cổng 451 (159m) và cửa Tây E3w (162m)
    {
        AStarResult aresE3 = AStar::run(g, "G", "E3");

        printResult("A* đến E3 tìm thấy đường", aresE3.found);
        printResult("A* đến E3 tự động chọn Lối Đông D3", aresE3.entryNode == "D3");
        printResult("A* đến E3 khoảng cách chuẩn 159m (qua trục đường nhựa phía Đông)", approxEq(aresE3.totalDistance, 159.0),
                    "Nhận được: " + std::to_string(aresE3.totalDistance) + "m");

        AStarResult aresE3w = AStar::run(g, "G", "E3w");
        printResult("A* đến E3w tự động chọn Lối Tây", aresE3w.entryNode == "E3w");
        printResult("A* đến E3w khoảng cách chuẩn 162m (qua Đại lộ Thành Công & Sáng Tạo)", approxEq(aresE3w.totalDistance, 162.0),
                    "Nhận được: " + std::to_string(aresE3w.totalDistance) + "m");
    }

    // Kịch bản 4.3: Chạy Benchmark so sánh hiệu năng giữa Dijkstra, A* và BFS
    {
        BenchmarkResult bench = AStar::benchmark(g, "G", "E3");

        printResult("Benchmark: Dijkstra tìm thấy", bench.dijkstra.found);
        printResult("Benchmark: A* tìm thấy", bench.astar.found);
        printResult("Benchmark: BFS tìm thấy", bench.bfs.found);

        // A* sử dụng hàm Heuristic định hướng đích nên duyệt ít đỉnh hơn hoặc bằng Dijkstra
        printResult("A* duyệt ít đỉnh hơn hoặc bằng Dijkstra (A* <= Dijkstra)", 
                    bench.astar.visitedCount <= bench.dijkstra.visitedCount,
                    "A*: " + std::to_string(bench.astar.visitedCount) + " đỉnh vs Dijkstra: " + std::to_string(bench.dijkstra.visitedCount) + " đỉnh");

        // Quãng đường A* và Dijkstra phải tương đương (khoảng cách tối ưu)
        printResult("Quãng đường A* và Dijkstra trùng khớp", 
                    approxEq(bench.astar.totalDistance, bench.dijkstra.totalDistance));

        // In bảng đối sánh Benchmark trực quan
        AStar::printBenchmark(bench);
    }
}

// ============================================================================
//  MAIN
// ============================================================================
int main() {
    std::cout << "========================================================================\n";
    std::cout << "  KIỂM THỬ CÁC TÍNH NĂNG NÂNG CAO C++ CORE (UTC2 CAMPUS NAVIGATION)\n";
    std::cout << "========================================================================\n";

    test_multiStopRouting();
    test_turnByTurnDirections();
    test_scheduleOptimizer();
    test_aStarAndBenchmark();

    std::cout << "\n========================================================================\n";
    if (g_allPassed) {
        std::cout << "  >>> TẤT CẢ CÁC TEST NÂNG CAO ĐÃ VƯỢT QUA THÀNH CÔNG (PASS)! <<<\n";
    } else {
        std::cout << "  >>> CÓ TEST KHÔNG ĐẠT (FAIL)! VUI LÒNG KIỂM TRA LẠI. <<<\n";
    }
    std::cout << "========================================================================\n";

    return g_allPassed ? 0 : 1;
}
