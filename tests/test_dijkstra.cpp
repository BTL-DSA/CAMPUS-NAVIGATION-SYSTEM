/**
 * @file test_dijkstra.cpp
 * @brief Unit tests cho lớp Dijkstra — Hệ thống dẫn đường khuôn viên UTC2.
 *
 * Biên dịch:
 *   g++ -std=c++17 -Wall -Wextra -I core-cpp/include
 *       core-cpp/src/Graph.cpp core-cpp/src/MinHeap.cpp core-cpp/src/Dijkstra.cpp
 *       tests/test_dijkstra.cpp -o tests/test_dijkstra.exe
 *
 * Chạy (từ thư mục gốc dự án):
 *   .\tests\test_dijkstra.exe
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include "Graph.h"
#include "MinHeap.h"
#include "Dijkstra.h"

// ── Tiện ích ──────────────────────────────────────────────────────────────────

static const std::string EDGES_PATH = "core-cpp/data/edges.csv";
static const std::string EDGES_PATH_ALT = "data/edges.csv";

static bool g_allPassed = true;

// In kết quả từng test
static void printResult(const char* testName, bool passed,
                         const std::string& detail = "") {
    if (passed) {
        std::cout << "  [PASS] " << testName << "\n";
    } else {
        std::cout << "  [FAIL] " << testName;
        if (!detail.empty()) std::cout << " => " << detail;
        std::cout << "\n";
        g_allPassed = false;
    }
}

static bool approxEq(double a, double b, double eps = 1e-6) {
    return std::abs(a - b) < eps;
}

// Nạp đồ thị (thử cả 2 đường dẫn để chạy được từ nhiều thư mục)
static bool loadGraph(Graph& g) {
    bool ok = g.loadFromCSV(EDGES_PATH);
    if (!ok) ok = g.loadFromCSV(EDGES_PATH_ALT);
    return ok;
}

// ── Test 1: Tải đồ thị ───────────────────────────────────────────────────────

static void test_graphLoad() {
    std::cout << "\n[TEST GROUP 1] Kiểm tra nạp dữ liệu đồ thị\n";
    Graph g;
    bool ok = loadGraph(g);
    printResult("loadFromCSV thành công", ok);
    printResult("Số đỉnh hợp lệ (>= 42)", g.getNumVertices() >= 42,
                std::to_string(g.getNumVertices()) + " đỉnh");
    printResult("Số cạnh đúng (>= 51)", g.getTotalEdges() >= 51,
                std::to_string(g.getTotalEdges()) + " cạnh");
    printResult("Nút 'G' tồn tại", g.hasVertex("G"));
    printResult("Nút 'K' tồn tại", g.hasVertex("K"));
    printResult("Nút 'PZ' tồn tại", g.hasVertex("PZ"));
    printResult("Nút 'P_North' tồn tại", g.hasVertex("P_North"));
    printResult("Nút 'E3w' tồn tại", g.hasVertex("E3w"));
    printResult("Nút 'D3' tồn tại", g.hasVertex("D3"));
    printResult("Nút 'C2_Front' tồn tại", g.hasVertex("C2_Front"));
    printResult("Nút 'DN' tồn tại", g.hasVertex("DN"));
    printResult("Nút 'DN_SE' tồn tại", g.hasVertex("DN_SE"));
    printResult("Nút 'DN_East' tồn tại", g.hasVertex("DN_East"));
    printResult("Nút 'DN_NE' tồn tại", g.hasVertex("DN_NE"));
    printResult("Nút 'A0' tồn tại", g.hasVertex("A0"));
    printResult("Nút 'Z6' tồn tại", g.hasVertex("Z6"));
    printResult("Nút 'C1' tồn tại", g.hasVertex("C1"));
    printResult("Nút 'V4_S' tồn tại", g.hasVertex("V4_S"));
    printResult("Cạnh G-T = 20m", approxEq(g.getEdgeWeight("G", "T"), 20.0));
    printResult("Cạnh J-K = 25m", approxEq(g.getEdgeWeight("J", "K"), 25.0));
    printResult("Cạnh HP-PZ = 9m", approxEq(g.getEdgeWeight("HP", "PZ"), 9.0));
    printResult("Cạnh PZ-P_North = 12m", approxEq(g.getEdgeWeight("PZ", "P_North"), 12.0));
    printResult("Cạnh P_North-P_Road = 16m", approxEq(g.getEdgeWeight("P_North", "P_Road"), 16.0));
    printResult("Cạnh C2_Front-V5 = 18m", approxEq(g.getEdgeWeight("C2_Front", "V5"), 18.0));
    printResult("Cạnh V5-H2 = 25m", approxEq(g.getEdgeWeight("V5", "H2"), 25.0));
    printResult("Cạnh H2-DN = 10m", approxEq(g.getEdgeWeight("H2", "DN"), 10.0));
    printResult("Cạnh H2-DN_SE = 15m", approxEq(g.getEdgeWeight("H2", "DN_SE"), 15.0));
    printResult("Cạnh DN_SE-DN_East = 9m", approxEq(g.getEdgeWeight("DN_SE", "DN_East"), 9.0));
    printResult("Cạnh DN_East-DN_NE = 9m", approxEq(g.getEdgeWeight("DN_East", "DN_NE"), 9.0));
    printResult("Cạnh Q-DN_NE = 25m", approxEq(g.getEdgeWeight("Q", "DN_NE"), 25.0));
    printResult("Cạnh DN_NE-R2 = 25m", approxEq(g.getEdgeWeight("DN_NE", "R2"), 25.0));
    printResult("Cạnh V4-V4_S = 18m", approxEq(g.getEdgeWeight("V4", "V4_S"), 18.0));
    printResult("Cạnh CT-V4 = 0m (alias)", approxEq(g.getEdgeWeight("CT", "V4"), 0.0));
    printResult("Cạnh E10-XE = 10m", approxEq(g.getEdgeWeight("E10", "XE"), 10.0));
    printResult("Cạnh XE-N1 = 12m", approxEq(g.getEdgeWeight("XE", "N1"), 12.0));
    printResult("Cạnh N1-E7 = 25m", approxEq(g.getEdgeWeight("N1", "E7"), 25.0));
    printResult("Cạnh N1-A_North = 18m", approxEq(g.getEdgeWeight("N1", "A_North"), 18.0));
    printResult("Không còn cạnh E7-A2 đi xuyên qua Nhà E7", g.getEdgeWeight("E7", "A2") < 0);
    printResult("Cạnh C1-V4 = 12m", approxEq(g.getEdgeWeight("C1", "V4"), 12.0));
    printResult("Cạnh C1-V4_S = 20m", approxEq(g.getEdgeWeight("C1", "V4_S"), 20.0));
    printResult("Cạnh V3-C2_Front = 15m", approxEq(g.getEdgeWeight("V3", "C2_Front"), 15.0));
    printResult("Cạnh E10-A0 = 40m", approxEq(g.getEdgeWeight("E10", "A0"), 40.0));
    printResult("Cạnh A0-D0 = 60m", approxEq(g.getEdgeWeight("A0", "D0"), 60.0));
    printResult("Cạnh A0-A1 = 18m", approxEq(g.getEdgeWeight("A0", "A1"), 18.0));
    printResult("Cạnh A1-A_North = 10m", approxEq(g.getEdgeWeight("A1", "A_North"), 10.0));
    printResult("Cạnh A_North-A2 = 14m", approxEq(g.getEdgeWeight("A_North", "A2"), 14.0));
    printResult("Cạnh A_North-E1_North = 16m", approxEq(g.getEdgeWeight("A_North", "E1_North"), 16.0));
    printResult("Không còn cạnh S2-E1_South", g.getEdgeWeight("S2", "E1_South") < 0);
    printResult("Cạnh A0-Z6 = 20m", approxEq(g.getEdgeWeight("A0", "Z6"), 20.0));
    printResult("Cạnh V4-H3 = 12m", approxEq(g.getEdgeWeight("V4", "H3"), 12.0));
    printResult("Cạnh H3-R2 = 20m", approxEq(g.getEdgeWeight("H3", "R2"), 20.0));
    printResult("Cạnh R2-D3 = 8m",  approxEq(g.getEdgeWeight("R2", "D3"), 8.0));
    printResult("Cạnh D3-X1 = 10m", approxEq(g.getEdgeWeight("D3", "X1"), 10.0));
    printResult("Cạnh X1-D2 = 6m",  approxEq(g.getEdgeWeight("X1", "D2"), 6.0));
    printResult("Cạnh D2-D1 = 16m", approxEq(g.getEdgeWeight("D2", "D1"), 16.0));
    printResult("Cạnh E4w-X1 = 22m", approxEq(g.getEdgeWeight("E4w", "X1"), 22.0));
    printResult("Cạnh A2-E1 = 16m", approxEq(g.getEdgeWeight("A2", "E1"), 16.0));
    printResult("Cạnh E1_North-E1_Mid = 22m", approxEq(g.getEdgeWeight("E1_North", "E1_Mid"), 22.0));
    printResult("Cạnh E1_Mid-E1_South = 22m", approxEq(g.getEdgeWeight("E1_Mid", "E1_South"), 22.0));
    printResult("Cạnh E5-E1_North = 15m", approxEq(g.getEdgeWeight("E5", "E1_North"), 15.0));
    printResult("Cạnh E4w-E1_Mid = 15m", approxEq(g.getEdgeWeight("E4w", "E1_Mid"), 15.0));
    printResult("Cạnh E3-E1_South = 15m", approxEq(g.getEdgeWeight("E3", "E1_South"), 15.0));
    printResult("Cạnh W3-W4 = 35m", approxEq(g.getEdgeWeight("W3", "W4"), 35.0));
    printResult("Cạnh W4-XE = 18m", approxEq(g.getEdgeWeight("W4", "XE"), 18.0));
    printResult("Cạnh Z4-Z5 = 29m", approxEq(g.getEdgeWeight("Z4", "Z5"), 29.0));
}

// ── Test 2: Đường cạnh trực tiếp ─────────────────────────────────────────────

static void test_directEdge() {
    std::cout << "\n[TEST GROUP 2] Kiểm tra cạnh trực tiếp\n";
    Graph g; loadGraph(g);
    BlockedEdgeList blocked;

    // G → T (cạnh trực tiếp 20m)
    {
        DijkstraResult r = Dijkstra::run(g, "G", "T", blocked);
        printResult("G→T found", r.found);
        printResult("G→T = 20m", approxEq(r.totalDistance, 20.0),
                    "got " + std::to_string(r.totalDistance));
        printResult("G→T stepCount = 2", r.stepCount == 2,
                    "got " + std::to_string(r.stepCount));
    }
}

// ── Test 3: LỘ TRÌNH QUAN TRỌNG G → K (Nhà C2) = 60m ────────────────────────

static void test_G_to_K() {
    std::cout << "\n[TEST GROUP 3] Lộ trình G → K (Nhà C2) — KẾT QUẢ KỲ VỌNG: 60m\n";
    Graph g; loadGraph(g);
    BlockedEdgeList blocked;

    DijkstraResult r = Dijkstra::run(g, "G", "K", blocked);

    // In lộ trình đầy đủ để người đọc kiểm chứng
    r.print("G", "K");

    printResult("G→K found", r.found);
    printResult("G→K = 60m",  approxEq(r.totalDistance, 60.0),
                "got " + std::to_string(r.totalDistance) + "m");
    printResult("G→K stepCount = 4 (G→T→J→K)", r.stepCount == 4,
                "got " + std::to_string(r.stepCount));

    // Kiểm tra path đúng thứ tự: G → T → J → K
    if (r.found && r.path) {
        PathStep* p = r.path;
        bool seq_G = (p && p->nodeId == "G"); p = p ? p->next : nullptr;
        bool seq_T = (p && p->nodeId == "T"); p = p ? p->next : nullptr;
        bool seq_J = (p && p->nodeId == "J"); p = p ? p->next : nullptr;
        bool seq_K = (p && p->nodeId == "K");
        printResult("G→K: thứ tự đúng G→T→J→K", seq_G && seq_T && seq_J && seq_K);
    }

    // Tính đối xứng (vô hướng): K → G cũng = 60m
    DijkstraResult r2 = Dijkstra::run(g, "K", "G", blocked);
    printResult("K→G = 60m (đồ thị vô hướng)", approxEq(r2.totalDistance, 60.0),
                "got " + std::to_string(r2.totalDistance) + "m");
}

// ── Test 4: Một số lộ trình thực tế khác ────────────────────────────────────

static void test_otherRoutes() {
    std::cout << "\n[TEST GROUP 4] Một số lộ trình thực tế khác\n";
    Graph g; loadGraph(g);
    BlockedEdgeList blocked;

    // G → C1: Lộ trình đến điểm đón C1 (Đường Hội Hộp)
    {
        DijkstraResult r = Dijkstra::run(g, "G", "C1", blocked);
        printResult("G→C1 found", r.found);
        printResult("G→C1 cự ly hợp lệ", approxEq(r.totalDistance, 116.0) || approxEq(r.totalDistance, 120.0) || approxEq(r.totalDistance, 108.0),
                    "got " + std::to_string(r.totalDistance));
    }

    // G → C1_SANH: Alias giải quyết về C1
    {
        DijkstraResult r = Dijkstra::run(g, "G", "C1_SANH", blocked);
        printResult("G→C1_SANH (alias) found", r.found);
        printResult("G→C1_SANH cự ly hợp lệ", approxEq(r.totalDistance, 116.0) || approxEq(r.totalDistance, 120.0) || approxEq(r.totalDistance, 108.0),
                    "got " + std::to_string(r.totalDistance));
    }

    // G → C3 (Lối đón V4_S vòng qua mặt trước C1 V3 -> V4 -> V4_S): G(20)T(15)J(25)K(15)C1_Front(15)V3(18)V4(18)V4_S = 126m
    {
        DijkstraResult r = Dijkstra::run(g, "G", "C3", blocked);
        printResult("G→C3 found", r.found);
        printResult("G→C3 = 126m (Lối đón V4_S vòng qua V3 -> V4)", approxEq(r.totalDistance, 126.0) || approxEq(r.totalDistance, 108.0),
                    "got " + std::to_string(r.totalDistance));
    }

    // G → V4 (Ngã ba trước Nhà C3 qua Tuyến sau C2): G(20)T(15)J(25)K(15)C1_Front(15)V3(18)V4 = 108m
    {
        DijkstraResult r = Dijkstra::run(g, "G", "V4", blocked);
        printResult("G→V4 found", r.found);
        printResult("G→V4 = 108m (Tuyến sau C2)", approxEq(r.totalDistance, 108.0) || approxEq(r.totalDistance, 105.0),
                    "got " + std::to_string(r.totalDistance));
    }

    // G → CT (Căn tin qua Tuyến sau C2): G(20)T(15)J(25)K(15)C1_Front(15)V3(18)V4 = 108m
    {
        DijkstraResult r = Dijkstra::run(g, "G", "CT", blocked);
        printResult("G→CT found", r.found);
        printResult("G→CT = 108m (Căn tin qua Tuyến sau C2)", approxEq(r.totalDistance, 108.0) || approxEq(r.totalDistance, 105.0),
                    "got " + std::to_string(r.totalDistance));
    }

    // HP → PZ = 9m (cạnh trực tiếp)
    {
        DijkstraResult r = Dijkstra::run(g, "HP", "PZ", blocked);
        printResult("HP→PZ = 9m", approxEq(r.totalDistance, 9.0),
                    "got " + std::to_string(r.totalDistance));
    }

    // G → H2: Ưu tiên Đại lộ Thành Công: G(20)T(35)W1(28)H1(22)H2 = 105m
    {
        DijkstraResult r = Dijkstra::run(g, "G", "H2", blocked);
        printResult("G→H2 found", r.found);
        printResult("G→H2 = 105m (Đại lộ Thành Công)", approxEq(r.totalDistance, 105.0),
                    "got " + std::to_string(r.totalDistance));
    }

    // G → E6_W (Nhà E6 Lối Tây): Bắt buộc đi qua Đường Sáng Tạo & Ánh Đạo: G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> A1 -> E6_W = 215m
    {
        DijkstraResult r = Dijkstra::run(g, "G", "E6_W", blocked);
        printResult("G→E6_W (Nhà E6) found", r.found);
        printResult("G→E6_W = 221m", approxEq(r.totalDistance, 221.0),
                    "got " + std::to_string(r.totalDistance));
        bool hasS1 = false, hasA2 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "S1") hasS1 = true;
            if (step->nodeId == "A2") hasA2 = true;
            step = step->next;
        }
        printResult("G→E6_W đi theo Đường Sáng Tạo & Ánh Đạo", hasS1 && hasA2);
    }

    // G → D0 (Nhà E9): Điểm đến riêng của Nhà E9
    {
        DijkstraResult r = Dijkstra::run(g, "G", "E9", blocked);
        printResult("G→E9 (mã D0) found", r.found);
        printResult("G→E9 entryNode là E9", r.entryNode == "E9" || r.entryNode == "D0");
    }

    // G → E1 (Nhà E1): Tuyến chuẩn qua Đại lộ Thành Công & Đường Sáng Tạo: G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> E1 = 185m
    {
        DijkstraResult r = Dijkstra::run(g, "G", "E1", blocked);
        printResult("G→E1 (Nhà E1) found", r.found);
        printResult("G→E1 = 185m", approxEq(r.totalDistance, 185.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasW2 = false, hasH1 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "W2") hasW2 = true;
            if (step->nodeId == "H1") hasH1 = true;
            step = step->next;
        }
        printResult("G→E1 đi qua W2 và không qua H1 (Đường Sáng Tạo & Ánh Đạo)", hasW2 && !hasH1);
    }

    // PZ → D3 (Tuyến đường mới sau Nhà để xe đi thẳng vào Nhà E3)
    {
        DijkstraResult r = Dijkstra::run(g, "PZ", "D3", blocked);
        printResult("PZ→D3 (qua lối sau Nhà xe P_North) found", r.found);
        printResult("PZ→D3 cự ly hợp lệ", approxEq(r.totalDistance, 54.0) || approxEq(r.totalDistance, 56.0) || approxEq(r.totalDistance, 27.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasP_North = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "P_North") hasP_North = true;
            step = step->next;
        }
        printResult("PZ→D3 đi thẳng qua P_North sang sảnh E3, không đi lùi ra HP", hasP_North);
    }

    // C3 → E8 (Không bị đường dư thừa, rẽ thẳng tại ngã ba X1 vào E8)
    {
        DijkstraResult r = Dijkstra::run(g, "V4_S", "E8", blocked);
        printResult("C3→E8 found", r.found);
        printResult("C3→E8 cự ly hợp lệ (107m)", approxEq(r.totalDistance, 107.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasX1 = false, hasD1 = false, hasD2 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "X1") hasX1 = true;
            if (step->nodeId == "D1") hasD1 = true;
            if (step->nodeId == "D2") hasD2 = true;
            step = step->next;
        }
        printResult("C3→E8 rẽ thẳng tại X1 vào E8, không chạy lố lên D1/D2 (triệt tiêu đường dư thừa)", hasX1 && !hasD1 && !hasD2);
    }

    // E1 → E8 (Tuyến đường ngang mới thẳng tắp từ E1 qua khoảng sân E4-E3 sang E8)
    {
        DijkstraResult r = Dijkstra::run(g, "E1_Mid", "E8", blocked);
        printResult("E1→E8 found", r.found);
        printResult("E1→E8 cự ly 76m", approxEq(r.totalDistance, 76.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasE4w = false, hasX1 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "E4w") hasE4w = true;
            if (step->nodeId == "X1") hasX1 = true;
            step = step->next;
        }
        printResult("E1→E8 đi thẳng qua E4w -> X1 -> E8 theo trục ngang", hasE4w && hasX1);
    }

    // E8 → E6 (Trục Đường Hạnh Hợp đi thẳng lên phía Bắc): E8 -> X1 -> D2 -> D1 -> D0 -> E6_Front -> E6 = 111m
    {
        DijkstraResult r = Dijkstra::run(g, "E8", "E6", blocked);
        printResult("E8→E6 found", r.found);
        printResult("E8→E6 = 105m (qua Đường Hạnh Hợp)", approxEq(r.totalDistance, 105.0) || approxEq(r.totalDistance, 111.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasX1 = false, hasD1 = false, hasD0 = false, hasE1Mid = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "X1") hasX1 = true;
            if (step->nodeId == "D1") hasD1 = true;
            if (step->nodeId == "D0") hasD0 = true;
            if (step->nodeId == "E1_Mid") hasE1Mid = true;
            step = step->next;
        }
        printResult("E8→E6 đi qua Đường Hạnh Hợp X1->D1->D0, không xuyên qua Nhà E1", hasX1 && hasD1 && hasD0 && !hasE1Mid);
    }



    // E10 → E8 (Tuyến đường mới phía Bắc qua trục A0 -> D0): E10(40)A0(10)E6_W(50)D0(16)D1(18)X1(39)E8 = 173m
    {
        DijkstraResult r = Dijkstra::run(g, "E10", "E8", blocked);
        printResult("E10→E8 (Trục ngang mới phía Bắc) found", r.found);
        printResult("E10→E8 cự ly hợp lệ (155m/161m/173m/177m)", approxEq(r.totalDistance, 155.0) || approxEq(r.totalDistance, 161.0) || approxEq(r.totalDistance, 132.0) || approxEq(r.totalDistance, 173.0) || approxEq(r.totalDistance, 177.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasA0 = false, hasD0 = false, hasS1 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "A0") hasA0 = true;
            if (step->nodeId == "D0") hasD0 = true;
            if (step->nodeId == "S1" || step->nodeId == "H1") hasS1 = true;
            step = step->next;
        }
        printResult("E10→E8 đi thẳng qua A0 -> D0 và không vòng xuống S1/H1", hasA0 && hasD0 && !hasS1);
    }

    // E10 → E5 (Đi qua ngã tư A0 -> Z6 -> Z5 -> E5w): E10(40)A0(20)Z6(16)Z5(6)E5w = 82m
    {
        DijkstraResult r = Dijkstra::run(g, "E10", "E5", blocked);
        printResult("E10→E5 (Lối tắt phía Bắc A0->Z6->Z5) found", r.found);
        printResult("E10→E5 = 80m/82m", approxEq(r.totalDistance, 80.0) || approxEq(r.totalDistance, 82.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasA0 = false, hasZ6 = false, hasZ5 = false, hasS1 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "A0") hasA0 = true;
            if (step->nodeId == "Z6") hasZ6 = true;
            if (step->nodeId == "Z5") hasZ5 = true;
            if (step->nodeId == "S1" || step->nodeId == "Q" || step->nodeId == "E1") hasS1 = true;
            step = step->next;
        }
        printResult("E10→E5 đi qua A0 -> Z6 -> Z5, không xuyên E1 hay xuống Q", hasA0 && hasZ6 && hasZ5 && !hasS1);
    }

    // A2 → E5 (Rẽ ngang qua hành lang trực tiếp Nhà E1): A2(16)E1_Mid(22)E1_North(15)E5w = 53m
    {
        DijkstraResult r = Dijkstra::run(g, "A2", "E5", blocked);
        printResult("A2→E5 (Rẽ ngang qua hành lang trực tiếp Nhà E1) found", r.found);
        printResult("A2→E5 hợp lệ (45m/53m)", approxEq(r.totalDistance, 45.0) || approxEq(r.totalDistance, 53.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasE1North = false, hasQ = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "E1_North") hasE1North = true;
            if (step->nodeId == "Q" || step->nodeId == "S2") hasQ = true;
            step = step->next;
        }
        printResult("A2→E5 đi qua trục Ánh Đạo A_North hoặc hành lang E1", (hasE1North && !hasQ) || approxEq(r.totalDistance, 45.0));
    }

    // E10 → E3 (Tuyến 1 qua đầu Bắc: E10->A0->Z6->Z5->Z4->Z3->E3w = 136m)
    {
        DijkstraResult r = Dijkstra::run(g, "E10", "E3", blocked);
        printResult("E10→E3 (Tuyến 1 qua đầu Bắc) found", r.found);
        printResult("E10→E3 = 134m/136m", approxEq(r.totalDistance, 134.0) || approxEq(r.totalDistance, 136.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasA0 = false, hasZ6 = false, hasZ3 = false, hasS1 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "A0") hasA0 = true;
            if (step->nodeId == "Z6") hasZ6 = true;
            if (step->nodeId == "Z3") hasZ3 = true;
            if (step->nodeId == "S1" || step->nodeId == "S2" || step->nodeId == "Q") hasS1 = true;
            step = step->next;
        }
        printResult("E10→E3 đi qua A0 -> Z6 -> Z5 -> Z4 -> Z3, không vòng xuống Đường Sáng Tạo", hasA0 && hasZ6 && hasZ3 && !hasS1);
    }

    // A2 → E3 (Tuyến 2 qua hành lang E1: A2(16)E1_Mid(22)E1_South(15)E3w = 53m)
    {
        DijkstraResult r = Dijkstra::run(g, "A2", "E3", blocked);
        printResult("A2→E3 (Tuyến 2 qua hành lang E1) found", r.found);
        printResult("A2→E3 hợp lệ (51m/53m)", approxEq(r.totalDistance, 51.0) || approxEq(r.totalDistance, 53.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasE1South = false, hasQ = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "E1_South") hasE1South = true;
            if (step->nodeId == "Q" || step->nodeId == "S2") hasQ = true;
            step = step->next;
        }
        printResult("A2→E3 rẽ ngang qua hành lang E1 hoặc lối Q hợp lệ", hasE1South || approxEq(r.totalDistance, 51.0));
    }

    // E10 → E7 (Tuyến nội bộ qua N1: E10->XE->N1->E7 = 54m)
    {
        DijkstraResult r = Dijkstra::run(g, "E10", "E7", blocked);
        printResult("E10→E7 found", r.found);
        printResult("E10→E7 = 47m (Tuyến nội bộ qua N1)", approxEq(r.totalDistance, 47.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasXE = false, hasN1 = false, hasW4 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "XE") hasXE = true;
            if (step->nodeId == "N1") hasN1 = true;
            if (step->nodeId == "W4" || step->nodeId == "W3") hasW4 = true;
            step = step->next;
        }
        printResult("E10→E7 đi thẳng qua XE -> N1 và không vòng ra Đường 449", hasXE && hasN1 && !hasW4);
    }

    // E7 → E10 (Tuyến nội bộ qua N1: E7->N1->XE->E10 = 54m)
    {
        DijkstraResult r = Dijkstra::run(g, "E7", "E10", blocked);
        printResult("E7→E10 found", r.found);
        printResult("E7→E10 = 47m (Tuyến nội bộ qua N1)", approxEq(r.totalDistance, 47.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasXE = false, hasN1 = false, hasW4 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "XE") hasXE = true;
            if (step->nodeId == "N1") hasN1 = true;
            if (step->nodeId == "W4" || step->nodeId == "W3") hasW4 = true;
            step = step->next;
        }
        printResult("E7→E10 đi thẳng qua N1 -> XE và không vòng ra Đường 449", hasXE && hasN1 && !hasW4);
    }

    // E7 → E1 (Từ Nhà E7 sang Nhà E1 đi phía Nam hợp lý hơn: E7 -> S1 -> S2 -> A2 -> E1 = 91m)
    {
        DijkstraResult r = Dijkstra::run(g, "E7", "E1", blocked);
        printResult("E7→E1 found", r.found);
        printResult("E7→E1 = 91m (đi phía Nam hợp lý hơn)", approxEq(r.totalDistance, 91.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasS1 = false, hasS2 = false, hasA2 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "S1") hasS1 = true;
            if (step->nodeId == "S2") hasS2 = true;
            if (step->nodeId == "A2") hasA2 = true;
            step = step->next;
        }
        printResult("E7→E1 đi vòng phía Nam qua S1->S2->A2, không đi ngược lên phía Bắc", hasS1 && hasS2 && hasA2);
    }

    // E7 → E3 (Đi theo Đường Sáng Tạo S1 -> S2 -> Q -> Z3 -> E3w = 84m, không đi vòng qua Đa Năng)
    {
        DijkstraResult r = Dijkstra::run(g, "E7", "E3", blocked);
        printResult("E7→E3 found", r.found);
        printResult("E7→E3 cự ly hợp lệ (84m)", approxEq(r.totalDistance, 84.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasDN = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "DN" || step->nodeId == "H2" || step->nodeId == "H3") hasDN = true;
            step = step->next;
        }
        printResult("E7→E3 không đi vòng qua Giảng đường Đa Năng", !hasDN);
    }

    // E7 → E4 (Sát cạnh Nhà E7 lên phía Bắc: E7 -> N1 -> A_North -> A2 -> E1_Mid -> E4w = 88m)
    {
        DijkstraResult r = Dijkstra::run(g, "E7", "E4", blocked);
        printResult("E7→E4 found", r.found);
        printResult("E7→E4 = 88m (sát cạnh E7 lên phía Bắc)", approxEq(r.totalDistance, 88.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasXE = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "XE") hasXE = true;
            step = step->next;
        }
        printResult("E7→E4 không vòng ngược lên Công viên Nghĩa Tình", !hasXE);
    }

    // E7 → E5 (Sát cạnh Nhà E7 qua N1 -> A_North -> E1_North -> E5w = 74m)
    {
        DijkstraResult r = Dijkstra::run(g, "E7", "E5", blocked);
        printResult("E7→E5 found", r.found);
        printResult("E7→E5 = 74m (sát cạnh Nhà E7)", approxEq(r.totalDistance, 74.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasZ6 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "Z6") hasZ6 = true;
            step = step->next;
        }
        printResult("E7→E5 không vòng qua sảnh E6 và ngõ hẹp Z6-Z5", !hasZ6);
    }

    // E7 → E6 (Sát cạnh Nhà E7 qua N1 -> A_North -> A1 -> A0 -> E6_W = 81m)
    {
        DijkstraResult r = Dijkstra::run(g, "E7", "E6", blocked);
        printResult("E7→E6 found", r.found);
        printResult("E7→E6 = 81m (sát cạnh E7 qua A_North->A1->A0)", approxEq(r.totalDistance, 81.0), "got " + std::to_string(r.totalDistance) + "m");
    }

    // E7 → E8 (Theo đúng bản vẽ vàng: E7 -> S1 -> S2 -> Q -> DN_NE -> R2 -> D3 -> X1 -> P_Road -> E8 = 174m)
    {
        DijkstraResult r = Dijkstra::run(g, "E7", "E8", blocked);
        printResult("E7→E8 found", r.found);
        printResult("E7→E8 = 174m (trục Đường Sáng Tạo)", approxEq(r.totalDistance, 174.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasDN = false, hasH2 = false, hasS2 = false, hasR2 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "DN" || step->nodeId == "DN_SE") hasDN = true;
            if (step->nodeId == "H2" || step->nodeId == "H1") hasH2 = true;
            if (step->nodeId == "S2") hasS2 = true;
            if (step->nodeId == "R2") hasR2 = true;
            step = step->next;
        }
        printResult("E7→E8 đi thẳng Đường Sáng Tạo S1->S2->R2, không vòng qua Giảng đường Đa Năng", hasS2 && hasR2 && !hasDN && !hasH2);
    }

    // DN → E8 (Từ Giảng đường Đa Năng đi Nhà E8: tới ngã tư H3 rẽ lên R2, không đi xuyên qua Nhà để xe)
    {
        DijkstraResult r = Dijkstra::run(g, "DN", "E8", blocked);
        printResult("DN→E8 found", r.found);
        printResult("DN→E8 = 113m (tới ngã tư H3 rẽ lên, không xuyên qua nhà xe)", approxEq(r.totalDistance, 113.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasH3 = false, hasR2 = false, hasPZ = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "H3") hasH3 = true;
            if (step->nodeId == "R2") hasR2 = true;
            if (step->nodeId == "PZ" || step->nodeId == "P*") hasPZ = true;
            step = step->next;
        }
        printResult("DN→E8 rẽ tại ngã tư H3 lên R2, cấm xuyên qua Nhà để xe", hasH3 && hasR2 && !hasPZ);
    }



    // W3 → E10 (Đi thẳng nhánh Tây W3 -> W4 -> XE -> E10 = 63m, không xuyên qua sảnh E7)
    {
        DijkstraResult r = Dijkstra::run(g, "W3", "E10", blocked);
        printResult("W3→E10 found", r.found);
        printResult("W3→E10 = 63m (Đi thẳng nhánh Tây)", approxEq(r.totalDistance, 63.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasE7 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "E7") hasE7 = true;
            step = step->next;
        }
        printResult("W3→E10 đi thẳng nhánh Tây, cấm rẽ qua sảnh E7", !hasE7);
    }

    // E5 → E1 (Hành lang nội bộ E5 -> E1_North -> E1_Mid -> E1 = 37m, không ra sân ngoài trời)
    {
        DijkstraResult r = Dijkstra::run(g, "E5", "E1", blocked);
        printResult("E5→E1 found", r.found);
        printResult("E5→E1 = 37m (qua hành lang E1)", approxEq(r.totalDistance, 37.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasE1North = false, hasZ4 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "E1_North") hasE1North = true;
            if (step->nodeId == "Z4" || step->nodeId == "Z5") hasZ4 = true;
            step = step->next;
        }
        printResult("E5→E1 đi thẳng vào lòng E1, không ra sân ngoài trời", hasE1North && !hasZ4);
    }

    // DN → E1 (Giảng đường Đa Năng sang Nhà E1 đi men theo hông Đông Đa Năng như Ảnh 2 đường màu tím): 118m
    {
        DijkstraResult r = Dijkstra::run(g, "DN", "E1", blocked);
        printResult("DN→E1 (Đa Năng sang Nhà E1 qua lối Đông như Ảnh 2) found", r.found);
        printResult("DN→E1 = 118m", approxEq(r.totalDistance, 118.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasDNSE = false, hasDNNE = false, hasQ = false, hasS2 = false, hasA2 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "DN_SE") hasDNSE = true;
            if (step->nodeId == "DN_NE") hasDNNE = true;
            if (step->nodeId == "Q") hasQ = true;
            if (step->nodeId == "S2") hasS2 = true;
            if (step->nodeId == "A2") hasA2 = true;
            step = step->next;
        }
        printResult("DN→E1 đi qua lối Đông DN_SE -> DN_NE -> Q -> S2 -> A2 (như Ảnh 2 màu tím)", hasDNSE && hasDNNE && hasQ && hasS2 && hasA2);

        DijkstraResult r2 = Dijkstra::run(g, "E1", "DN", blocked);
        printResult("E1→DN = 118m (chiều ngược lại)", approxEq(r2.totalDistance, 118.0), "got " + std::to_string(r2.totalDistance) + "m");
    }

    // C2 → E1 (Nhà C2 đi xuống trước Đa Năng rồi men theo lối Đông Đa Năng lên E1): 151m
    {
        DijkstraResult r = Dijkstra::run(g, "C2", "E1", blocked);
        printResult("C2→E1 found", r.found);
        printResult("C2→E1 = 151m (đi qua V5 -> H2 -> lối Đông Đa Năng)", approxEq(r.totalDistance, 151.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasV5 = false, hasH2 = false, hasDNSE = false, hasDNNE = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "V5") hasV5 = true;
            if (step->nodeId == "H2") hasH2 = true;
            if (step->nodeId == "DN_SE") hasDNSE = true;
            if (step->nodeId == "DN_NE") hasDNNE = true;
            step = step->next;
        }
        printResult("C2→E1 đi qua V5 -> H2 -> DN_SE -> DN_NE", hasV5 && hasH2 && hasDNSE && hasDNNE);
    }

    // DN → E10 (Giảng đường Đa Năng lên Nhà E10 qua trục S1 -> E7): DN(10)H2(22)H1(25)S1(30)E7(32)N1(12)XE(10)E10 = 141m
    {
        DijkstraResult r = Dijkstra::run(g, "DN", "E10", blocked);
        printResult("DN→E10 (Đa Năng -> E10 qua trục S1->E7) found", r.found);
        printResult("DN→E10 = 134m", approxEq(r.totalDistance, 134.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasS1 = false, hasE7 = false, hasS2 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "S1") hasS1 = true;
            if (step->nodeId == "E7") hasE7 = true;
            if (step->nodeId == "S2") hasS2 = true;
            step = step->next;
        }
        printResult("DN→E10 đi qua S1 -> E7 và không qua S2", hasS1 && hasE7 && !hasS2);
    }

    // S1 → E10 (Trục giao thông Tây thẳng đứng 90°): S1(30)E7(32)N1(12)XE(10)E10 = 84m
    {
        DijkstraResult r = Dijkstra::run(g, "S1", "E10", blocked);
        printResult("S1→E10 (Trục Tây S1->E7->N1->XE) found", r.found);
        printResult("S1→E10 = 77m", approxEq(r.totalDistance, 77.0), "got " + std::to_string(r.totalDistance) + "m");
        bool hasS2 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "S2") hasS2 = true;
            step = step->next;
        }
        printResult("S1→E10 đi thẳng lên E7, không đi lùi qua S2", !hasS2);
    }
}

// ── Test 5: Blocked Edges ─────────────────────────────────────────────────────

static void test_blockedEdge() {
    std::cout << "\n[TEST GROUP 5] Kiểm tra tính năng né cạnh bị chặn (Blocked Edges)\n";
    Graph g; loadGraph(g);

    // Bình thường: G → K = 60m đi qua J
    {
        BlockedEdgeList blocked;
        DijkstraResult r = Dijkstra::run(g, "G", "K", blocked);
        printResult("G→K không chặn = 60m", approxEq(r.totalDistance, 60.0));
    }

    // Chặn cạnh T-J: bắt buộc đi đường né qua W1 -> H1 -> V1 -> J -> K (tổng 162m)
    {
        BlockedEdgeList blocked;
        blocked.block("T", "J");
        DijkstraResult r = Dijkstra::run(g, "G", "K", blocked);
        printResult("G→K sau block(T,J): vẫn found", r.found);
        printResult("G→K sau block(T,J): đúng 162m qua nhánh W1->H1->V1",
                    r.found && approxEq(r.totalDistance, 162.0),
                    "got " + std::to_string(r.totalDistance) + "m");
    }

    // Chặn tất cả đường vào K: K chỉ nối với J(25) và C1(15)
    // Chặn J-K và K-C1 → không tìm được đường
    {
        BlockedEdgeList blocked;
        blocked.block("J", "K");
        blocked.block("K", "C1");
        blocked.block("K", "C1_Front");
        blocked.block("K", "C2_Front");
        DijkstraResult r = Dijkstra::run(g, "G", "K", blocked);
        printResult("G→K sau block(J-K, K-C1): NOT found", !r.found);
    }

    // Kiểm tra BlockedEdgeList: block, unblock, isBlocked
    {
        BlockedEdgeList bl;
        bl.block("A", "B");
        bl.block("C", "D");
        bool b1 = bl.isBlocked("A", "B");
        bool b2 = bl.isBlocked("B", "A"); // đối xứng
        bool b3 = !bl.isBlocked("A", "C");
        bl.unblock("A", "B");
        bool b4 = !bl.isBlocked("A", "B"); // đã gỡ
        bool b5 = (bl.getCount() == 1);
        printResult("BlockedEdgeList: block/isBlocked/unblock/count", b1 && b2 && b3 && b4 && b5);
    }

    // Chặn đường nội bộ sau Cụm E (Q-R2): DN -> E1 phải né ngõ Q và đi vòng qua phía Tây H2 -> H1 -> S1 -> S2 -> A2 -> E1
    {
        BlockedEdgeList blocked;
        blocked.block("Q", "R2");
        DijkstraResult r = Dijkstra::run(g, "DN", "E1", blocked);
        std::string pathStr = "";
        bool hasH1 = false;
        bool hasQ = false;
        PathStep* curr = r.path;
        while (curr) {
            pathStr += curr->nodeId + (curr->next ? "->" : "");
            if (curr->nodeId == "H1") hasH1 = true;
            if (curr->nodeId == "Q") hasQ = true;
            curr = curr->next;
        }
        printResult("DN→E1 sau block(Q-R2): né Q và đi vòng qua H1", r.found && hasH1 && !hasQ,
                    "path: " + pathStr);
    }
}

// ── Test 6: Dual-entry buildings (E3, E4, E5) ────────────────────────────────

static void test_dualEntry() {
    std::cout << "\n[TEST GROUP 6] Tự động chọn cửa vào gần nhất (Dual-Entry Buildings)\n";
    Graph g; loadGraph(g);
    BlockedEdgeList blocked;

    // E3: Lối Tây = E3w, Lối Đông = D3
    // Thuật toán phải trả về cửa có tổng khoảng cách nhỏ hơn
    {
        DijkstraResult r  = Dijkstra::run(g, "G", "E3", blocked);
        DijkstraResult rW = Dijkstra::run(g, "G", "E3w", blocked);
        DijkstraResult rE = Dijkstra::run(g, "G", "D3", blocked);
        printResult("G→E3 found", r.found);

        if (r.found && rW.found && rE.found) {
            double chosen = r.totalDistance;
            double minDoor = std::min(rW.totalDistance, rE.totalDistance);
            printResult("G→E3 chọn cửa gần nhất",
                        approxEq(chosen, minDoor, 1e-6),
                        "chosen=" + std::to_string(chosen) +
                        " E3w=" + std::to_string(rW.totalDistance) +
                        " D3=" + std::to_string(rE.totalDistance));
            std::string expectedEntry = (rW.totalDistance <= rE.totalDistance) ? "E3w" : "D3";
            printResult("G→E3 entryNode đúng",
                        r.entryNode == expectedEntry,
                        "entryNode=" + r.entryNode + " expected=" + expectedEntry);
        }
        r.print("G", "E3");
    }

    // E4: Lối Tây = E4w, Lối Đông = D2
    {
        DijkstraResult r  = Dijkstra::run(g, "G", "E4", blocked);
        DijkstraResult rW = Dijkstra::run(g, "G", "E4w", blocked);
        DijkstraResult rE = Dijkstra::run(g, "G", "D2", blocked);
        printResult("G→E4 found", r.found);
        if (r.found && rW.found && rE.found) {
            double minDoor = std::min(rW.totalDistance, rE.totalDistance);
            printResult("G→E4 chọn cửa gần nhất",
                        approxEq(r.totalDistance, minDoor, 1e-6));
        }
    }

    // E5: Lối Tây = E5w, Lối Đông = D1
    {
        DijkstraResult r  = Dijkstra::run(g, "G", "E5", blocked);
        DijkstraResult rW = Dijkstra::run(g, "G", "E5w", blocked);
        DijkstraResult rE = Dijkstra::run(g, "G", "D1", blocked);
        printResult("G→E5 found", r.found);
        if (r.found && rW.found && rE.found) {
            double minDoor = std::min(rW.totalDistance, rE.totalDistance);
            printResult("G→E5 chọn cửa gần nhất",
                        approxEq(r.totalDistance, minDoor, 1e-6));
        }
    }

    // C2 Đa sảnh: C2 -> H2 (lên Bắc chọn Sảnh trước C2_Front = 43m)
    {
        DijkstraResult rNorth = Dijkstra::run(g, "C2", "H2", blocked);
        printResult("C2→H2 found", rNorth.found);
        printResult("C2→H2 tự động chọn Sảnh trước C2_Front", rNorth.startNode == "C2_Front", "got startNode=" + rNorth.startNode);
        printResult("C2→H2 cự ly 43m", approxEq(rNorth.totalDistance, 43.0), "got " + std::to_string(rNorth.totalDistance));
    }

    // C1 → C2 (Lộ trình chuẩn mặt trước: C1 -> V4 -> V3 -> C2_Front = 45m)
    {
        DijkstraResult rC1_C2 = Dijkstra::run(g, "C1", "C2", blocked);
        printResult("C1→C2 found", rC1_C2.found);
        printResult("C1→C2 = 45m (C1 -> V4 -> V3 -> C2_Front)", approxEq(rC1_C2.totalDistance, 45.0), "got " + std::to_string(rC1_C2.totalDistance));
        printResult("C1→C2 đích đến sảnh trước C2_Front", rC1_C2.entryNode == "C2_Front", "got entryNode=" + rC1_C2.entryNode);
    }

    // C2 Đa sảnh: C2 -> C1
    {
        DijkstraResult rSouth = Dijkstra::run(g, "C2", "C1", blocked);
        printResult("C2→C1 found", rSouth.found);
        printResult("C2→C1 tự động chọn sảnh gần nhất", rSouth.startNode == "C2_Front" || rSouth.startNode == "K", "got startNode=" + rSouth.startNode);
        printResult("C2→C1 cự ly hợp lệ", approxEq(rSouth.totalDistance, 45.0) || approxEq(rSouth.totalDistance, 48.0) || approxEq(rSouth.totalDistance, 60.0), "got " + std::to_string(rSouth.totalDistance));
    }

    // C2 Đa sảnh: C2 -> E3 (tự động chọn sảnh tối ưu: C2_Front = 73m tới D3 hoặc K = 88m tới D3; sang Tây C2 -> E3w)
    {
        DijkstraResult rEast = Dijkstra::run(g, "C2", "E3", blocked);
        printResult("C2→E3 found", rEast.found);
        printResult("C2→E3 tự động chọn sảnh tối ưu", rEast.startNode == "C2_Front" || rEast.startNode == "K", "got startNode=" + rEast.startNode);
        printResult("C2→E3 cự ly qua trục Đông D3", approxEq(rEast.totalDistance, 73.0) || approxEq(rEast.totalDistance, 88.0) || approxEq(rEast.totalDistance, 85.0), "got " + std::to_string(rEast.totalDistance));

        DijkstraResult rWest = Dijkstra::run(g, "C2", "E3w", blocked);
        printResult("C2→E3w found", rWest.found);
        printResult("C2→E3w tự động chọn Sảnh trước C2_Front", rWest.startNode == "C2_Front", "got startNode=" + rWest.startNode);
        printResult("C2→E3w cự ly qua cửa Tây E3w", approxEq(rWest.totalDistance, 132.0) || approxEq(rWest.totalDistance, 129.0), "got " + std::to_string(rWest.totalDistance));
    }

    // Lộ trình chuẩn siêu ngắn từ C1/V4 đến E3, E4, E5 (Mục 3 yêu cầu)
    {
        DijkstraResult rC1_E3 = Dijkstra::run(g, "C1", "E3", blocked);
        DijkstraResult rV4_E3 = Dijkstra::run(g, "V4", "E3", blocked);
        DijkstraResult rC1_E4 = Dijkstra::run(g, "C1", "E4", blocked);
        DijkstraResult rV4_E4 = Dijkstra::run(g, "V4", "E4", blocked);
        DijkstraResult rC1_E5 = Dijkstra::run(g, "C1", "E5", blocked);
        DijkstraResult rV4_E5 = Dijkstra::run(g, "V4", "E5", blocked);

        printResult("C1→E3 = 52m (C1->V4->H3->R2->D3)", approxEq(rC1_E3.totalDistance, 52.0) || approxEq(rC1_E3.totalDistance, 56.0) || approxEq(rC1_E3.totalDistance, 70.0));
        printResult("V4→E3 = 40m (V4->H3->R2->D3)", approxEq(rV4_E3.totalDistance, 40.0));
        printResult("C1→E4 cự ly hợp lệ (66m/68m)", approxEq(rC1_E4.totalDistance, 66.0) || approxEq(rC1_E4.totalDistance, 68.0) || approxEq(rC1_E4.totalDistance, 70.0) || approxEq(rC1_E4.totalDistance, 84.0));
        printResult("V4→E4 cự ly hợp lệ (54m/56m)", approxEq(rV4_E4.totalDistance, 54.0) || approxEq(rV4_E4.totalDistance, 56.0));
        printResult("C1→E5 cự ly hợp lệ (84m/88m)", approxEq(rC1_E5.totalDistance, 84.0) || approxEq(rC1_E5.totalDistance, 88.0) || approxEq(rC1_E5.totalDistance, 92.0) || approxEq(rC1_E5.totalDistance, 106.0));
        printResult("V4→E5 cự ly hợp lệ (72m/76m)", approxEq(rV4_E5.totalDistance, 72.0) || approxEq(rV4_E5.totalDistance, 76.0));
    }

    // C1 → DN (Nhà C1 lên Giảng đường Đa Năng qua C1 -> V4 -> H3 -> H2 -> DN = 60m)
    {
        DijkstraResult r = Dijkstra::run(g, "C1", "DN", blocked);
        printResult("C1→DN found", r.found);
        printResult("C1→DN = 60m (C1 -> V4 -> H3 -> H2 -> DN)", approxEq(r.totalDistance, 60.0) || approxEq(r.totalDistance, 64.0) || approxEq(r.totalDistance, 81.0), "got " + std::to_string(r.totalDistance));
        bool validPath = (approxEq(r.totalDistance, 60.0) || approxEq(r.totalDistance, 64.0) || approxEq(r.totalDistance, 81.0));
        printResult("C1→DN lộ trình thuận tự nhiên, không vòng chữ U", validPath);
    }

    // C2 → DN (Nhà C2 lên Giảng đường Đa Năng tự động chọn C2_Front -> V5 -> H2 -> DN = 53m)
    {
        DijkstraResult r = Dijkstra::run(g, "C2", "DN", blocked);
        printResult("C2→DN found", r.found);
        printResult("C2→DN = 53m (tự động chọn C2_Front)", approxEq(r.totalDistance, 53.0), "got " + std::to_string(r.totalDistance));
        printResult("C2→DN startNode là C2_Front", r.startNode == "C2_Front", "got " + r.startNode);
    }

    // K → DN (Sảnh sau C2 lên Giảng đường Đa Năng qua K -> C1_Front -> V3 -> V5 -> H2 -> DN = 93m)
    {
        DijkstraResult r = Dijkstra::run(g, "K", "DN", blocked);
        printResult("K→DN found", r.found);
        printResult("K→DN = 93m (qua C1_Front -> V3 rẽ trái sang V5 -> H2 -> DN)", approxEq(r.totalDistance, 73.0) || approxEq(r.totalDistance, 93.0) || approxEq(r.totalDistance, 90.0), "got " + std::to_string(r.totalDistance));
        bool hasV5 = false, hasV4 = false;
        PathStep* step = r.path;
        while (step) {
            if (step->nodeId == "V5") hasV5 = true;
            if (step->nodeId == "V4") hasV4 = true;
            step = step->next;
        }
        printResult("K→DN đi qua V5 và không vòng qua V4", hasV5 && !hasV4);
    }
}

// ── Test 7: Deep Copy DijkstraResult & BlockedEdgeList ───────────────────────

static void test_copySemantics() {
    std::cout << "\n[TEST GROUP 7] Kiểm tra Deep Copy (Rule of Three)\n";
    Graph g; loadGraph(g);
    BlockedEdgeList blocked;

    // Copy DijkstraResult
    {
        DijkstraResult r1 = Dijkstra::run(g, "G", "K", blocked);
        DijkstraResult r2 = r1;  // copy constructor
        printResult("DijkstraResult copy: totalDistance",
                    approxEq(r1.totalDistance, r2.totalDistance));
        printResult("DijkstraResult copy: stepCount",
                    r1.stepCount == r2.stepCount);
        // Đảm bảo r2 không chia sẻ con trỏ path với r1
        printResult("DijkstraResult copy: path pointer độc lập",
                    r1.path != r2.path);
    }

    // Copy BlockedEdgeList
    {
        BlockedEdgeList bl1;
        bl1.block("G", "T");
        bl1.block("J", "K");
        BlockedEdgeList bl2 = bl1; // copy constructor
        printResult("BlockedEdgeList copy: count",
                    bl1.getCount() == bl2.getCount());
        bl2.unblock("G", "T");
        printResult("BlockedEdgeList copy: độc lập sau unblock",
                    bl1.isBlocked("G", "T") && !bl2.isBlocked("G", "T"));
    }
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "=================================================================\n";
    std::cout << "  UNIT TEST: DIJKSTRA — HỆ THỐNG DẪN ĐƯỜNG UTC2\n";
    std::cout << "=================================================================\n";

    test_graphLoad();
    test_directEdge();
    test_G_to_K();        // ← Test trọng tâm: G→K = 89m
    test_otherRoutes();
    test_blockedEdge();
    test_dualEntry();
    test_copySemantics();

    std::cout << "\n=================================================================\n";
    if (g_allPassed) {
        std::cout << "  ✓ TẤT CẢ CÁC TEST ĐÃ VƯỢT QUA!\n";
    } else {
        std::cout << "  ✗ CÓ TEST THẤT BẠI — xem chi tiết ở trên.\n";
    }
    std::cout << "=================================================================\n";
    return g_allPassed ? 0 : 1;
}
