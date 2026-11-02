#include "Graph.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

namespace {
    // Hàm phụ trợ cắt bỏ khoảng trắng và ký tự xuống dòng thừa ở hai đầu chuỗi
    std::string trim(const std::string& str) {
        size_t first = 0;
        while (first < str.size() && (std::isspace(static_cast<unsigned char>(str[first])) || str[first] == '\r')) {
            ++first;
        }
        if (first == str.size()) return "";
        size_t last = str.size() - 1;
        while (last > first && (std::isspace(static_cast<unsigned char>(str[last])) || str[last] == '\r')) {
            --last;
        }
        return str.substr(first, last - first + 1);
    }
}

Graph::Graph(int initialCapacity)
    : vertices(nullptr), numVertices(0), capacity(0) {
    if (initialCapacity < 4) initialCapacity = 4;
    capacity = initialCapacity;
    vertices = new VertexNode[capacity];
}

Graph::~Graph() {
    freeMemory();
}

Graph::Graph(const Graph& other)
    : vertices(nullptr), numVertices(0), capacity(0) {
    copyFrom(other);
}

Graph& Graph::operator=(const Graph& other) {
    if (this != &other) {
        freeMemory();
        copyFrom(other);
    }
    return *this;
}

void Graph::freeMemory() {
    if (vertices != nullptr) {
        for (int i = 0; i < numVertices; ++i) {
            EdgeNode* curr = vertices[i].head;
            while (curr != nullptr) {
                EdgeNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
            vertices[i].head = nullptr;
        }
        delete[] vertices;
        vertices = nullptr;
    }
    numVertices = 0;
    capacity = 0;
}

void Graph::copyFrom(const Graph& other) {
    capacity = (other.capacity > 0) ? other.capacity : 32;
    numVertices = other.numVertices;
    vertices = new VertexNode[capacity];

    for (int i = 0; i < numVertices; ++i) {
        vertices[i].name = other.vertices[i].name;
        vertices[i].displayName = other.vertices[i].displayName;
        vertices[i].description = other.vertices[i].description;
        vertices[i].x = other.vertices[i].x;
        vertices[i].y = other.vertices[i].y;
        vertices[i].head = nullptr;

        EdgeNode* srcNode = other.vertices[i].head;
        EdgeNode** tailPtr = &(vertices[i].head);
        while (srcNode != nullptr) {
            *tailPtr = new EdgeNode(srcNode->dest, srcNode->weight, srcNode->roadType, nullptr);
            tailPtr = &((*tailPtr)->next);
            srcNode = srcNode->next;
        }
    }
}

void Graph::resize(int newCapacity) {
    VertexNode* newVertices = new VertexNode[newCapacity];
    for (int i = 0; i < numVertices; ++i) {
        newVertices[i].name = vertices[i].name;
        newVertices[i].displayName = vertices[i].displayName;
        newVertices[i].description = vertices[i].description;
        newVertices[i].x = vertices[i].x;
        newVertices[i].y = vertices[i].y;
        newVertices[i].head = vertices[i].head;
    }
    delete[] vertices;
    vertices = newVertices;
    capacity = newCapacity;
}

int Graph::findVertexIndex(const std::string& name) const {
    for (int i = 0; i < numVertices; ++i) {
        if (vertices[i].name == name) {
            return i;
        }
    }
    return -1;
}

bool Graph::hasVertex(const std::string& name) const {
    return findVertexIndex(name) != -1;
}

void Graph::addVertex(const std::string& name, const std::string& displayName,
                      const std::string& desc, double x, double y) {
    if (name.empty()) {
        return;
    }

    int idx = findVertexIndex(name);
    if (idx != -1) {
        // Đã tồn tại, cập nhật thêm tên hiển thị và mô tả nếu có
        if (!displayName.empty()) vertices[idx].displayName = displayName;
        if (!desc.empty()) vertices[idx].description = desc;
        if (x != 0.0 || y != 0.0) {
            vertices[idx].x = x;
            vertices[idx].y = y;
        }
        return;
    }

    if (numVertices >= capacity) {
        int newCap = (capacity == 0) ? 8 : capacity * 2;
        resize(newCap);
    }

    vertices[numVertices].name = name;
    vertices[numVertices].displayName = displayName.empty() ? name : displayName;
    vertices[numVertices].description = desc;
    vertices[numVertices].x = x;
    vertices[numVertices].y = y;
    vertices[numVertices].head = nullptr;
    ++numVertices;
}

double Graph::getX(const std::string& name) const {
    int idx = findVertexIndex(name);
    return (idx != -1) ? vertices[idx].x : 0.0;
}

double Graph::getY(const std::string& name) const {
    int idx = findVertexIndex(name);
    return (idx != -1) ? vertices[idx].y : 0.0;
}

void Graph::setCoordinates(const std::string& name, double x, double y) {
    int idx = findVertexIndex(name);
    if (idx != -1) {
        vertices[idx].x = x;
        vertices[idx].y = y;
    }
}

void Graph::addDirectedEdgeInternal(const std::string& src, const std::string& dest, double weight, const std::string& roadType) {
    addVertex(src);
    addVertex(dest);

    int u = findVertexIndex(src);
    if (u == -1) return;

    // Kiểm tra xem cạnh nối src -> dest đã tồn tại hay chưa
    EdgeNode* curr = vertices[u].head;
    while (curr != nullptr) {
        if (curr->dest == dest) {
            curr->weight = weight; // Cập nhật trọng số nếu đã tồn tại
            curr->roadType = roadType;
            return;
        }
        curr = curr->next;
    }

    // Thêm cạnh mới vào đầu danh sách kề của đỉnh src
    vertices[u].head = new EdgeNode(dest, weight, roadType, vertices[u].head);
}

void Graph::addEdge(const std::string& src, const std::string& dest, double weight, const std::string& roadType) {
    if (src.empty() || dest.empty() || src == dest) {
        return;
    }
    // Đồ thị vô hướng: Thêm cạnh theo cả hai hướng src <-> dest
    addDirectedEdgeInternal(src, dest, weight, roadType);
    addDirectedEdgeInternal(dest, src, weight, roadType);
}

std::string Graph::getEdgeRoadType(const std::string& src, const std::string& dest) const {
    EdgeNode* curr = getNeighbors(src);
    while (curr != nullptr) {
        if (curr->dest == dest) {
            return curr->roadType;
        }
        curr = curr->next;
    }
    return "branch";
}

EdgeNode* Graph::getNeighbors(const std::string& name) const {
    int idx = findVertexIndex(name);
    if (idx == -1) return nullptr;
    return vertices[idx].head;
}

EdgeNode* Graph::getNeighbors(int vertexIndex) const {
    if (vertexIndex < 0 || vertexIndex >= numVertices) return nullptr;
    return vertices[vertexIndex].head;
}

int Graph::getNumVertices() const {
    return numVertices;
}

std::string Graph::getVertexName(int index) const {
    if (index >= 0 && index < numVertices) {
        return vertices[index].name;
    }
    return "";
}

std::string Graph::getDisplayName(const std::string& name) const {
    int idx = findVertexIndex(name);
    if (idx != -1 && !vertices[idx].displayName.empty()) {
        return vertices[idx].displayName;
    }
    return name;
}

std::string Graph::getDescription(const std::string& name) const {
    int idx = findVertexIndex(name);
    if (idx != -1) {
        return vertices[idx].description;
    }
    return "";
}

double Graph::getEdgeWeight(const std::string& src, const std::string& dest) const {
    EdgeNode* curr = getNeighbors(src);
    while (curr != nullptr) {
        if (curr->dest == dest) {
            return curr->weight;
        }
        curr = curr->next;
    }
    return -1.0;
}

int Graph::getDegree(const std::string& name) const {
    int count = 0;
    EdgeNode* curr = getNeighbors(name);
    while (curr != nullptr) {
        ++count;
        curr = curr->next;
    }
    return count;
}

int Graph::getTotalEdges() const {
    int totalDirected = 0;
    for (int i = 0; i < numVertices; ++i) {
        EdgeNode* curr = vertices[i].head;
        while (curr != nullptr) {
            ++totalDirected;
            curr = curr->next;
        }
    }
    return totalDirected / 2; // Đồ thị vô hướng: số cạnh = tổng số nửa bậc / 2
}

bool Graph::loadNodesFromCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[Graph Warning] Không thể mở file nodes: " << filePath << std::endl;
        return false;
    }

    std::string line;
    int lineNum = 0;
    int loadedNodes = 0;

    while (std::getline(file, line)) {
        ++lineNum;
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == '#') {
            continue;
        }

        size_t c1 = trimmedLine.find(',');
        if (c1 == std::string::npos) continue;

        std::string id = trim(trimmedLine.substr(0, c1));
        if (lineNum == 1 && (id == "id" || id == "ID" || id == "node")) {
            continue; // Bỏ qua tiêu đề
        }

        size_t c2 = trimmedLine.find(',', c1 + 1);
        std::string displayName;
        std::string description;
        double x = 0.0;
        double y = 0.0;

        if (c2 != std::string::npos) {
            displayName = trim(trimmedLine.substr(c1 + 1, c2 - c1 - 1));
            size_t c3 = trimmedLine.find(',', c2 + 1);
            if (c3 != std::string::npos) {
                description = trim(trimmedLine.substr(c2 + 1, c3 - c2 - 1));
                size_t c4 = trimmedLine.find(',', c3 + 1);
                if (c4 != std::string::npos) {
                    try {
                        x = std::stod(trim(trimmedLine.substr(c3 + 1, c4 - c3 - 1)));
                        y = std::stod(trim(trimmedLine.substr(c4 + 1)));
                    } catch (...) {
                        x = 0.0;
                        y = 0.0;
                    }
                }
            } else {
                description = trim(trimmedLine.substr(c2 + 1));
            }
        } else {
            displayName = trim(trimmedLine.substr(c1 + 1));
        }

        addVertex(id, displayName, description, x, y);
        if (id == "A2" && !hasVertex("E1")) {
            addVertex("E1", "Nhà E1", "Văn phòng Bộ môn", x, y);
        } else if (id == "E1" && !hasVertex("A2")) {
            addVertex("A2", "Nhà E1", "Văn phòng Bộ môn", x, y);
        }
        if (id == "P*" && !hasVertex("PZ")) {
            addVertex("PZ", "Nhà để xe", "Khu để xe máy / ô tô", x, y);
        } else if (id == "PZ" && !hasVertex("P*")) {
            addVertex("P*", "Nhà để xe", "Khu để xe máy / ô tô", x, y);
        }
        ++loadedNodes;
    }

    file.close();
    std::cout << "[Graph] Nạp thông tin " << loadedNodes << " địa điểm từ " << filePath << std::endl;
    return true;
}

bool Graph::loadEdgesFromCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[Graph Error] Không thể mở file edges: " << filePath << std::endl;
        return false;
    }

    std::string line;
    int lineNum = 0;
    int loadedEdges = 0;

    while (std::getline(file, line)) {
        ++lineNum;
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty() || trimmedLine[0] == '#') {
            continue;
        }

        std::stringstream ss(trimmedLine);
        std::string src, dest, weightStr, roadType;

        if (std::getline(ss, src, ',') &&
            std::getline(ss, dest, ',') &&
            std::getline(ss, weightStr, ',')) {

            src = trim(src);
            dest = trim(dest);
            weightStr = trim(weightStr);

            if (std::getline(ss, roadType, ',')) {
                roadType = trim(roadType);
            } else {
                roadType = "branch";
            }

            // Bỏ qua dòng tiêu đề nếu có
            if (lineNum == 1 && (src == "src" || src == "source" || src == "from" || src == "u")) {
                continue;
            }

            try {
                double weight = std::stod(weightStr);
                if (weight >= 0) {
                    addEdge(src, dest, weight, roadType);
                    ++loadedEdges;
                }
            } catch (const std::exception& e) {
                if (lineNum > 1) {
                    std::cerr << "[Graph Warning] Lỗi đọc dữ liệu dòng " << lineNum 
                              << ": " << trimmedLine << " (" << e.what() << ")" << std::endl;
                }
            }
        }
    }

    file.close();
    std::cout << "[Graph] Nạp thành công " << loadedEdges << " cạnh từ " 
              << filePath << " (Tổng: " << numVertices << " đỉnh, " 
              << getTotalEdges() << " cạnh vô hướng)." << std::endl;
    return true;
}

bool Graph::loadFromCSV(const std::string& filePath) {
    // Tự động tìm nạp nodes.csv nếu nằm cùng thư mục với edges.csv
    std::string dir = "";
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        dir = filePath.substr(0, lastSlash + 1);
    }
    std::string nodesPath = dir + "nodes.csv";
    std::ifstream testNodes(nodesPath);
    if (testNodes.good()) {
        testNodes.close();
        loadNodesFromCSV(nodesPath);
    }

    return loadEdgesFromCSV(filePath);
}

void Graph::printGraph() const {
    std::cout << "\n================ BẢNG DANH SÁCH KỀ ĐỒ THỊ (UTC2) ================\n";
    std::cout << "Tổng số đỉnh: " << numVertices << " | Tổng số cạnh vô hướng: " << getTotalEdges() << "\n";
    std::cout << "-----------------------------------------------------------------\n";

    for (int i = 0; i < numVertices; ++i) {
        std::cout << "[" << std::setw(2) << i << "] " << std::left << std::setw(5) << vertices[i].name 
                  << " (" << std::left << std::setw(25) << vertices[i].displayName << ") -> ";
        EdgeNode* curr = vertices[i].head;
        if (curr == nullptr) {
            std::cout << "(không có cạnh nối)";
        } else {
            bool first = true;
            while (curr != nullptr) {
                if (!first) std::cout << " -> ";
                std::cout << "(" << curr->dest << ", " << curr->weight << "m)";
                first = false;
                curr = curr->next;
            }
        }
        std::cout << " -> nullptr\n";
    }
    std::cout << "=================================================================\n\n";
}

void Graph::clear() {
    freeMemory();
}
