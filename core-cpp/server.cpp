#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0A00
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif

#include "httplib.h"
#include "json.hpp"
#include "Graph.h"
#include "Dijkstra.h"
#include "Navigator.h"
#include "ScheduleOptimizer.h"
#include "AStar.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>

using json = nlohmann::json;

// --- Helper Utilities ---

static double round1(double val) {
    return std::round(val * 10.0) / 10.0;
}

static double round2(double val) {
    return std::round(val * 100.0) / 100.0;
}

static std::string trimString(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static BlockedEdgeList parseBlockedEdges(const std::string& blockedStr) {
    BlockedEdgeList list;
    if (blockedStr.empty()) return list;

    std::stringstream ss(blockedStr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token = trimString(token);
        if (token.empty()) continue;
        size_t dash = token.find('-');
        if (dash != std::string::npos) {
            std::string u = trimString(token.substr(0, dash));
            std::string v = trimString(token.substr(dash + 1));
            if (!u.empty() && !v.empty()) {
                list.block(u, v);
            }
        }
    }
    return list;
}

static std::string findExistingPath(const std::vector<std::string>& candidates) {
    for (const auto& path : candidates) {
        std::ifstream f(path);
        if (f.good()) return path;
    }
    return candidates.empty() ? "" : candidates.front();
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::cout << "======================================================================\n";
    std::cout << "   UTC2 CAMPUS NAVIGATION SYSTEM - C++ STANDALONE HTTP SERVER\n";
    std::cout << "   Phan hieu Truong Dai hoc Giao thong Van tai tai TP. Ho Chi Minh\n";
    std::cout << "======================================================================\n\n";

    // 1. Resolve data paths
    std::string nodesCsv = findExistingPath({
        "core-cpp/data/nodes.csv",
        "data/nodes.csv",
        "./nodes.csv",
        "../core-cpp/data/nodes.csv"
    });
    std::string edgesCsv = findExistingPath({
        "core-cpp/data/edges.csv",
        "data/edges.csv",
        "./edges.csv",
        "../core-cpp/data/edges.csv"
    });
    std::string nodesJsonPath = findExistingPath({
        "nodes.json",
        "./nodes.json",
        "../nodes.json"
    });
    std::string webUiDir = findExistingPath({
        "web-ui/index.html",
        "./web-ui/index.html",
        "../web-ui/index.html"
    });
    if (!webUiDir.empty()) {
        size_t idx = webUiDir.find_last_of("/\\");
        webUiDir = (idx != std::string::npos) ? webUiDir.substr(0, idx) : "./web-ui";
    } else {
        webUiDir = "./web-ui";
    }

    // 2. Load Graph
    Graph graph;
    std::cout << "[Init] Dang nap danh sach dinh tu: " << nodesCsv << " ...\n";
    bool okNodes = graph.loadNodesFromCSV(nodesCsv);
    std::cout << "[Init] Dang nap danh sach canh tu: " << edgesCsv << " ...\n";
    bool okEdges = graph.loadEdgesFromCSV(edgesCsv);

    if (!okNodes || !okEdges) {
        std::cerr << "[Warning] Co the mot so file du lieu CSV chua duoc nap day du!\n";
    }
    std::cout << "[OK] Do thi da san sang: " << graph.getNumVertices() << " dinh, "
              << graph.getTotalEdges() << " canh vo huong.\n";

    // 3. Cache nodes JSON
    json nodesArray = json::array();
    std::ifstream jFile(nodesJsonPath);
    if (jFile.good()) {
        try {
            nodesArray = json::parse(jFile);
            std::cout << "[Init] Da nap " << nodesArray.size() << " dia diem tu " << nodesJsonPath << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[Warning] Loi doc nodes.json: " << e.what() << "\n";
        }
    }
    if (nodesArray.empty()) {
        std::cout << "[Init] Tao danh sach nodes JSON tu bo nho Graph...\n";
        for (int i = 0; i < graph.getNumVertices(); ++i) {
            std::string nid = graph.getVertexName(i);
            nodesArray.push_back({
                {"id", nid},
                {"name", graph.getDisplayName(nid)},
                {"description", graph.getDescription(nid)},
                {"x", graph.getX(nid)},
                {"y", graph.getY(nid)}
            });
        }
    }

    // 4. Initialize HTTP Server
    httplib::Server svr;

    // CORS & Common Headers post-routing
    svr.set_post_routing_handler([](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
    });

    // OPTIONS preflight
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");
        res.status = 204;
    });

    // Mount Static Web UI
    std::cout << "[Mount] Phuc vu giao dien Frontend tinh tu thu muc: " << webUiDir << "\n";
    svr.set_mount_point("/", webUiDir);

    // Fallback explicitly serving index.html at root "/"
    svr.Get("/", [&webUiDir](const httplib::Request&, httplib::Response& res) {
        std::string indexPath = webUiDir + "/index.html";
        std::ifstream file(indexPath, std::ios::binary);
        if (file.good()) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            res.set_content(content, "text/html; charset=utf-8");
            res.status = 200;
        } else {
            res.status = 404;
            res.set_content("<h1>404 Not Found: web-ui/index.html</h1>", "text/html; charset=utf-8");
        }
    });

    // ─────────────────────────────────────────────────────────────────────────
    // Route 1: GET /api/health
    // ─────────────────────────────────────────────────────────────────────────
    svr.Get("/api/health", [&graph](const httplib::Request&, httplib::Response& res) {
        json j = {
            {"status", "ok"},
            {"vertices", graph.getNumVertices()},
            {"edges", graph.getTotalEdges()},
            {"message", "UTC2 Navigation Server (C++ Core) dang hoat dong binh thuong."},
            {"endpoints", {
                "GET /api/route",
                "POST /api/multi-stop",
                "POST /api/schedule-check",
                "GET /api/benchmark",
                "GET /api/nodes",
                "GET /api/health"
            }}
        };
        res.set_content(j.dump(2), "application/json; charset=utf-8");
        res.status = 200;
    });

    // ─────────────────────────────────────────────────────────────────────────
    // Route 2: GET /api/nodes
    // ─────────────────────────────────────────────────────────────────────────
    svr.Get("/api/nodes", [&nodesArray](const httplib::Request&, httplib::Response& res) {
        json j = {
            {"count", nodesArray.size()},
            {"nodes", nodesArray}
        };
        res.set_content(j.dump(2), "application/json; charset=utf-8");
        res.status = 200;
    });

    // ─────────────────────────────────────────────────────────────────────────
    // Route 3: GET /api/route
    // ─────────────────────────────────────────────────────────────────────────
    svr.Get("/api/route", [&graph](const httplib::Request& req, httplib::Response& res) {
        std::string start = req.has_param("start") ? trimString(req.get_param_value("start")) : "G";
        if (start.empty()) start = "G";

        std::string end = req.has_param("end") ? trimString(req.get_param_value("end")) : "";
        if (end.empty()) {
            json err = {{"error", "Thieu tham so bat buoc: 'end'"}};
            res.status = 400;
            res.set_content(err.dump(2), "application/json; charset=utf-8");
            return;
        }

        std::string blockedStr = req.has_param("blocked") ? req.get_param_value("blocked") : "";
        BlockedEdgeList blockedList = parseBlockedEdges(blockedStr);

        DijkstraResult dijRes = Dijkstra::run(graph, start, end, blockedList, false);

        if (!dijRes.found) {
            json err = {
                {"error", "Khong tim thay duong di tu '" + start + "' den '" + end + "'."},
                {"path", json::array()},
                {"total_distance", nullptr}
            };
            res.status = 404;
            res.set_content(err.dump(2), "application/json; charset=utf-8");
            return;
        }

        std::vector<std::string> pathNodes;
        PathStep* step = dijRes.path;
        while (step != nullptr) {
            pathNodes.push_back(step->nodeId);
            step = step->next;
        }

        std::vector<TurnStep> turns = Navigator::generateDirections(graph, pathNodes);
        json detailedDirs = json::array();
        json simpleDirs = json::array();
        for (const auto& ts : turns) {
            detailedDirs.push_back({
                {"step_index", ts.stepIndex},
                {"from_node", ts.fromNode},
                {"to_node", ts.toNode},
                {"from_name", ts.fromName},
                {"to_name", ts.toName},
                {"distance", round1(ts.distance)},
                {"action", ts.actionText},
                {"angle_deg", round1(ts.angleDeg)},
                {"instruction", ts.instruction}
            });
            simpleDirs.push_back(ts.instruction);
        }

        std::string startResolved = dijRes.startNode.empty() ? start : dijRes.startNode;
        std::string entryResolved = dijRes.entryNode.empty() ? end : dijRes.entryNode;

        json j = {
            {"start", startResolved},
            {"start_node", startResolved},
            {"end", end},
            {"entry_node", entryResolved},
            {"total_distance", round1(dijRes.totalDistance)},
            {"walking_time_sec", static_cast<int>(std::round(dijRes.totalDistance / 1.2))},
            {"path", pathNodes},
            {"directions", simpleDirs},
            {"turn_by_turn", detailedDirs}
        };

        res.set_content(j.dump(2), "application/json; charset=utf-8");
        res.status = 200;
    });

    // ─────────────────────────────────────────────────────────────────────────
    // Route 4: POST /api/multi-stop (and GET fallback)
    // ─────────────────────────────────────────────────────────────────────────
    auto handleMultiStop = [&graph](const httplib::Request& req, httplib::Response& res) {
        std::vector<std::string> stops;
        std::string blockedStr = "";

        if (!req.body.empty()) {
            try {
                json body = json::parse(req.body);
                if (body.contains("stops") && body["stops"].is_array()) {
                    for (const auto& s : body["stops"]) {
                        std::string stopStr = trimString(s.get<std::string>());
                        if (!stopStr.empty()) stops.push_back(stopStr);
                    }
                }
                if (body.contains("blocked") && body["blocked"].is_string()) {
                    blockedStr = body["blocked"].get<std::string>();
                }
            } catch (...) {}
        }

        // Query params fallback
        if (stops.empty() && req.has_param("stops")) {
            std::stringstream ss(req.get_param_value("stops"));
            std::string tok;
            while (std::getline(ss, tok, ',')) {
                tok = trimString(tok);
                if (!tok.empty()) stops.push_back(tok);
            }
        }
        if (blockedStr.empty() && req.has_param("blocked")) {
            blockedStr = req.get_param_value("blocked");
        }

        if (stops.size() < 2) {
            json err = {{"error", "Can cung cap danh sach 'stops' co it nhat 2 diem dung (vi du: ['G', 'CT', 'E3'])."}};
            res.status = 400;
            res.set_content(err.dump(2), "application/json; charset=utf-8");
            return;
        }

        BlockedEdgeList blockedList = parseBlockedEdges(blockedStr);
        MultiStopResult multiRes = Navigator::findMultiStopPath(graph, stops, false, blockedList);

        if (!multiRes.success) {
            json err = {{"error", "Khong tim thay lo trinh noi qua tat ca cac diem ghe."}};
            res.status = 404;
            res.set_content(err.dump(2), "application/json; charset=utf-8");
            return;
        }

        json segArray = json::array();
        for (const auto& seg : multiRes.segments) {
            std::vector<std::string> segPath;
            PathStep* st = seg.path;
            while (st != nullptr) {
                segPath.push_back(st->nodeId);
                st = st->next;
            }
            std::string fromId = segPath.empty() ? "" : segPath.front();
            std::string toId = segPath.empty() ? "" : segPath.back();
            segArray.push_back({
                {"from", fromId},
                {"to", toId},
                {"from_name", graph.getDisplayName(fromId)},
                {"to_name", graph.getDisplayName(toId)},
                {"distance", round1(seg.totalDistance)},
                {"path", segPath}
            });
        }

        std::vector<TurnStep> turns = Navigator::generateDirections(graph, multiRes.fullPath);
        json detailedDirs = json::array();
        json simpleDirs = json::array();
        for (const auto& ts : turns) {
            detailedDirs.push_back({
                {"step_index", ts.stepIndex},
                {"from_node", ts.fromNode},
                {"to_node", ts.toNode},
                {"from_name", ts.fromName},
                {"to_name", ts.toName},
                {"distance", round1(ts.distance)},
                {"action", ts.actionText},
                {"angle_deg", round1(ts.angleDeg)},
                {"instruction", ts.instruction}
            });
            simpleDirs.push_back(ts.instruction);
        }

        json orderedNames = json::array();
        for (const auto& s : multiRes.orderedStops) {
            orderedNames.push_back(graph.getDisplayName(s));
        }

        json j = {
            {"success", true},
            {"original_stops", multiRes.originalStops},
            {"ordered_stops", multiRes.orderedStops},
            {"ordered_stop_names", orderedNames},
            {"total_distance", round1(multiRes.totalDistance)},
            {"walking_time_sec", multiRes.walkingTimeSec},
            {"algorithm_used", multiRes.algorithmUsed},
            {"full_path", multiRes.fullPath},
            {"segments", segArray},
            {"directions", simpleDirs},
            {"turn_by_turn", detailedDirs}
        };

        res.set_content(j.dump(2), "application/json; charset=utf-8");
        res.status = 200;
    };

    svr.Post("/api/multi-stop", handleMultiStop);
    svr.Get("/api/multi-stop", handleMultiStop);

    // ─────────────────────────────────────────────────────────────────────────
    // Route 5: POST /api/schedule-check (and GET fallback)
    // ─────────────────────────────────────────────────────────────────────────
    auto handleScheduleCheck = [&graph](const httplib::Request& req, httplib::Response& res) {
        std::string room1 = "";
        std::string room2 = "";
        int breakTime = 300;
        std::string blockedStr = "";

        if (!req.body.empty()) {
            try {
                json body = json::parse(req.body);
                if (body.contains("room1")) room1 = trimString(body["room1"].get<std::string>());
                if (body.contains("room2")) room2 = trimString(body["room2"].get<std::string>());
                if (body.contains("break_time")) {
                    if (body["break_time"].is_number()) breakTime = body["break_time"].get<int>();
                    else if (body["break_time"].is_string()) breakTime = std::stoi(body["break_time"].get<std::string>());
                }
                if (body.contains("blocked") && body["blocked"].is_string()) {
                    blockedStr = body["blocked"].get<std::string>();
                }
            } catch (...) {}
        }

        if (room1.empty() && req.has_param("room1")) room1 = trimString(req.get_param_value("room1"));
        if (room2.empty() && req.has_param("room2")) room2 = trimString(req.get_param_value("room2"));
        if (req.has_param("break_time")) {
            try { breakTime = std::stoi(req.get_param_value("break_time")); } catch (...) {}
        }
        if (blockedStr.empty() && req.has_param("blocked")) blockedStr = req.get_param_value("blocked");

        if (room1.empty() || room2.empty()) {
            json err = {{"error", "Can cung cap phong hoc tiet truoc 'room1' va phong tiet sau 'room2'."}};
            res.status = 400;
            res.set_content(err.dump(2), "application/json; charset=utf-8");
            return;
        }

        BlockedEdgeList blockedList = parseBlockedEdges(blockedStr);
        TransitionResult trans = ScheduleOptimizer::checkTransition(graph, room1, room2, breakTime, blockedList);

        std::vector<TurnStep> turns = Navigator::generateDirections(graph, trans.path);
        json detailedDirs = json::array();
        json simpleDirs = json::array();
        for (const auto& ts : turns) {
            detailedDirs.push_back({
                {"step_index", ts.stepIndex},
                {"from_node", ts.fromNode},
                {"to_node", ts.toNode},
                {"from_name", ts.fromName},
                {"to_name", ts.toName},
                {"distance", round1(ts.distance)},
                {"action", ts.actionText},
                {"angle_deg", round1(ts.angleDeg)},
                {"instruction", ts.instruction}
            });
            simpleDirs.push_back(ts.instruction);
        }

        std::string badgeClass = trans.isAtRisk ? "danger" : "success";
        json j = {
            {"success", true},
            {"from_room", trans.fromRoom},
            {"to_room", trans.toRoom},
            {"from_node", trans.fromNode},
            {"to_node", trans.toNode},
            {"from_name", trans.fromName},
            {"to_name", trans.toName},
            {"distance", round1(trans.distanceMeters)},
            {"walking_time_sec", trans.walkingTimeSec},
            {"break_time_sec", trans.availableBreakSec},
            {"is_at_risk", trans.isAtRisk},
            {"status", trans.status},
            {"badge_class", badgeClass},
            {"message", trans.warningMessage},
            {"path", trans.path},
            {"directions", simpleDirs},
            {"turn_by_turn", detailedDirs}
        };

        res.set_content(j.dump(2), "application/json; charset=utf-8");
        res.status = 200;
    };

    svr.Post("/api/schedule-check", handleScheduleCheck);
    svr.Get("/api/schedule-check", handleScheduleCheck);

    // ─────────────────────────────────────────────────────────────────────────
    // Route 6: GET /api/benchmark
    // ─────────────────────────────────────────────────────────────────────────
    svr.Get("/api/benchmark", [&graph](const httplib::Request& req, httplib::Response& res) {
        std::string start = req.has_param("start") ? trimString(req.get_param_value("start")) : "G";
        if (start.empty()) start = "G";
        std::string end = req.has_param("end") ? trimString(req.get_param_value("end")) : "E3";
        if (end.empty()) end = "E3";

        std::string blockedStr = req.has_param("blocked") ? req.get_param_value("blocked") : "";
        BlockedEdgeList blockedList = parseBlockedEdges(blockedStr);

        BenchmarkResult bRes = AStar::benchmark(graph, start, end, blockedList);

        json metrics = json::array();
        metrics.push_back({
            {"name", "Dijkstra"},
            {"description", "Tim duong ngan nhat chuan xac tren do thi co trong so"},
            {"distance", bRes.dijkstra.found ? json(round1(bRes.dijkstra.totalDistance)) : json(nullptr)},
            {"visited_nodes", bRes.dijkstra.visitedCount},
            {"time_us", round2(bRes.dijkstra.executionTimeUs)},
            {"path_length", bRes.dijkstra.path.size()}
        });
        metrics.push_back({
            {"name", "A* (Euclidean Heuristic)"},
            {"description", "Dinh huong bang Heuristic khoang cach Euclid h(u)"},
            {"distance", bRes.astar.found ? json(round1(bRes.astar.totalDistance)) : json(nullptr)},
            {"visited_nodes", bRes.astar.visitedCount},
            {"time_us", round2(bRes.astar.executionTimeUs)},
            {"path_length", bRes.astar.path.size()}
        });
        metrics.push_back({
            {"name", "BFS (Breadth-First Search)"},
            {"description", "Duyet theo so canh (khong xet khoang cach met thuc te)"},
            {"distance", bRes.bfs.found ? json(round1(bRes.bfs.totalDistance)) : json(nullptr)},
            {"visited_nodes", bRes.bfs.visitedCount},
            {"time_us", round2(bRes.bfs.executionTimeUs)},
            {"path_length", bRes.bfs.path.size()}
        });

        std::ostringstream ssConc;
        ssConc << "A* su dung ham Heuristic giup giam so dinh duyet tu "
               << bRes.dijkstra.visitedCount << " dinh (Dijkstra) xuong con "
               << bRes.astar.visitedCount << " dinh ma van dam bao khoang cach toi uu "
               << round1(bRes.astar.totalDistance) << "m!";

        json j = {
            {"start", start},
            {"dest", end},
            {"target_node", bRes.destNode},
            {"metrics", metrics},
            {"conclusion", ssConc.str()}
        };

        res.set_content(j.dump(2), "application/json; charset=utf-8");
        res.status = 200;
    });

    // 5. Start Server
    int port = 8080;
    const char* envPort = std::getenv("PORT");
    if (envPort) {
        try { port = std::stoi(envPort); } catch (...) {}
    } else if (argc > 1) {
        try { port = std::stoi(argv[1]); } catch (...) {}
    }

    std::cout << "\n>>> [Server Ready] Khoi dong C++ Web Server tai: http://localhost:" << port << "\n";
    std::cout << ">>> Nhan Ctrl+C de dung server.\n\n";

    if (!svr.listen("0.0.0.0", port)) {
        std::cerr << "[Error] Khong the mo server tai cong " << port << " (co the cong dang bi chiem dung)!\n";
        return 1;
    }

    return 0;
}
