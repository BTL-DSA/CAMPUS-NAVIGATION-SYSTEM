#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <iostream>

/**
 * @brief Node biểu diễn một cạnh kề trong danh sách liên kết đơn của đồ thị.
 * Tuân thủ quy chuẩn con trỏ thuần, không dùng STL container.
 */
struct EdgeNode {
    std::string dest;        // Mã nút đích (ví dụ: "T", "C1", "E7")
    double weight;           // Trọng số cạnh (khoảng cách thực tế mét)
    std::string roadType;    // Loại đường: "main", "internal", "branch"
    EdgeNode* next;          // Con trỏ tới cạnh kề tiếp theo

    EdgeNode(const std::string& d = "", double w = 0.0, const std::string& rt = "branch", EdgeNode* n = nullptr)
        : dest(d), weight(w), roadType(rt), next(n) {}
};

/**
 * @brief Node biểu diễn một đỉnh của đồ thị và con trỏ đầu danh sách kề.
 */
struct VertexNode {
    std::string name;        // Mã nút định danh (ví dụ: "G", "C1", "E7", "P*")
    std::string displayName; // Tên hiển thị (ví dụ: "Cổng số 451", "Hội trường C1")
    std::string description; // Mô tả chi tiết vị trí
    double x;                // Tọa độ pixel X trên sơ đồ bản đồ
    double y;                // Tọa độ pixel Y trên sơ đồ bản đồ
    EdgeNode* head;          // Con trỏ đầu danh sách liên kết các cạnh kề

    VertexNode() : name(""), displayName(""), description(""), x(0.0), y(0.0), head(nullptr) {}
    VertexNode(const std::string& n, const std::string& dName = "", const std::string& desc = "",
               double px = 0.0, double py = 0.0)
        : name(n), displayName(dName.empty() ? n : dName), description(desc), x(px), y(py), head(nullptr) {}
};

/**
 * @brief Lớp Graph biểu diễn đồ thị vô hướng có trọng số.
 * Sử dụng Danh sách kề (Adjacency List) với mảng động con trỏ thuần.
 * Không sử dụng std::vector hay std::list bên trong cấu trúc lưu trữ đồ thị.
 * Quản lý bộ nhớ nghiêm ngặt theo Rule of Three.
 */
class Graph {
private:
    VertexNode* vertices; // Mảng động lưu trữ các đỉnh
    int numVertices;      // Số đỉnh hiện tại trong đồ thị
    int capacity;         // Sức chứa tối đa hiện tại của mảng vertices

    // Các hàm phụ trợ nội bộ
    void resize(int newCapacity);
    void freeMemory();
    void copyFrom(const Graph& other);
    void addDirectedEdgeInternal(const std::string& src, const std::string& dest, double weight, const std::string& roadType);

public:
    // Khởi tạo đồ thị với sức chứa ban đầu
    explicit Graph(int initialCapacity = 32);

    // Destructor: Thu hồi toàn bộ danh sách liên kết và mảng đỉnh
    ~Graph();

    // Copy Constructor & Assignment Operator (Rule of Three)
    Graph(const Graph& other);
    Graph& operator=(const Graph& other);

    // Tìm chỉ số đỉnh theo mã nút (name). Trả về -1 nếu không tồn tại
    int findVertexIndex(const std::string& name) const;

    // Kiểm tra đỉnh có tồn tại trong đồ thị không
    bool hasVertex(const std::string& name) const;

    // Thêm hoặc cập nhật một đỉnh mới vào đồ thị (hỗ trợ kèm tọa độ x, y)
    void addVertex(const std::string& name, const std::string& displayName = "",
                   const std::string& desc = "", double x = 0.0, double y = 0.0);

    // Lấy tọa độ pixel X của đỉnh (trả về 0.0 nếu không tồn tại)
    double getX(const std::string& name) const;

    // Lấy tọa độ pixel Y của đỉnh (trả về 0.0 nếu không tồn tại)
    double getY(const std::string& name) const;

    // Cập nhật tọa độ pixel cho đỉnh
    void setCoordinates(const std::string& name, double x, double y);

    // Thêm cạnh vô hướng hai chiều giữa src và dest với trọng số weight và loại đường roadType
    void addEdge(const std::string& src, const std::string& dest, double weight, const std::string& roadType = "branch");

    // Lấy loại đường của cạnh ("main", "internal", "branch")
    std::string getEdgeRoadType(const std::string& src, const std::string& dest) const;

    // Lấy con trỏ đầu danh sách kề của đỉnh name (trả về nullptr nếu không tồn tại)
    EdgeNode* getNeighbors(const std::string& name) const;

    // Lấy con trỏ đầu danh sách kề theo chỉ số đỉnh
    EdgeNode* getNeighbors(int vertexIndex) const;

    // Lấy số lượng đỉnh hiện có
    int getNumVertices() const;

    // Lấy mã đỉnh tại chỉ số index
    std::string getVertexName(int index) const;

    // Lấy tên hiển thị (displayName) của đỉnh
    std::string getDisplayName(const std::string& name) const;

    // Lấy mô tả chi tiết của đỉnh
    std::string getDescription(const std::string& name) const;

    // Lấy trọng số cạnh giữa hai đỉnh kề nhau. Trả về -1.0 nếu không có cạnh nối
    double getEdgeWeight(const std::string& src, const std::string& dest) const;

    // Lấy bậc của một đỉnh (số cạnh nối trực tiếp)
    int getDegree(const std::string& name) const;

    // Lấy tổng số cạnh vô hướng trong đồ thị
    int getTotalEdges() const;

    // Nạp danh sách các nút từ file nodes.csv (id,name,description)
    bool loadNodesFromCSV(const std::string& filePath);

    // Nạp các cạnh từ file edges.csv (source,target,weight)
    bool loadEdgesFromCSV(const std::string& filePath);

    // Nạp đồ thị tự động từ file CSV (hỗ trợ cả nạp kèm nodes.csv nếu cùng thư mục)
    bool loadFromCSV(const std::string& filePath);

    // In toàn bộ danh sách kề ra màn hình console
    void printGraph() const;

    // Xóa sạch dữ liệu đồ thị và giải phóng toàn bộ bộ nhớ
    void clear();
};

#endif // GRAPH_H
