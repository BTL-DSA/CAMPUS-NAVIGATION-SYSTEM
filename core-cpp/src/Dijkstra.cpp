#include "Dijkstra.h"
#include <iomanip>
#include <stdexcept>
#include <cmath>
#include <vector>

namespace {
    [[maybe_unused]] bool isClusterC1C2(const std::string& id) {
        return (id == "K" || id == "C2" || id == "NHÀ C2" || id == "PHÒNG C2" ||
                id == "C1_J" || id == "C1" || id == "NHÀ C1" || id == "HỘI TRƯỜNG C1" ||
                id == "C1_SANH" || id == "C1_Sanh" || id == "C1_WEST" || id == "C1_Front" || id == "C1_FRONT");
    }

    bool isClusterC(const std::string& id) {
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

    bool isClusterCEdge(const std::string& u, const std::string& v) {
        if ((u == "T" && v == "J") || (u == "J" && v == "T")) return true;
        if ((u == "J" && v == "K") || (u == "K" && v == "J")) return true;
        if ((u == "K" && v == "C1_Front") || (u == "C1_Front" && v == "K")) return true;
        if ((u == "C1_Front" && v == "V3") || (u == "V3" && v == "C1_Front")) return true;
        if ((u == "C1" && v == "V4") || (u == "V4" && v == "C1")) return true;
        if ((u == "C1" && v == "V4_S") || (u == "V4_S" && v == "C1")) return true;
        if ((u == "V3" && v == "C2_Front") || (u == "C2_Front" && v == "V3")) return true;
        if ((u == "V3" && v == "V4") || (u == "V4" && v == "V3")) return true;
        if ((u == "V3" && v == "V4_S") || (u == "V4_S" && v == "V3")) return true;
        if ((u == "V4" && v == "H3") || (u == "H3" && v == "V4")) return true;
        if ((u == "H3" && v == "R2") || (u == "R2" && v == "H3")) return true;
        if ((u == "R2" && v == "D3") || (u == "D3" && v == "R2")) return true;
        if ((u == "D3" && v == "X1") || (u == "X1" && v == "D3")) return true;
        if ((u == "X1" && v == "D2") || (u == "D2" && v == "X1")) return true;
        if ((u == "D2" && v == "D1") || (u == "D1" && v == "D2")) return true;
        if ((u == "J" && v == "V1") || (u == "V1" && v == "J")) return true;
        if ((u == "V1" && v == "C2_Front") || (u == "C2_Front" && v == "V1")) return true;
        if ((u == "C2_Front" && v == "V5") || (u == "V5" && v == "C2_Front")) return true;
        if ((u == "V5" && v == "V3") || (u == "V3" && v == "V5")) return true;
        if ((u == "V1" && v == "H1") || (u == "H1" && v == "V1")) return true;
        if ((u == "V5" && v == "H2") || (u == "H2" && v == "V5")) return true;
        if ((u == "V4" && v == "V4_S") || (u == "V4_S" && v == "V4")) return true;
        if ((u == "V4" && v == "CT") || (u == "CT" && v == "V4")) return true;
        if ((u == "H2" && v == "DN") || (u == "DN" && v == "H2")) return true;
        return false;
    }

    double getRoadMultiplier(const Graph& graph, const std::string& u, const std::string& v, bool isClusterRoute = false) {
        if (isClusterRoute && isClusterCEdge(u, v)) return 0.9;
        std::string rtype = graph.getEdgeRoadType(u, v);
        if (rtype == "main" || rtype == "major") return 0.8;
        if (rtype == "internal" || rtype == "minor") return 1.4;
        return 1.0;
    }

    bool isEdgeRestricted(const std::string& u, const std::string& v, const std::string& src, const std::string& dest, const BlockedEdgeList& blocked, bool isMultiStop = false) {
        if (isMultiStop) return false;

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
        bool isX1D1 = (u == "X1" && v == "D1") || (u == "D1" && v == "X1");
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

        bool destIsClusterC = isClusterC(dest);
        bool srcIsClusterC  = isClusterC(src);

        // 0. Kịch bản C2 lên Giảng đường Đa Năng (DN): Cấm đi vòng chữ U qua V4 -> H3
        bool srcIsC2 = src == "C2" || src == "K" || src == "C2_Front" || src == "C1_Front" || src == "C1_J" || src == "NHÀ C2";
        bool destIsC2 = dest == "C2" || dest == "K" || dest == "C2_Front" || dest == "C1_Front" || dest == "C1_J" || dest == "NHÀ C2";
        bool destIsDN = dest == "DN" || dest == "GIẢNG ĐƯỜNG ĐA NĂNG" || dest == "H2";
        bool srcIsDN = src == "DN" || src == "GIẢNG ĐƯỜNG ĐA NĂNG" || src == "H2";

        if ((srcIsC2 && destIsDN) || (srcIsDN && destIsC2)) {
            if (isV4H3 || isH2H3) return true;
        }

        // 0b. Kịch bản Giảng đường Đa Năng <-> Nhà E1: Đi theo lối Đông Đa Năng lên Đường Sáng Tạo (như Ảnh 2 đường màu tím)
        bool destIsE1 = (dest == "E1" || dest == "NHÀ E1" || dest == "E1_Mid" || dest == "E1_MID" || dest == "KHOA CNTT" || dest == "A2");
        bool srcIsE1 = (src == "E1" || src == "NHÀ E1" || src == "E1_Mid" || src == "E1_MID" || src == "KHOA CNTT" || src == "A2");

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

        // 1. Kịch bản Cụm C (C1, C2, C3, CT...): Tuyến chuẩn qua trục nội bộ C (T -> J)
        if (destIsClusterC || srcIsClusterC) {
            bool isToC1 = (dest == "C1" || dest == "NHÀ C1" || dest == "HỘI TRƯỜNG C1" || dest == "C1_SANH");
            bool isFromC1 = (src == "C1" || src == "NHÀ C1" || src == "HỘI TRƯỜNG C1" || src == "C1_SANH");
            if (isToC1 || isFromC1) {
                if ((u == "J" && v == "K") || (u == "K" && v == "J")) return true;
                if ((u == "C1_Front" && v == "V3") || (u == "V3" && v == "C1_Front")) return true;
            }
            if (isTW1) {
                bool tjBlocked = blocked.isBlocked("T", "J");
                if (!tjBlocked) return true; // Chặn T-W1 để bắt buộc đi tuyến nội bộ T -> J
            }
            return false;
        }

        // 2. Đi đến các tòa ngoài Cụm C (Khu E, PZ...): Ưu tiên trục lớn T -> W1
        if (isTJ) {
            bool tw1Blocked = blocked.isBlocked("T", "W1");
            if (!tw1Blocked) return true;
        }

        // 3. Kịch bản Nhà E1 (Đường Sáng Tạo & Ánh Đạo): Tuyến chuẩn G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> E1
        if (dest == "E1" || dest == "A2" || dest == "NHÀ E1" || dest == "KHOA CNTT" || dest == "E1_Mid" || dest == "E1_MID") {
            if (isA2A1) {
                return true; // Chặn A2-A1 để không bao giờ đi từ A1 xuống A2
            }
            if (isW1H1) {
                return true; // Bắt buộc đi dọc Đại lộ Thành Công W1 -> W2 rồi rẽ vào Đường Sáng Tạo S1
            }
        }

        // 4. Kịch bản Nhà E6 (Đường Sáng Tạo & Ánh Đạo): Tuyến chuẩn G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> A1 -> E6_W
        if (dest == "E6" || dest == "E6_W" || dest == "NHÀ E6") {
            if (isW1H1) {
                return true; // Bắt buộc đi dọc Đại lộ Thành Công W1 -> W2 rồi rẽ vào Đường Sáng Tạo S1
            }
            if (isW2W3) {
                return true; // Tuyệt đối không chạy vòng lên hết Đường 449 qua Công viên Nghĩa tình
            }
            if (isS1E7) {
                return true; // Bắt buộc đi thẳng S1 -> S2
            }
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) {
                return true; // Không đi xuyên qua hành lang Nhà E1
            }
        }
        if (src == "E6" || src == "E6_W" || src == "NHÀ E6") {
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) {
                return true;
            }
        }
        if (dest == "E9" || dest == "NHÀ E9") {
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) {
                return true;
            }
        }
        if (src == "E9" || src == "NHÀ E9") {
            if ((u == "E4w" && v == "E1_Mid") || (u == "E1_Mid" && v == "E4w")) {
                return true;
            }
        }

        // 5. Kịch bản các tòa nhà E Tây-Bắc khác (E7, E10):
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
            if (isW1H1) {
                return true;
            }
        }

        // 6. Kịch bản Nhà E3, E4, E5 (Lối Tây qua nút Q và phân luồng cửa vào):
        bool destIsEWest = isEWestEntry(dest);
        bool destIsEEast = isEastAxis(dest);
        bool isEBuilding = destIsEWest || destIsEEast || dest == "E3" || dest == "E4" || dest == "E5";
        if (isEBuilding) {
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
                if (destIsEWest) {
                    if (isW2S1) return true;
                    if (isH2S2) return true;
                    bool isA2E1Mid = (u == "A2" && (v == "E1_Mid" || v == "E1")) || (v == "A2" && (u == "E1_Mid" || u == "E1"));
                    if (isE1Z4 || isA2E1Mid) return true;
                    if (isH3R2 || isQR2) return true;
                    if (isZ6Z5) return true;
                } else {
                    if (isH1S1) return true;
                    if (isQR2) return true;
                    if (isZ6Z5) return true;
                }
            } else if (isClusterC(src) || src == "H2" || src == "DN") {
                if (isQR2) return true;
                if (isZ6Z5) return true;
            } else if (src == "E10" || src == "NHÀ E10" || src == "A0" || src == "NGÃ TƯ A0") {
                if (isS2Q || isA2S2) return true;
                if (src == "E10" || src == "NHÀ E10") {
                    if (isXEE10 || isA0A1) return true;
                    if ((u == "A0" && v == "D0") || (u == "D0" && v == "A0") ||
                        (u == "E6" && v == "D0") || (u == "D0" && v == "E6")) return true;
                }
            }
        }

        if (dest == "E5" || dest == "NHÀ E5" || dest == "E5w") {
            if (src != "E10" && src != "NHÀ E10" && src != "A0") {
                if (isZ6Z5) return true;
            }
        }
        // 7. Kịch bản Nhà E8 / E8B:
        if (isE8(dest)) {
            // Không đi xuyên qua Nhà để xe PZ (trừ khi xuất phát từ chính Nhà để xe)
            bool isParking = (u == "PZ" || v == "PZ" || u == "P*" || v == "P*");
            if (src != "PZ" && src != "P*" && isParking) return true;

            // Từ E7 đi E8: đi thẳng theo trục Đường Sáng Tạo S1 -> S2 -> Q -> DN_NE -> R2 -> D3 -> X1 -> E8
            // Cấm rẽ xuống H1 vòng qua Giảng đường Đa Năng
            if (src == "E7" || src == "NHÀ E7" || src == "S1" || src == "S2") {
                if (isH1S1) return true;
            } else {
                if (isQR2) {
                    return true; // Chặn ngõ hẹp sau lưng E3/E4
                }
            }
            if (src == "E10" || src == "NHÀ E10") {
                if (isXEE10 || isA0A1 || isZ6Z5 || isA2A1) {
                    return true; // Từ E10 sang E8 đi thẳng qua trục ngang phía Bắc A0 -> D0 -> Đường Hạnh Hợp
                }
            }
            if ((u == "X1" && (v == "D2" || v == "D1")) || (v == "X1" && (u == "D2" || u == "D1"))) {
                bool isNorthSource = (src == "E10" || src == "NHÀ E10" || src == "D1" || src == "D0" || src == "E9" || src == "NHÀ E9" ||
                                      src == "E6" || src == "E6_W" || src == "NHÀ E6" || src == "A0" || src == "A1" || src == "E6" || src == "E9");
                if (!isNorthSource) {
                    return true; // Tuyệt đối không đi lố từ X1 lên D2/D1 rồi mới sang E8
                }
            }
            if (src == "G" || src == "T" || src == "W1" || src == "H1") {
                if (isH1S1) {
                    return true; // Chặn rẽ vào S1, ép đi thẳng trục chính Hội Nhập H1 -> H2 -> H3
                }
                if (isD2D1 || isX1D1) {
                    return true; // Tuyến từ Cổng phía Nam không đi qua D1
                }
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

// ============================================================
//  Hằng số
// ============================================================
const double Dijkstra::INF = 1e18;

// ============================================================
//  BlockedEdgeList — Danh sách cạnh bị chặn (liên kết đơn thuần)
// ============================================================

BlockedEdgeList::BlockedEdgeList() : head(nullptr), count(0) {}

BlockedEdgeList::~BlockedEdgeList() {
    freeMemory();
}

void BlockedEdgeList::freeMemory() {
    BlockedEdge* curr = head;
    while (curr != nullptr) {
        BlockedEdge* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    head = nullptr;
    count = 0;
}

void BlockedEdgeList::copyFrom(const BlockedEdgeList& other) {
    head = nullptr;
    count = 0;
    if (other.head == nullptr) return;

    // Sao chép theo thứ tự giữ nguyên (đi qua 2 lần để giữ thứ tự)
    // Lần 1: đếm
    int n = 0;
    BlockedEdge* p = other.head;
    while (p) { ++n; p = p->next; }

    // Lần 2: sao chép dùng mảng tạm con trỏ trên stack
    // (n tối đa nhỏ nên dùng mảng VLA tĩnh 256 phần tử đủ dùng)
    const int MAX_BLOCKED = 256;
    if (n > MAX_BLOCKED) n = MAX_BLOCKED; // bảo vệ tràn
    BlockedEdge* buf[MAX_BLOCKED];
    p = other.head;
    for (int i = 0; i < n && p; ++i, p = p->next) buf[i] = p;

    for (int i = n - 1; i >= 0; --i) {
        head = new BlockedEdge(buf[i]->u, buf[i]->v, head);
        ++count;
    }
}

BlockedEdgeList::BlockedEdgeList(const BlockedEdgeList& other) : head(nullptr), count(0) {
    copyFrom(other);
}

BlockedEdgeList& BlockedEdgeList::operator=(const BlockedEdgeList& other) {
    if (this != &other) {
        freeMemory();
        copyFrom(other);
    }
    return *this;
}

void BlockedEdgeList::block(const std::string& u, const std::string& v) {
    if (isBlocked(u, v)) return; // Đã tồn tại, không thêm trùng
    head = new BlockedEdge(u, v, head);
    ++count;
}

void BlockedEdgeList::unblock(const std::string& u, const std::string& v) {
    BlockedEdge* prev = nullptr;
    BlockedEdge* curr = head;
    while (curr != nullptr) {
        bool match = (curr->u == u && curr->v == v) ||
                     (curr->u == v && curr->v == u);
        if (match) {
            if (prev) prev->next = curr->next;
            else      head = curr->next;
            delete curr;
            --count;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

bool BlockedEdgeList::isBlocked(const std::string& u, const std::string& v) const {
    BlockedEdge* curr = head;
    while (curr != nullptr) {
        if ((curr->u == u && curr->v == v) ||
            (curr->u == v && curr->v == u)) {
            return true;
        }
        // Tương thích ngược: Nếu chặn toàn tuyến Q-R2 thì các đoạn con Q-DN_NE và DN_NE-R2 cũng bị chặn
        if ((curr->u == "Q" && curr->v == "R2") || (curr->u == "R2" && curr->v == "Q")) {
            if ((u == "Q" && v == "DN_NE") || (u == "DN_NE" && v == "Q") ||
                (u == "DN_NE" && v == "R2") || (u == "R2" && v == "DN_NE")) {
                return true;
            }
        }
        // Tương thích ngược: Nếu chặn H2-H3 thì các đoạn con H2-DN_SE và DN_SE-H3 cũng bị chặn
        if ((curr->u == "H2" && curr->v == "H3") || (curr->u == "H3" && curr->v == "H2")) {
            if ((u == "H2" && v == "DN_SE") || (u == "DN_SE" && v == "H2") ||
                (u == "DN_SE" && v == "H3") || (u == "H3" && v == "DN_SE")) {
                return true;
            }
        }
        curr = curr->next;
    }
    return false;
}

void BlockedEdgeList::clear() {
    freeMemory();
}

int BlockedEdgeList::getCount() const {
    return count;
}

// ============================================================
//  DijkstraResult — Kết quả thuật toán
// ============================================================

DijkstraResult::DijkstraResult()
    : found(false), totalDistance(0.0), startNode(""), entryNode(""), path(nullptr), stepCount(0) {}

DijkstraResult::~DijkstraResult() {
    freeMemory();
}

void DijkstraResult::freeMemory() {
    PathStep* curr = path;
    while (curr != nullptr) {
        PathStep* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    path = nullptr;
    stepCount = 0;
}

void DijkstraResult::copyFrom(const DijkstraResult& other) {
    found = other.found;
    totalDistance = other.totalDistance;
    startNode = other.startNode;
    entryNode = other.entryNode;
    stepCount = other.stepCount;
    path = nullptr;

    if (other.path == nullptr) return;

    // Thu thập các bước vào mảng tạm
    const int MAX_STEPS = 512;
    PathStep* buf[MAX_STEPS];
    int n = 0;
    PathStep* p = other.path;
    while (p && n < MAX_STEPS) {
        buf[n++] = p;
        p = p->next;
    }

    // Xây dựng lại theo thứ tự (reversed insert)
    for (int i = n - 1; i >= 0; --i) {
        path = new PathStep(buf[i]->nodeId, buf[i]->displayName,
                            buf[i]->cumulativeDist, buf[i]->stepDist, path);
    }
}

DijkstraResult::DijkstraResult(const DijkstraResult& other) : path(nullptr), stepCount(0) {
    copyFrom(other);
}

DijkstraResult& DijkstraResult::operator=(const DijkstraResult& other) {
    if (this != &other) {
        freeMemory();
        copyFrom(other);
    }
    return *this;
}

void DijkstraResult::print(const std::string& srcId, const std::string& destId) const {
    std::cout << "\n";
    std::cout << "┌────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│             KẾT QUẢ TÌM ĐƯỜNG NGẮN NHẤT (DIJKSTRA)           │\n";
    std::cout << "└────────────────────────────────────────────────────────────────┘\n";

    if (!srcId.empty()) {
        std::cout << "  Xuất phát : " << srcId;
        if (!startNode.empty() && startNode != srcId)
            std::cout << "  (sảnh xuất phát: " << startNode << ")";
        std::cout << "\n";
    }
    if (!destId.empty()) std::cout << "  Đích đến  : " << destId;
    if (!entryNode.empty() && entryNode != destId)
        std::cout << "  (cửa vào: " << entryNode << ")";
    std::cout << "\n";

    if (!found) {
        std::cout << "  [!] Không tìm thấy đường đi!\n";
        return;
    }

    std::cout << "  Tổng cự ly: " << std::fixed << std::setprecision(1)
              << totalDistance << " mét\n";
    std::cout << "  Số bước   : " << stepCount << "\n";
    std::cout << "  ──────────────────────────────────────────────────────────────\n";

    int step = 1;
    PathStep* curr = path;
    while (curr != nullptr) {
        std::cout << "  [" << std::setw(2) << step++ << "] "
                  << std::left << std::setw(5) << curr->nodeId
                  << "  " << std::setw(30) << curr->displayName;
        if (curr->stepDist > 0)
            std::cout << "  (+" << std::fixed << std::setprecision(0)
                      << curr->stepDist << "m  | tổng: "
                      << curr->cumulativeDist << "m)";
        std::cout << "\n";
        curr = curr->next;
    }
    std::cout << "  ──────────────────────────────────────────────────────────────\n\n";
}

// ============================================================
//  Dijkstra — resolveTarget: xử lý tên tòa nhà 2 lối vào
// ============================================================

void Dijkstra::resolveTarget(const std::string& dest,
                              std::string& out1,
                              std::string& out2) {
    out2 = ""; // mặc định không có cửa thứ hai

    if (dest == "E3" || dest == "NHÀ E3") {
        out1 = "D3";  // Lối Đông (Đường Hạnh Hộp / Hội Nhập)
        out2 = "E3w"; // Lối Tây (Hành lang Z3)
    } else if (dest == "E3w" || dest == "E3W" || dest == "E3_WEST" || dest == "E3_West") {
        out1 = "E3w";
        out2 = "";
    } else if (dest == "E3_EAST" || dest == "E3_East" || dest == "E3_E" || dest == "D3") {
        out1 = "D3";
        out2 = "";
    } else if (dest == "E4" || dest == "NHÀ E4") {
        out1 = "D2";  // Lối Đông
        out2 = "E4w"; // Lối Tây
    } else if (dest == "E4w" || dest == "E4W" || dest == "E4_WEST" || dest == "E4_West") {
        out1 = "E4w";
        out2 = "";
    } else if (dest == "E4_EAST" || dest == "E4_East" || dest == "E4_E" || dest == "D2") {
        out1 = "D2";
        out2 = "";
    } else if (dest == "E5" || dest == "NHÀ E5") {
        out1 = "D1";  // Lối Đông
        out2 = "E5w"; // Lối Tây
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
    } else if (dest == "CT" || dest == "CĂN TIN" || dest == "CANTEEN" || dest == "CĂN TIN SINH VIÊN") {
        out1 = "V4";
    } else if (dest == "C2" || dest == "NHÀ C2" || dest == "NHAC2" || dest == "PHÒNG C2") {
        out1 = "C2_Front";
        out2 = "K";
    } else if (dest == "DN" || dest == "ĐA NĂNG" || dest == "GIẢNG ĐƯỜNG ĐA NĂNG" || dest == "ĐN") {
        out1 = "DN";
    } else if (dest == "C3" || dest == "NHÀ C3" || dest == "THƯ VIỆN") {
        out1 = "V4_S";
    } else if (dest == "C1" || dest == "NHÀ C1" || dest == "HỘI TRƯỜNG C1" || dest == "C1_SANH" || dest == "C1_Sanh" || dest == "C1_EAST" || dest == "C1_East") {
        out1 = "C1";
    } else if (dest == "C1_WEST" || dest == "C1_Front" || dest == "C1_FRONT") {
        out1 = "C1_Front";
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
    } else {
        out1 = dest;
    }
}

// ============================================================
//  Dijkstra — resolveOrigin: xử lý điểm xuất phát đa sảnh (C2)
// ============================================================

void Dijkstra::resolveOrigin(const std::string& src,
                              std::string& out1,
                              std::string& out2) {
    out2 = ""; // mặc định không có sảnh thứ hai

    if (src == "C2" || src == "NHÀ C2" || src == "NHAC2" || src == "PHÒNG C2") {
        out1 = "C2_Front"; // Sảnh trước nối lên Đa năng (Bắc)
        out2 = "K";        // Sảnh sau nối sang J/C1 (Nam)
    } else if (src == "DN" || src == "ĐA NĂNG" || src == "GIẢNG ĐƯỜNG ĐA NĂNG" || src == "ĐN") {
        out1 = "DN";
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
    } else {
        out1 = src;
    }
}

// ============================================================
//  Dijkstra — runCore: hạt nhân thuật toán
// ============================================================

bool Dijkstra::runCore(
        const Graph& graph,
        const std::string& src,
        const std::string& target,
        const BlockedEdgeList& blocked,
        double* distOut,
        int* prevOut,
        bool isMultiStop) {

    int n = graph.getNumVertices();
    int srcIdx = graph.findVertexIndex(src);
    int tgtIdx = graph.findVertexIndex(target);

    if (srcIdx == -1 || tgtIdx == -1 || n == 0) return false;

    // Khởi tạo mảng khoảng cách và mảng cha
    for (int i = 0; i < n; ++i) {
        distOut[i] = INF;
        prevOut[i] = -1;
    }
    distOut[srcIdx] = 0.0;

    // Hàng đợi ưu tiên Min-Heap thuần (không dùng std::priority_queue)
    MinHeap pq;
    pq.insert(src, 0.0);

    while (!pq.isEmpty()) {
        HeapNode top = pq.extractMin();
        int uIdx = graph.findVertexIndex(top.name);

        if (uIdx == -1) continue;
        // Bỏ qua nếu khoảng cách trong heap đã lỗi thời (lazy deletion)
        if (top.distance > distOut[uIdx] + 1e-9) continue;

        // Đến đích rồi, dừng sớm
        if (top.name == target) break;

        // Thư giãn (Relaxation) các cạnh kề
        EdgeNode* edge = graph.getNeighbors(uIdx);
        while (edge != nullptr) {
            bool isCluster = isClusterC(src) || isClusterC(target);
            if (!blocked.isBlocked(top.name, edge->dest) && !isEdgeRestricted(top.name, edge->dest, src, target, blocked, isMultiStop)) {
                int vIdx = graph.findVertexIndex(edge->dest);
                if (vIdx != -1) {
                    double roadMult = getRoadMultiplier(graph, top.name, edge->dest, isCluster);
                    double turnPen = getTurnPenalty(graph, prevOut[uIdx], uIdx, vIdx);
                    double stepCost = (edge->weight * roadMult) + turnPen;
                    double newDist = distOut[uIdx] + stepCost;
                    if (newDist < distOut[vIdx] - 1e-9) {
                        distOut[vIdx] = newDist;
                        prevOut[vIdx] = uIdx;
                        // Lazy insert: chèn lại với giá trị mới thay vì decreaseKey O(N)
                        pq.insert(edge->dest, newDist);
                    }
                }
            }
            edge = edge->next;
        }
    }

    return distOut[tgtIdx] < INF;
}

// ============================================================
//  Dijkstra — buildPath: truy vết lộ trình từ mảng prev[]
// ============================================================

PathStep* Dijkstra::buildPath(
        const Graph& graph,
        int targetIdx,
        const double* /*distArr*/,
        const int* prevArr,
        int& outStepCount,
        double* outTotalDist) {

    // Thu thập thứ tự ngược (target → src)
    const int MAX_PATH = 512;
    int pathIdx[MAX_PATH];
    int len = 0;

    int cur = targetIdx;
    while (cur != -1 && len < MAX_PATH) {
        pathIdx[len++] = cur;
        cur = prevArr[cur];
    }

    outStepCount = len;
    if (len == 0) {
        if (outTotalDist) *outTotalDist = 0.0;
        return nullptr;
    }

    // Tính khoảng cách mét thực tế từ src đến target theo mảng pathIdx
    // pathIdx[0] là target, pathIdx[len-1] là src
    double realCumDist[MAX_PATH];
    realCumDist[len - 1] = 0.0;
    for (int i = len - 2; i >= 0; --i) {
        int u = pathIdx[i + 1];
        int v = pathIdx[i];
        double w = graph.getEdgeWeight(graph.getVertexName(u), graph.getVertexName(v));
        realCumDist[i] = realCumDist[i + 1] + (w >= 0 ? w : 0.0);
    }

    if (outTotalDist) {
        *outTotalDist = realCumDist[0];
    }

    // Xây chuỗi PathStep theo thứ tự đúng (src → target)
    // Dùng kỹ thuật chèn đầu để đảo ngược mảng
    PathStep* head = nullptr;
    for (int i = 0; i < len; ++i) {
        int idx = pathIdx[i]; // đang đi ngược, i=0 là target
        std::string nodeId = graph.getVertexName(idx);
        std::string dName  = graph.getDisplayName(nodeId);

        double cumDist  = realCumDist[i];
        double stepDist = (i < len - 1) ? (realCumDist[i] - realCumDist[i + 1]) : 0.0;

        // Chèn vào đầu danh sách → danh sách sẽ ở thứ tự src → target
        head = new PathStep(nodeId, dName, cumDist, stepDist, head);
    }

    return head;
}

// ============================================================
//  Dijkstra::run — Hàm công khai chính
// ============================================================

DijkstraResult Dijkstra::run(
        const Graph& graph,
        const std::string& src,
        const std::string& dest,
        const BlockedEdgeList& blocked,
        bool isMultiStop) {

    DijkstraResult result;
    int n = graph.getNumVertices();

    if (n == 0 || n > MAX_VERTICES) {
        std::cerr << "[Dijkstra] Đồ thị rỗng hoặc vượt MAX_VERTICES!\n";
        return result;
    }

    // --- Giải quyết điểm xuất phát (hỗ trợ đa sảnh C2: V5 và K) ---
    std::string s1, s2;
    resolveOrigin(src, s1, s2);

    // --- Giải quyết điểm đích (dual-entry hoặc trực tiếp) ---
    std::string t1, t2;
    resolveTarget(dest, t1, t2);

    std::vector<std::string> validOrigins;
    if (graph.hasVertex(s1)) validOrigins.push_back(s1);
    if (!s2.empty() && graph.hasVertex(s2)) validOrigins.push_back(s2);
    if (validOrigins.empty()) {
        if (graph.hasVertex(src)) validOrigins.push_back(src);
        else return result;
    }

    std::vector<std::string> validTargets;
    if (graph.hasVertex(t1)) validTargets.push_back(t1);
    if (!t2.empty() && graph.hasVertex(t2)) validTargets.push_back(t2);
    if (validTargets.empty()) {
        if (graph.hasVertex(dest)) validTargets.push_back(dest);
        else return result;
    }

    double* dist = new double[n];
    int*    prev = new int[n];

    double bestDist = INF;
    int bestStepCount = 0;
    PathStep* bestPath = nullptr;
    std::string bestStart = "";
    std::string bestTarget = "";

    for (const auto& curSrc : validOrigins) {
        for (const auto& curTgt : validTargets) {
            bool found = runCore(graph, curSrc, curTgt, blocked, dist, prev, isMultiStop);
            int tgtIdx = graph.findVertexIndex(curTgt);
            if (found && tgtIdx != -1) {
                double curDist = INF;
                int curSteps = 0;
                PathStep* curPath = buildPath(graph, tgtIdx, dist, prev, curSteps, &curDist);
                if (curPath && curDist < bestDist - 1e-9) {
                    while (bestPath) {
                        PathStep* tmp = bestPath;
                        bestPath = bestPath->next;
                        delete tmp;
                    }
                    bestPath = curPath;
                    bestDist = curDist;
                    bestStepCount = curSteps;
                    bestStart = curSrc;
                    bestTarget = curTgt;
                } else {
                    while (curPath) {
                        PathStep* tmp = curPath;
                        curPath = curPath->next;
                        delete tmp;
                    }
                }
            }
        }
    }

    delete[] dist;
    delete[] prev;

    if (bestPath != nullptr && bestDist < INF) {
        result.found = true;
        result.startNode = bestStart;
        result.entryNode = bestTarget;
        result.totalDistance = bestDist;
        result.stepCount = bestStepCount;
        result.path = bestPath;
    } else {
        result.found = false;
        result.startNode = "";
        result.entryNode = "";
        result.totalDistance = 0.0;
        result.stepCount = 0;
        result.path = nullptr;
    }

    return result;
}

