#include "AStar.h"
#include <cmath>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <unordered_map>

const double AStar::INF = 1e15;

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
    {"E7",  {440.0, 315.0}}, {"A1",  {575.0, 110.0}}, {"A_North", {575.0, 175.0}}, {"A2",  {575.0, 315.0}}, {"E1",  {630.0, 315.0}},
    {"XE",  {410.0, 110.0}}, {"N1",  {440.0, 165.0}}, {"E10", {410.0, 85.0}}, {"A0", {575.0, 85.0}}, {"Z6", {705.0, 85.0}},
    {"E1_North", {630.0, 190.0}}, {"E1_Mid", {630.0, 315.0}}, {"E1_South", {630.0, 440.0}},
    {"E3",  {740.0, 440.0}}, {"E4",  {740.0, 315.0}}, {"E5",  {740.0, 190.0}},
    {"Z3",  {705.0, 440.0}}, {"E3w", {740.0, 440.0}}, {"Z4",  {705.0, 315.0}},
    {"E4w", {740.0, 315.0}}, {"Z5",  {705.0, 190.0}}, {"E5w", {740.0, 190.0}},
    {"R2",  {920.0, 492.0}}, {"D3",  {920.0, 440.0}}, {"X1",  {920.0, 342.0}},
    {"D2",  {920.0, 315.0}}, {"D1",  {920.0, 190.0}}, {"D0",  {920.0, 85.0}},
    {"E6_W",{755.0, 85.0}}, {"E6",{755.0, 85.0}}, {"E9",{920.0, 85.0}}
};

void AStar::getNodeCoord(const Graph& g, const std::string& name, double& x, double& y) {
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

double AStar::euclideanDistance(double x1, double y1, double x2, double y2) {
    double dx = x1 - x2;
    double dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}

double AStar::heuristic(const Graph& g, const std::string& u, const std::string& v, double scale) {
    double xu = 0, yu = 0, xv = 0, yv = 0;
    getNodeCoord(g, u, xu, yu);
    getNodeCoord(g, v, xv, yv);
    return scale * euclideanDistance(xu, yu, xv, yv);
}

void AStar::resolveTarget(const std::string& dest, std::string& out1, std::string& out2) {
    out1 = dest;
    out2 = "";
    if (dest == "E3" || dest == "NHÀ E3") {
        out1 = "D3";
        out2 = "E3w";
    } else if (dest == "E3w" || dest == "E3W" || dest == "E3_WEST" || dest == "E3_West") {
        out1 = "E3w";
        out2 = "";
    } else if (dest == "E3_EAST" || dest == "E3_East" || dest == "E3_E" || dest == "D3") {
        out1 = "D3";
        out2 = "";
    } else if (dest == "E4" || dest == "NHÀ E4") {
        out1 = "D2";
        out2 = "E4w";
    } else if (dest == "E4w" || dest == "E4W" || dest == "E4_WEST" || dest == "E4_West") {
        out1 = "E4w";
        out2 = "";
    } else if (dest == "E4_EAST" || dest == "E4_East" || dest == "E4_E" || dest == "D2") {
        out1 = "D2";
        out2 = "";
    } else if (dest == "E5" || dest == "NHÀ E5") {
        out1 = "D1";
        out2 = "E5w";
    } else if (dest == "E5w" || dest == "E5W" || dest == "E5_WEST" || dest == "E5_West") {
        out1 = "E5w";
        out2 = "";
    } else if (dest == "E5_EAST" || dest == "E5_East" || dest == "E5_E" || dest == "D1") {
        out1 = "D1";
        out2 = "";
    } else if (dest == "P*" || dest == "NHÀ XE" || dest == "NHÀ ĐỂ XE" || dest == "BÃI XE") {
        out1 = "PZ";
    } else if (dest == "NHÀ E1" || dest == "KHOA CNTT") {
        out1 = "E1";
    } else if (dest == "C3" || dest == "NHÀ C3" || dest == "THƯ VIỆN") {
        out1 = "V4_S";
    } else if (dest == "C1" || dest == "NHÀ C1" || dest == "HỘI TRƯỜNG C1" || dest == "C1_SANH" || dest == "C1_Sanh" || dest == "C1_EAST" || dest == "C1_East") {
        out1 = "C1";
    } else if (dest == "C1_WEST" || dest == "C1_Front" || dest == "C1_FRONT") {
        out1 = "C1_Front";
    } else if (dest == "CT" || dest == "CĂN TIN" || dest == "CANTEEN" || dest == "CĂN TIN SINH VIÊN") {
        out1 = "V4";
    } else if (dest == "C2" || dest == "NHÀ C2" || dest == "NHAC2" || dest == "PHÒNG C2") {
        out1 = "C2_Front";
        out2 = "K";
    } else if (dest == "E6" || dest == "NHÀ E6" || dest == "E6_W") {
        out1 = "E6";
    } else if (dest == "E9" || dest == "NHÀ E9" || dest == "D0") {
        out1 = "E9";
    } else if (dest == "A0" || dest == "NGÃ TƯ A0") {
        out1 = "A0";
    } else if (dest == "Z6" || dest == "HÀNH LANG Z6") {
        out1 = "Z6";
    } else if (dest == "E1" || dest == "NHÀ E1" || dest == "KHOA CNTT" || dest == "E1_MID" || dest == "E1_Mid") {
        out1 = "E1_Mid";
    } else if (dest == "E1_NORTH" || dest == "E1_North") {
        out1 = "E1_North";
    } else if (dest == "E1_SOUTH" || dest == "E1_South") {
        out1 = "E1_South";
    }
}

void AStar::resolveOrigin(const std::string& src, std::string& out1, std::string& out2) {
    out1 = src;
    out2 = "";
    if (src == "C2" || src == "NHÀ C2" || src == "NHAC2" || src == "PHÒNG C2") {
        out1 = "C2_Front";
        out2 = "K";
    } else if (src == "P*" || src == "NHÀ XE" || src == "NHÀ ĐỂ XE" || src == "BÃI XE") {
        out1 = "PZ";
    } else if (src == "E1" || src == "NHÀ E1" || src == "KHOA CNTT" || src == "E1_MID" || src == "E1_Mid") {
        out1 = "E1_Mid";
    } else if (src == "C3" || src == "NHÀ C3" || src == "THƯ VIỆN") {
        out1 = "V4_S";
    } else if (src == "C1" || src == "NHÀ C1" || src == "HỘI TRƯỜNG C1" || src == "C1_SANH" || src == "C1_Sanh" || src == "C1_EAST" || src == "C1_East") {
        out1 = "C1";
    } else if (src == "C1_WEST" || src == "C1_Front" || src == "C1_FRONT") {
        out1 = "C1_Front";
    } else if (src == "CT" || src == "CĂN TIN" || src == "CANTEEN" || src == "CĂN TIN SINH VIÊN") {
        out1 = "V4";
    } else if (src == "E6" || src == "NHÀ E6" || src == "E6_W") {
        out1 = "E6";
    } else if (src == "E9" || src == "NHÀ E9" || src == "D0") {
        out1 = "E9";
    } else if (src == "A0" || src == "NGÃ TƯ A0") {
        out1 = "A0";
    } else if (src == "Z6" || src == "HÀNH LANG Z6") {
        out1 = "Z6";
    } else if (src == "E1_MID" || src == "E1_Mid") {
        out1 = "E1_Mid";
    } else if (src == "E1_NORTH" || src == "E1_North") {
        out1 = "E1_North";
    } else if (src == "E1_SOUTH" || src == "E1_South") {
        out1 = "E1_South";
    }
}

namespace {
    [[maybe_unused]] bool isClusterC1C2(const std::string& id) {
        return (id == "K" || id == "C2" || id == "NHÀ C2" || id == "PHÒNG C2" ||
                id == "C1_J" || id == "C1" || id == "NHÀ C1" || id == "HỘI TRƯỜNG C1" ||
                id == "C1_SANH" || id == "C1_Sanh" || id == "C1_WEST" || id == "C1_Front" || id == "C1_FRONT");
    }

    [[maybe_unused]] bool isClusterC(const std::string& id) {
        return (id == "K" || id == "C2" || id == "NHÀ C2" || id == "PHÒNG C2" ||
                id == "C1_J" || id == "C1" || id == "NHÀ C1" || id == "HỘI TRƯỜNG C1" ||
                id == "C1_SANH" || id == "C1_Sanh" || id == "C1_East" || id == "C1_EAST" ||
                id == "V4" || id == "C3" || id == "NHÀ C3" || id == "THƯ VIỆN" ||
                id == "CT" || id == "CĂN TIN" || id == "CANTEEN" ||
                id == "V1" || id == "V3" || id == "V5" || id == "C2_Front" || id == "V4_S" ||
                id == "C1_Front" || id == "C1_FRONT");
    }

    bool isEWestNorth(const std::string& id) {
        return (id == "E7" || id == "NHÀ E7" ||
                id == "E10" || id == "NHÀ E10" ||
                id == "E6" || id == "E6_W" || id == "NHÀ E6" ||
                id == "XE" || id == "N1");
    }

    bool isEWestEntry(const std::string& id) {
        return (id == "E3" || id == "E3w" || id == "NHÀ E3" ||
                id == "E4" || id == "E4w" || id == "NHÀ E4" ||
                id == "E5" || id == "E5w" || id == "NHÀ E5");
    }

    [[maybe_unused]] bool isEastAxis(const std::string& id) {
        return (id == "D3" || id == "D2" || id == "D1" || id == "R2" ||
                id == "E3_East" || id == "E3_EAST" || id == "E3_E" ||
                id == "E4_East" || id == "E4_EAST" || id == "E4_E" ||
                id == "E5_East" || id == "E5_EAST" || id == "E5_E");
    }

    bool isE8(const std::string& id) {
        return (id == "E8" || id == "NHÀ E8" || id == "E8B" || id == "NHÀ E8B" || id == "X1");
    }

    double getRoadMultiplier(const Graph& graph, const std::string& u, const std::string& v) {
        std::string rtype = graph.getEdgeRoadType(u, v);
        if (rtype == "main" || rtype == "major") return 0.8;
        if (rtype == "internal" || rtype == "minor") return 1.4;
        return 1.0;
    }

    bool isEdgeRestricted(const std::string& u, const std::string& v, const std::string& src, const std::string& dest, const BlockedEdgeList& blocked = BlockedEdgeList()) {
        bool isTJ   = (u == "T" && v == "J")   || (u == "J" && v == "T");
        bool isTW1  = (u == "T" && v == "W1")  || (u == "W1" && v == "T");
        bool isW1H1 = (u == "W1" && v == "H1") || (u == "H1" && v == "W1");
        bool isW2S1 = (u == "W2" && v == "S1") || (u == "S1" && v == "W2");
        bool isW2W3 = (u == "W2" && v == "W3") || (u == "W3" && v == "W2");
        bool isS1E7 = (u == "S1" && v == "E7") || (u == "E7" && v == "S1");
        bool isH3R2 = (u == "H3" && v == "R2") || (u == "R2" && v == "H3");
        bool isQR2  = (u == "Q" && v == "R2")  || (u == "R2" && v == "Q") || (u == "DN_NE" && v == "R2") || (u == "R2" && v == "DN_NE");
        bool isE1Z4 = (u == "E1" && v == "Z4") || (u == "Z4" && v == "E1");
        bool isH1S1 = (u == "H1" && v == "S1") || (u == "S1" && v == "H1");
        bool isA2A1 = (u == "A2" && v == "A1") || (u == "A1" && v == "A2");
        bool isD2D1 = (u == "D2" && v == "D1") || (u == "D1" && v == "D2");
        bool isZ6Z5 = (u == "Z6" && v == "Z5") || (u == "Z5" && v == "Z6");
        bool isS2Q  = (u == "S2" && v == "Q")  || (u == "Q" && v == "S2");
        bool isA2S2 = (u == "A2" && v == "S2") || (u == "S2" && v == "A2");
        bool isXEE10 = (u == "XE" && v == "E10") || (u == "E10" && v == "XE");
        bool isA0A1 = (u == "A0" && v == "A1") || (u == "A1" && v == "A0");
        bool isH2S2 = (u == "H2" && v == "S2") || (u == "S2" && v == "H2");
        bool isW3E7 = (u == "W3" && v == "E7") || (u == "E7" && v == "W3");
        bool isE7A2 = (u == "E7" && v == "A2") || (u == "A2" && v == "E7");
        bool isS1S2 = (u == "S1" && v == "S2") || (u == "S2" && v == "S1");
        bool isV4H3 = (u == "V4" && v == "H3") || (u == "H3" && v == "V4");
        bool isH2H3 = (u == "H2" && v == "H3") || (u == "H3" && v == "H2") || (u == "H2" && v == "DN_SE") || (u == "DN_SE" && v == "H2");

        bool destIsC = isClusterC(dest);
        bool srcIsC  = isClusterC(src);

        // 0. Kịch bản C2 lên Giảng đường Đa Năng (DN): Cấm đi vòng chữ U qua V4 -> H3
        bool srcIsC2 = src == "C2" || src == "K" || src == "C2_Front" || src == "C1_Front" || src == "C1_J" || src == "NHÀ C2";
        bool destIsC2 = dest == "C2" || dest == "K" || dest == "C2_Front" || dest == "C1_Front" || dest == "C1_J" || dest == "NHÀ C2";
        bool destIsDN = dest == "DN" || dest == "GIẢNG ĐƯỜNG ĐA NĂNG" || dest == "H2";
        bool srcIsDN = src == "DN" || src == "GIẢNG ĐƯỜNG ĐA NĂNG" || src == "H2";

        if ((srcIsC2 && destIsDN) || (srcIsDN && destIsC2)) {
            if (isV4H3 || isH2H3) return true;
        }

        // 0b. Kịch bản Nhà E1: Luôn vào qua sảnh chính Đường Ánh Đạo (A2 -> E1_Mid), không chui xuyên qua E4/E3/E5
        bool destIsE1 = (dest == "E1" || dest == "NHÀ E1" || dest == "E1_Mid" || dest == "E1_MID" || dest == "KHOA CNTT" || dest == "A2");
        bool srcIsE1 = (src == "E1" || src == "NHÀ E1" || src == "E1_Mid" || src == "E1_MID" || src == "KHOA CNTT" || src == "A2");

        // Kịch bản Giảng đường Đa Năng <-> Nhà E1: Đi theo lối Đông Đa Năng lên Đường Sáng Tạo (như Ảnh 2 đường màu tím)
        bool isDNE1Route = srcIsDN && destIsE1;
        bool isE1DNRoute = destIsDN && srcIsE1;
        if (isDNE1Route || isE1DNRoute) {
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) return true;
            if ((u == "E1_South" && v == "E1_Mid") || (u == "E1_Mid" && v == "E1_South")) return true;
            if ((u == "E1_North" && v == "E1_Mid") || (u == "E1_Mid" && v == "E1_North")) return true;
            bool eastIsBlocked = blocked.isBlocked("Q", "DN_NE") ||
                                  blocked.isBlocked("DN_SE", "DN_East") ||
                                  blocked.isBlocked("DN_East", "DN_NE") ||
                                  blocked.isBlocked("H2", "DN_SE") ||
                                  blocked.isBlocked("Q", "R2");
            if (!eastIsBlocked) {
                if ((u == "H2" && v == "H1") || (u == "H1" && v == "H2")) return true;
            }
        }

        // Kịch bản C2 <-> Nhà E1: Đi xuống trước Đa Năng rồi men theo lối Đông Đa Năng lên Đường Sáng Tạo & Ánh Đạo
        if ((srcIsC2 && destIsE1) || (srcIsE1 && destIsC2)) {
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) return true;
            if ((u == "E1_South" && v == "E1_Mid") || (u == "E1_Mid" && v == "E1_South")) return true;
            if ((u == "E1_North" && v == "E1_Mid") || (u == "E1_Mid" && v == "E1_North")) return true;
            bool eastIsBlocked = blocked.isBlocked("Q", "DN_NE") ||
                                  blocked.isBlocked("DN_SE", "DN_East") ||
                                  blocked.isBlocked("DN_East", "DN_NE") ||
                                  blocked.isBlocked("H2", "DN_SE") ||
                                  blocked.isBlocked("Q", "R2");
            if (!eastIsBlocked) {
                if ((u == "V1" && v == "H1") || (u == "H1" && v == "V1")) return true;
                if ((u == "H2" && v == "H1") || (u == "H1" && v == "H2")) return true;
                if ((u == "V3" && v == "V4") || (u == "V4" && v == "V3")) return true;
                if ((u == "V4" && v == "H3") || (u == "H3" && v == "V4")) return true;
            }
        }

        if (destIsC || srcIsC) {
            bool isToC1 = (dest == "C1" || dest == "NHÀ C1" || dest == "HỘI TRƯỜNG C1" || dest == "C1_SANH");
            bool isFromC1 = (src == "C1" || src == "NHÀ C1" || src == "HỘI TRƯỜNG C1" || src == "C1_SANH");
            if (isToC1 || isFromC1) {
                if ((u == "J" && v == "K") || (u == "K" && v == "J")) return true;
                if ((u == "C1_Front" && v == "V3") || (u == "V3" && v == "C1_Front")) return true;
            }
            if (isTW1) return true;
            return false;
        }

        if (isTJ) return true;

        // Kịch bản Nhà E1 (Đường Sáng Tạo & Ánh Đạo): Tuyến chuẩn G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> E1
        if (dest == "E1" || dest == "A2" || dest == "NHÀ E1" || dest == "KHOA CNTT" || dest == "E1_Mid" || dest == "E1_MID") {
            if (isA2A1) return true;
            if (isW1H1) return true; // Bắt buộc đi dọc Đại lộ Thành Công W1 -> W2 rồi rẽ vào Đường Sáng Tạo S1
        }

        // Kịch bản Nhà E6 (Đường Sáng Tạo & Ánh Đạo): Tuyến chuẩn G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> A1 -> E6_W
        if (dest == "E6" || dest == "E6_W" || dest == "NHÀ E6") {
            if (isW1H1) return true;
            if (isW2W3) return true;
            if (isS1E7) return true;
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) return true;
        }
        if (src == "E6" || src == "E6_W" || src == "NHÀ E6") {
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) return true;
        }
        if (dest == "E9" || dest == "NHÀ E9") {
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) return true;
        }
        if (src == "E9" || src == "NHÀ E9") {
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) return true;
        }

        // Kịch bản các tòa E Tây và Tây-Bắc (E7, E10)
        if (dest == "E10" || dest == "NHÀ E10") {
            if (isW3E7 || isE7A2 || isS1S2) return true; // Cấm rẽ từ W3 vào E7, cấm từ E7 luồn qua cầu Ánh Đạo A2, và cấm đi lùi S1->S2 khi lên E10
        }
        bool isE1Related = (dest == "E1" || dest == "NHÀ E1" || dest == "KHOA CNTT" || dest == "E1_Mid" || dest == "E1_MID");
        bool srcIsERelated = (src == "E3" || src == "E3w" || src == "E4" || src == "E4w" || src == "E5" || src == "E5w" ||
                              src == "E3_EAST" || src == "E4_EAST" || src == "E5_EAST" || src == "D3" || src == "D2" || src == "D1");
        srcIsE1 = (src == "E1" || src == "NHÀ E1" || src == "KHOA CNTT" || src == "E1_Mid" || src == "E1_MID");
        bool destIsERelated = (dest == "E3" || dest == "E3w" || dest == "E4" || dest == "E4w" || dest == "E5" || dest == "E5w");

        if ((isE1Related && srcIsERelated) || (srcIsE1 && destIsERelated)) {
            if ((u == "Z4" && v == "Z5") || (u == "Z5" && v == "Z4") ||
                (u == "Z3" && v == "Z4") || (u == "Z4" && v == "Z3") ||
                (u == "Q" && v == "Z3") || (u == "Z3" && v == "Q") ||
                (u == "S2" && v == "Q") || (u == "Q" && v == "S2") ||
                (u == "A2" && v == "S2") || (u == "S2" && v == "A2") ||
                (u == "A1" && v == "A2") || (u == "A2" && v == "A1") ||
                (u == "A0" && v == "A1") || (u == "A1" && v == "A0") ||
                (u == "Z6" && v == "A0") || (u == "A0" && v == "Z6") ||
                (u == "Z5" && v == "Z6") || (u == "Z6" && v == "Z5")) {
                return true; // Đi qua hành lang bên trong E1, không ra khoảng sân ngoài trời bên hông hoặc đường xe
            }
        }
        if (src == "E7" && (dest == "E1" || dest == "E1_Mid" || dest == "NHÀ E1")) {
            bool isE7N1 = (u == "E7" && v == "N1") || (u == "N1" && v == "E7");
            if (isE7N1) return true;
        }
        if (isEWestNorth(dest) && (src == "G" || src == "T" || src == "W1")) {
            if (isW1H1) return true;
        }
        if (dest == "E6" || dest == "E6_W" || dest == "NHÀ E6") {
            bool isE7N1 = (u == "E7" && v == "N1") || (u == "N1" && v == "E7");
            if ((src != "E7" && isE7N1) || isW2S1) return true; // Bắt buộc đi hết trục Tây Bắc: W1 -> W2 -> W3 -> W4 -> XE -> N1 -> A1 -> E6_W
        }

        // Kịch bản Nhà E3, E4, E5 (Lối Tây qua nút Q)
        if (isEWestEntry(dest) || dest == "E3" || dest == "E4" || dest == "E5") {
            if (src == "G" || src == "T" || src == "W1") {
                if (isW1H1) return true;
                if (isW2W3) return true;
                if (isS1E7) return true;
                if (isH3R2 || isQR2) return true;
                if (isZ6Z5) return true;

                bool isDestE4 = (dest == "E4" || dest == "E4w" || dest == "NHÀ E4");
                bool isDestE5 = (dest == "E5" || dest == "E5w" || dest == "NHÀ E5");
                bool isDestE3 = (dest == "E3" || dest == "E3w" || dest == "NHÀ E3");

                if (isDestE4 || isDestE5) {
                    if (isS2Q) return true;
                    if ((u == "A2" && v == "A_North") || (u == "A_North" && v == "A2")) return true;
                    if ((u == "Z4" && v == "Z5") || (u == "Z5" && v == "Z4")) return true;
                } else if (isDestE3) {
                    bool isA2E1Mid = (u == "A2" && (v == "E1_Mid" || v == "E1")) || (v == "A2" && (u == "E1_Mid" || u == "E1"));
                    if (isA2E1Mid) return true;
                }
            } else if (src == "H1") {
                if (isW2S1) return true;
                if (isH2S2) return true;
                bool isA2E1Mid = (u == "A2" && (v == "E1_Mid" || v == "E1")) || (v == "A2" && (u == "E1_Mid" || u == "E1"));
                if (isE1Z4 || isA2E1Mid) return true;
                if (isH3R2 || isQR2) return true;
                if (isZ6Z5) return true;
            } else if (isClusterC(src) || src == "H2" || src == "DN") {
                // Xuất phát từ Cụm C (C2, C1, V4...) hoặc Đa Năng đi E3/E4/E5:
                // Tuyến vòng ngoài phía Đông qua Nhà để xe: H2 -> H3 -> R2 -> D3...
                if (isQR2) return true;
                if (isZ6Z5) return true;
            } else if (src == "E10" || src == "NHÀ E10" || src == "A0" || src == "NGÃ TƯ A0") {
                // Xuất phát từ phía Bắc (E10, A0, A1, A2...):
                // Tuyệt đối không chạy vòng xuống Đường Sáng Tạo S2 -> Q:
                if (isS2Q || isA2S2) {
                    return true;
                }
                // Nếu xuất phát từ E10: đi thẳng qua trục ngang phía Bắc A0 -> Z6 -> Z5 -> Z4 -> Z3
                if (src == "E10" || src == "NHÀ E10") {
                    if (isXEE10 || isA0A1) {
                        return true; // Ép đi Tuyến 1: E10 -> A0 -> Z6 -> Z5 -> Z4 -> Z3 -> E3w
                    }
                    if ((u == "A0" && v == "D0") || (u == "D0" && v == "A0") ||
                        (u == "E6" && v == "D0") || (u == "D0" && v == "E6")) return true;
                }
            }
        }

        // Kịch bản Nhà E8 / E8B
        if (dest == "E5" || dest == "NHÀ E5" || dest == "E5w") {
            if (src != "E10" && src != "NHÀ E10" && src != "A0") {
                if (isZ6Z5) return true;
            }
        }
        if (isE8(dest)) {
            // Không đi xuyên qua Nhà để xe PZ (trừ khi xuất phát từ chính Nhà để xe)
            bool isParking = (u == "PZ" || v == "PZ" || u == "P*" || v == "P*");
            if (src != "PZ" && src != "P*" && isParking) return true;

            if (src == "E7" || src == "NHÀ E7" || src == "S1" || src == "S2") {
                if (isH1S1) return true;
            } else {
                if (isQR2) return true;
            }
            if (isD2D1) return true; // Tuyệt đối không đi lố lên D1 (ngang hông Nhà E5) rồi mới sang E8
            if (src == "G" || src == "T" || src == "W1" || src == "H1") {
                if (isH1S1) return true;
            }
        }

        if (isE8(src) && dest != "PZ" && dest != "P*") {
            bool isParking = (u == "PZ" || v == "PZ" || u == "P*" || v == "P*");
            if (isParking) return true;
        }

        return false;
    }

    double getTurnPenalty(const Graph& graph, int pIdx, int uIdx, int vIdx) {
        if (pIdx == -1) return 0.0;
        std::string pName = graph.getVertexName(pIdx);
        std::string uName = graph.getVertexName(uIdx);
        std::string vName = graph.getVertexName(vIdx);

        double px = graph.getX(pName), py = graph.getY(pName);
        double ux = graph.getX(uName), uy = graph.getY(uName);
        double vx = graph.getX(vName), vy = graph.getY(vName);

        double ax = ux - px, ay = uy - py;
        double bx = vx - ux, by = vy - uy;

        double cross = ax * by - ay * bx;
        double dot = ax * bx + ay * by;

        double rad = std::atan2(cross, dot);
        double deg = std::abs(rad * (180.0 / 3.14159265358979323846));
        if (deg <= 25.0) return 0.0;
        return 15.0 * (deg / 90.0);
    }
}

bool AStar::runAStarSingleTarget(
    const Graph& graph,
    const std::string& src,
    const std::string& target,
    const BlockedEdgeList& blocked,
    double heuristicScale,
    AStarResult& outResult
) {
    int n = graph.getNumVertices();
    int srcIdx = graph.findVertexIndex(src);
    int targetIdx = graph.findVertexIndex(target);

    if (srcIdx == -1 || targetIdx == -1) {
        outResult.found = false;
        return false;
    }

    if (src == target) {
        outResult.found = true;
        outResult.totalDistance = 0.0;
        outResult.path = {src};
        outResult.visitedNodes = 1;
        return true;
    }

    std::vector<double> gDist(n, INF);
    std::vector<bool> closed(n, false);
    std::vector<int> prev(n, -1);

    MinHeap openSet;
    gDist[srcIdx] = 0.0;
    double hStart = heuristic(graph, src, target, heuristicScale);
    openSet.insert(src, hStart);

    int visitedCount = 0;
    bool found = false;

    while (!openSet.isEmpty()) {
        HeapNode top = openSet.extractMin();
        std::string uName = top.name;
        int uIdx = graph.findVertexIndex(uName);
        if (uIdx == -1) continue;

        if (closed[uIdx]) {
            continue; // Bỏ qua bản ghi cũ do lazy deletion
        }

        closed[uIdx] = true;
        ++visitedCount;

        if (uName == target) {
            found = true;
            break;
        }

        EdgeNode* edge = graph.getNeighbors(uIdx);
        while (edge != nullptr) {
            std::string vName = edge->dest;
            double weight = edge->weight;

            if (!blocked.isBlocked(uName, vName) && !isEdgeRestricted(uName, vName, src, target, blocked)) {
                int vIdx = graph.findVertexIndex(vName);
                if (vIdx != -1 && !closed[vIdx]) {
                    double roadMult = getRoadMultiplier(graph, uName, vName);
                    double turnPen = getTurnPenalty(graph, prev[uIdx], uIdx, vIdx);
                    double stepCost = (weight * roadMult) + turnPen;
                    double tentG = gDist[uIdx] + stepCost;
                    if (tentG < gDist[vIdx]) {
                        gDist[vIdx] = tentG;
                        prev[vIdx] = uIdx;
                        double f = tentG + heuristic(graph, vName, target, heuristicScale);
                        openSet.insert(vName, f);
                    }
                }
            }
            edge = edge->next;
        }
    }

    outResult.visitedNodes = visitedCount;

    if (!found || gDist[targetIdx] >= INF) {
        outResult.found = false;
        return false;
    }

    outResult.found = true;

    // Truy vết lộ trình từ target về src
    std::vector<std::string> revPath;
    int curr = targetIdx;
    while (curr != -1) {
        revPath.push_back(graph.getVertexName(curr));
        curr = prev[curr];
    }
    std::reverse(revPath.begin(), revPath.end());
    outResult.path = revPath;

    // Tính tổng cự ly mét thực tế
    double totalPhysical = 0.0;
    for (size_t i = 0; i + 1 < revPath.size(); ++i) {
        double w = graph.getEdgeWeight(revPath[i], revPath[i + 1]);
        totalPhysical += (w >= 0 ? w : 0.0);
    }
    outResult.totalDistance = totalPhysical;

    return true;
}

AStarResult AStar::run(
    const Graph& graph,
    const std::string& src,
    const std::string& dest,
    const BlockedEdgeList& blocked,
    double heuristicScale
) {
    auto tStart = std::chrono::high_resolution_clock::now();

    std::string s1, s2;
    resolveOrigin(src, s1, s2);

    std::string t1, t2;
    resolveTarget(dest, t1, t2);

    std::vector<std::string> validOrigins;
    if (graph.hasVertex(s1)) validOrigins.push_back(s1);
    if (!s2.empty() && graph.hasVertex(s2)) validOrigins.push_back(s2);
    if (validOrigins.empty()) {
        if (graph.hasVertex(src)) validOrigins.push_back(src);
        else {
            AStarResult res;
            res.found = false;
            return res;
        }
    }

    std::vector<std::string> validTargets;
    if (graph.hasVertex(t1)) validTargets.push_back(t1);
    if (!t2.empty() && graph.hasVertex(t2)) validTargets.push_back(t2);
    if (validTargets.empty()) {
        if (graph.hasVertex(dest)) validTargets.push_back(dest);
        else {
            AStarResult res;
            res.found = false;
            return res;
        }
    }

    AStarResult finalRes;
    finalRes.found = false;
    double bestDist = INF;
    int totalVisited = 0;

    for (const auto& curSrc : validOrigins) {
        for (const auto& curTgt : validTargets) {
            AStarResult tempRes;
            bool ok = runAStarSingleTarget(graph, curSrc, curTgt, blocked, heuristicScale, tempRes);
            if (ok) {
                totalVisited += tempRes.visitedNodes;
                if (!finalRes.found || tempRes.totalDistance < bestDist) {
                    bestDist = tempRes.totalDistance;
                    finalRes = tempRes;
                    finalRes.startNode = curSrc;
                    finalRes.entryNode = curTgt;
                }
            }
        }
    }

    if (finalRes.found) {
        finalRes.visitedNodes = totalVisited;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    double elapsedUs = std::chrono::duration<double, std::micro>(tEnd - tStart).count();
    finalRes.executionTimeUs = elapsedUs;

    return finalRes;
}

AlgorithmMetric AStar::runDijkstraMetric(
    const Graph& graph,
    const std::string& src,
    const std::string& dest,
    const BlockedEdgeList& blocked
) {
    AlgorithmMetric metric;
    metric.algorithmName = "Dijkstra";

    auto tStart = std::chrono::high_resolution_clock::now();

    std::string cleanSrc = src;
    if (cleanSrc == "P*" || cleanSrc == "NHÀ XE" || cleanSrc == "NHÀ ĐỂ XE" || cleanSrc == "BÃI XE") cleanSrc = "PZ";
    else if (cleanSrc == "A2" || cleanSrc == "NHÀ E1" || cleanSrc == "KHOA CNTT") cleanSrc = "E1";
    else if (cleanSrc == "CT" || cleanSrc == "CĂN TIN" || cleanSrc == "CANTEEN") cleanSrc = "CT";

    std::string t1, t2;
    resolveTarget(dest, t1, t2);

    auto runCoreDijk = [&](const std::string& target, double& outDist, std::vector<std::string>& outPath, int& outVisited) -> bool {
        int n = graph.getNumVertices();
        int srcIdx = graph.findVertexIndex(cleanSrc);
        int targetIdx = graph.findVertexIndex(target);
        if (srcIdx == -1 || targetIdx == -1) return false;

        std::vector<double> dist(n, INF);
        std::vector<bool> closed(n, false);
        std::vector<int> prev(n, -1);

        MinHeap pq;
        dist[srcIdx] = 0.0;
        pq.insert(cleanSrc, 0.0);

        outVisited = 0;
        bool found = false;

        while (!pq.isEmpty()) {
            HeapNode top = pq.extractMin();
            int uIdx = graph.findVertexIndex(top.name);
            if (uIdx == -1 || closed[uIdx]) continue;

            closed[uIdx] = true;
            ++outVisited;

            if (top.name == target) {
                found = true;
                break;
            }

            EdgeNode* e = graph.getNeighbors(uIdx);
            while (e != nullptr) {
                if (!blocked.isBlocked(top.name, e->dest) && !isEdgeRestricted(top.name, e->dest, src, target, blocked)) {
                    int vIdx = graph.findVertexIndex(e->dest);
                    if (vIdx != -1 && !closed[vIdx]) {
                        double roadMult = getRoadMultiplier(graph, top.name, e->dest);
                        double turnPen = getTurnPenalty(graph, prev[uIdx], uIdx, vIdx);
                        double stepCost = (e->weight * roadMult) + turnPen;
                        double alt = dist[uIdx] + stepCost;
                        if (alt < dist[vIdx]) {
                            dist[vIdx] = alt;
                            prev[vIdx] = uIdx;
                            pq.insert(e->dest, alt);
                        }
                    }
                }
                e = e->next;
            }
        }

        if (!found || dist[targetIdx] >= INF) return false;

        outPath.clear();
        int curr = targetIdx;
        while (curr != -1) {
            outPath.push_back(graph.getVertexName(curr));
            curr = prev[curr];
        }
        std::reverse(outPath.begin(), outPath.end());

        double totalPhysical = 0.0;
        for (size_t i = 0; i + 1 < outPath.size(); ++i) {
            double w = graph.getEdgeWeight(outPath[i], outPath[i + 1]);
            totalPhysical += (w >= 0 ? w : 0.0);
        }
        outDist = totalPhysical;
        return true;
    };

    double d1 = INF, d2 = INF;
    std::vector<std::string> p1, p2;
    int v1 = 0, v2 = 0;

    bool ok1 = runCoreDijk(t1, d1, p1, v1);
    bool ok2 = (!t2.empty()) && runCoreDijk(t2, d2, p2, v2);

    if (ok1 && ok2) {
        if (d1 <= d2) {
            metric.found = true; metric.totalDistance = d1; metric.path = p1;
        } else {
            metric.found = true; metric.totalDistance = d2; metric.path = p2;
        }
        metric.visitedCount = v1 + v2;
    } else if (ok1) {
        metric.found = true; metric.totalDistance = d1; metric.path = p1; metric.visitedCount = v1;
    } else if (ok2) {
        metric.found = true; metric.totalDistance = d2; metric.path = p2; metric.visitedCount = v2;
    } else {
        metric.found = false;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    metric.executionTimeUs = std::chrono::duration<double, std::micro>(tEnd - tStart).count();

    return metric;
}

AlgorithmMetric AStar::runBFSMetric(
    const Graph& graph,
    const std::string& src,
    const std::string& dest,
    const BlockedEdgeList& blocked
) {
    AlgorithmMetric metric;
    metric.algorithmName = "BFS (Breadth-First Search)";

    auto tStart = std::chrono::high_resolution_clock::now();

    std::string t1, t2;
    resolveTarget(dest, t1, t2);

    auto runCoreBFS = [&](const std::string& target, double& outDist, std::vector<std::string>& outPath, int& outVisited) -> bool {
        int n = graph.getNumVertices();
        int srcIdx = graph.findVertexIndex(src);
        int targetIdx = graph.findVertexIndex(target);
        if (srcIdx == -1 || targetIdx == -1) return false;

        std::vector<bool> visited(n, false);
        std::vector<int> prev(n, -1);
        std::queue<int> q;

        visited[srcIdx] = true;
        q.push(srcIdx);

        outVisited = 0;
        bool found = false;

        while (!q.empty()) {
            int uIdx = q.front();
            q.pop();
            ++outVisited;

            std::string uName = graph.getVertexName(uIdx);
            if (uName == target) {
                found = true;
                break;
            }

            EdgeNode* e = graph.getNeighbors(uIdx);
            while (e != nullptr) {
                if (!blocked.isBlocked(uName, e->dest) && !isEdgeRestricted(uName, e->dest, src, target, blocked)) {
                    int vIdx = graph.findVertexIndex(e->dest);
                    if (vIdx != -1 && !visited[vIdx]) {
                        visited[vIdx] = true;
                        prev[vIdx] = uIdx;
                        q.push(vIdx);
                    }
                }
                e = e->next;
            }
        }

        if (!found) return false;

        outPath.clear();
        int curr = targetIdx;
        while (curr != -1) {
            outPath.push_back(graph.getVertexName(curr));
            curr = prev[curr];
        }
        std::reverse(outPath.begin(), outPath.end());

        // Tính khoảng cách thực tế của lộ trình BFS
        double distSum = 0.0;
        for (size_t i = 0; i + 1 < outPath.size(); ++i) {
            distSum += graph.getEdgeWeight(outPath[i], outPath[i + 1]);
        }
        outDist = distSum;
        return true;
    };

    double d1 = INF, d2 = INF;
    std::vector<std::string> p1, p2;
    int v1 = 0, v2 = 0;

    bool ok1 = runCoreBFS(t1, d1, p1, v1);
    bool ok2 = (!t2.empty()) && runCoreBFS(t2, d2, p2, v2);

    if (ok1 && ok2) {
        if (d1 <= d2) {
            metric.found = true; metric.totalDistance = d1; metric.path = p1;
        } else {
            metric.found = true; metric.totalDistance = d2; metric.path = p2;
        }
        metric.visitedCount = v1 + v2;
    } else if (ok1) {
        metric.found = true; metric.totalDistance = d1; metric.path = p1; metric.visitedCount = v1;
    } else if (ok2) {
        metric.found = true; metric.totalDistance = d2; metric.path = p2; metric.visitedCount = v2;
    } else {
        metric.found = false;
    }

    auto tEnd = std::chrono::high_resolution_clock::now();
    metric.executionTimeUs = std::chrono::duration<double, std::micro>(tEnd - tStart).count();

    return metric;
}

BenchmarkResult AStar::benchmark(
    const Graph& graph,
    const std::string& src,
    const std::string& dest,
    const BlockedEdgeList& blocked,
    double heuristicScale
) {
    BenchmarkResult bench;
    bench.startNode = src;
    bench.destNode = dest;

    // Đo Dijkstra
    bench.dijkstra = runDijkstraMetric(graph, src, dest, blocked);

    // Đo A*
    auto tStart = std::chrono::high_resolution_clock::now();
    AStarResult astarRes = run(graph, src, dest, blocked, heuristicScale);
    auto tEnd = std::chrono::high_resolution_clock::now();

    bench.astar.algorithmName = "A* (Euclidean Heuristic)";
    bench.astar.found = astarRes.found;
    bench.astar.totalDistance = astarRes.totalDistance;
    bench.astar.visitedCount = astarRes.visitedNodes;
    bench.astar.path = astarRes.path;
    bench.astar.executionTimeUs = std::chrono::duration<double, std::micro>(tEnd - tStart).count();

    // Đo BFS
    bench.bfs = runBFSMetric(graph, src, dest, blocked);

    return bench;
}

void AStar::printBenchmark(const BenchmarkResult& bench) {
    std::cout << "\n========================================================================================\n";
    std::cout << "  BENCHMARK SO SÁNH HIỆU NĂNG THUẬT TOÁN: " << bench.startNode << " -> " << bench.destNode << "\n";
    std::cout << "========================================================================================\n";
    std::cout << std::left 
              << std::setw(28) << "Thuật toán"
              << std::setw(18) << "Quãng đường (m)"
              << std::setw(18) << "Đỉnh duyệt (node)"
              << std::setw(20) << "Thời gian (us)"
              << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";

    auto printRow = [](const AlgorithmMetric& m) {
        std::cout << std::left 
                  << std::setw(28) << m.algorithmName;
        if (m.found) {
            std::cout << std::setw(18) << (std::to_string(m.totalDistance).substr(0, 5) + " m")
                      << std::setw(18) << (std::to_string(m.visitedCount) + " đỉnh")
                      << std::setw(20) << (std::to_string(m.executionTimeUs).substr(0, 7) + " us");
        } else {
            std::cout << std::setw(18) << "N/A"
                      << std::setw(18) << "N/A"
                      << std::setw(20) << "N/A";
        }
        std::cout << "\n";
    };

    printRow(bench.dijkstra);
    printRow(bench.astar);
    printRow(bench.bfs);

    std::cout << "----------------------------------------------------------------------------------------\n";
    std::cout << "  [NHẬN XÉT GIẢI THUẬT]:\n";
    std::cout << "  - Dijkstra: Đảm bảo khoảng cách ngắn nhất chuẩn xác trên đồ thị có trọng số.\n";
    std::cout << "  - A*: Dùng hàm Heuristic hướng đích h(u), giảm đáng kể số đỉnh duyệt (" 
              << bench.astar.visitedCount << " so với " << bench.dijkstra.visitedCount << " của Dijkstra).\n";
    std::cout << "  - BFS: Tối ưu số cạnh (hop count) nhưng không xét trọng số mét, duyệt loang nhiều đỉnh ("
              << bench.bfs.visitedCount << " đỉnh).\n";
    std::cout << "========================================================================================\n";
}
