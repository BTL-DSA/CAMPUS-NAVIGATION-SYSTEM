/**
 * graphEngine.js — Bộ máy giải thuật đồ thị UTC2 thuần JavaScript (Standalone)
 *
 * Tích hợp đầy đủ:
 *   - Dữ liệu bản đồ UTC2 (42 nút, 52 cạnh vô hướng có trọng số)
 *   - Min-Heap tự cài (không dùng thư viện ngoài)
 *   - Dijkstra (Shortest Path, blocked edges, dual-entry)
 *   - A*  (Euclidean Heuristic)
 *   - BFS (đếm số hop)
 *   - Multi-stop Routing (Brute-force ≤5, Nearest Neighbor >5)
 *   - Turn-by-turn (Cross Product xác định hướng rẽ)
 *   - Schedule Optimizer (cảnh báo trễ giờ học)
 *   - Benchmark (so sánh vi giây 3 giải thuật)
 *
 * Bài tập lớn môn Cấu trúc Dữ liệu & Giải thuật — UTC2 Campus Navigation
 */

// ============================================================================
//  DỮ LIỆU BẢN ĐỒ UTC2 (nhúng trực tiếp từ nodes.csv + edges.csv)
// ============================================================================

/** Bảng nút: id → { id, name, description, x, y } */
const UTC2_NODES = {
    // Trục Cổng chính & Đại lộ Thành công (Đường 449 mép Tây) - thẳng đứng X = 200
    'G':   { id:'G',   name:'Cổng số 451',               description:'Điểm xuất phát chính (Đường Lê Văn Việt)', x:200,  y:980 },
    'T':   { id:'T',   name:'Ngã ba sau Cổng 451',        description:'Giao giữa Đại lộ Thành công và đường nhỏ', x:200,  y:840 },
    'W1':  { id:'W1',  name:'Đại lộ Thành công W1',       description:'Giao đường Hội nhập',                    x:200,  y:630 },
    'W2':  { id:'W2',  name:'Đại lộ Thành công W2',       description:'Giao đường Sáng tạo',                    x:200,  y:492 },
    'W3':  { id:'W3',  name:'Đại lộ Thành công W3',       description:'Cạnh Quảng trường 27/4 đối diện E7',     x:200,  y:315 },
    'W4':  { id:'W4',  name:'Đại lộ Thành công W4',       description:'Phía trên Quảng trường 27/4',            x:200,  y:110 },

    // Trục ngang sau lưng C2 & Cụm C (Y = 840), Trục trước C2 & C1 (Y = 745)
    'J':    { id:'J',   name:'Ngã tư dưới CV Thành công',  description:'Giao lối lên V1 và sang cụm C',           x:430,  y:840 },
    'K':    { id:'K',   name:'Nhà C2 (Sảnh sau)',          description:'Phòng học C2 (Cửa Nam)',                  x:635,  y:840 },
    'C2':   { id:'C2',  name:'Nhà C2',                     description:'Phòng học C2 (Sảnh trước C2_Front / Sảnh sau K)', x:635,  y:745 },
    'C1_J': { id:'C1_J',name:'Sảnh Tây Nhà C1',         description:'Khoảng sân giữa Nhà C2 và Nhà C1',       x:780,  y:840 },
    'C1_Front': { id:'C1_Front', name:'Nhà C1 (Sảnh Tây)', description:'Sảnh đón phía Tây Nhà C1 nối ngã ba V3', x:780, y:840 },
    'C1':   { id:'C1',  name:'Nhà C1',                   description:'Điểm đón phía Đông Nhà C1 (Đường Hội Hộp)', x:890,  y:780 },
    'C1_East': { id:'C1_East', name:'Nhà C1 (Phía Đông)', description:'Điểm đón phía Đông Nhà C1',             x:890,  y:780 },
    'V1':   { id:'V1',  name:'Đường nội bộ V1',            description:'Góc Tây-Bắc Nhà C2',                     x:430,  y:745 },
    'C2_Front': { id:'C2_Front', name:'Nhà C2 (Sảnh trước)', description:'Cửa Bắc Nhà C2 đường nội bộ',       x:635,  y:745 },
    'V5':   { id:'V5',  name:'Ngã ba tim đường V5',        description:'Ngã ba đường nội bộ trước C2 nối lên Đa năng', x:665, y:745 },
    'V3':   { id:'V3',  name:'Đường nội bộ V3',            description:'Đường nội bộ trước C1 và C2',             x:780,  y:745 },
    'V4':   { id:'V4',  name:'Ngã ba trước Nhà C3',        description:'Ngã ba Đường Hội Hộp và đường trước C2',  x:920,  y:745 },
    'V4_S': { id:'V4_S',name:'Nhà C3',                  description:'Điểm đón Nhà C3',                        x:920,  y:840 },
    'C3':   { id:'C3',  name:'Nhà C3',                     description:'Trung tâm TT Thư viện & Phòng khảo thí', x:970,  y:710 },
    'CT':   { id:'CT',  name:'Căn tin sinh viên',          description:'Khu dịch vụ ăn uống',                    x:920,  y:710 },

    // Trục ngang Đại lộ Hội Nhập - thẳng tắp Y = 630
    'H1':  { id:'H1',  name:'Đường Hội nhập H1',          description:'Giao đường nội bộ phía Tây',             x:405,  y:630 },
    'H2':  { id:'H2',  name:'Đường Hội nhập H2',          description:'Ngã tư đường Hội nhập trước Giảng đường Đa năng', x:665, y:630 },
    'DN':  { id:'DN',  name:'Giảng đường Đa năng',        description:'Sảnh chính Giảng đường Đa năng',          x:665,  y:600 },
    'DN_SE':   { id:'DN_SE',   name:'Lối Đông Đa năng (Nam)',  description:'Góc Đông-Nam Giảng đường Đa năng nối Đường Hội nhập', x:820, y:630 },
    'DN_East': { id:'DN_East', name:'Giảng đường Đa năng (Lối Đông)', description:'Lối đi bộ hành lang phía Đông Giảng đường Đa năng', x:820, y:560 },
    'DN_NE':   { id:'DN_NE',   name:'Lối Đông Đa năng (Bắc)',  description:'Góc Đông-Bắc Giảng đường Đa năng nối Đường Sáng tạo', x:820, y:492 },
    'H3':  { id:'H3',  name:'Đường Hội nhập H3',          description:'Giao lối lên trục Đông cụm E',           x:920,  y:630 },
    'HP':  { id:'HP',  name:'Đường Hội nhập HP',          description:'Tim đường Hội nhập trước cửa Nhà để xe', x:1040, y:630 },
    'HE8B':{ id:'HE8B',name:'Đường Hội nhập E8B',         description:'Tim đường Hội nhập trước sảnh E8B',       x:1225, y:630 },
    'P*':  { id:'P*',  name:'Nhà để xe',                  description:'Khu để xe máy / ô tô',                   x:1040, y:560 },
    'PZ':  { id:'PZ',  name:'Nhà để xe',                  description:'Khu để xe máy / ô tô',                   x:1040, y:560 },
    'P_North': { id:'P_North', name:'Lối Bắc Nhà để xe', description:'Lối thông sau Nhà để xe sang Nhà E3', x:1040, y:440 },
    'P_Road':  { id:'P_Road',  name:'Ngã ba đường sau Nhà để xe', description:'Giao lộ đường sau Nhà xe và đường sang E8', x:1040, y:342 },
    'E8B': { id:'E8B', name:'Nhà E8B',                    description:'Phòng thí nghiệm PTC-UTC2',               x:1225, y:590 },

    // Trục ngang Đường Sáng Tạo - thẳng tắp Y = 492
    'S1':  { id:'S1',  name:'Đường Sáng tạo S1',          description:'Phía dưới Nhà E7',                       x:440,  y:492 },
    'S2':  { id:'S2',  name:'Đường Sáng tạo S2',          description:'Giao đường Ánh Đạo',                     x:575,  y:492 },
    'Q':   { id:'Q',   name:'Đường Sáng tạo Q',           description:'Góc quay dưới chân Nhà E3',              x:705,  y:492 },
    'E7':  { id:'E7',  name:'Nhà E7',                     description:'Sảnh hành lang phía Tây Nhà E7 (cạnh Quảng trường 27/4)', x:440,  y:315 },

    // Trục dọc Đường Ánh Đạo - thẳng đứng X = 575
    'A_North': { id:'A_North', name:'Đường Ánh Đạo (Bắc)', description:'Giao lộ Ánh Đạo ngang phía Bắc Nhà E7', x:575, y:175 },
    'A2':  { id:'A2',  name:'Đường Ánh Đạo A2',           description:'Ngã ba tim đường Ánh Đạo đối diện E1',   x:575,  y:315 },
    'E1':  { id:'E1',  name:'Nhà E1',                     description:'Văn phòng Bộ môn',                       x:630,  y:315 },
    'A1':  { id:'A1',  name:'Đường Ánh Đạo A1',           description:'Bùng binh phía Bắc đường Ánh Đạo',       x:575,  y:110 },
    'E6':  { id:'E6',  name:'Nhà E6',                     description:'Phòng học E6 (Giữa tòa nhà)',             x:755,  y:85  },
    'E6_W':{ id:'E6_W',name:'Nhà E6',                     description:'Phòng học E6 (Giữa tòa nhà)',             x:755,  y:85  },
    'E6_Front':{ id:'E6_Front', name:'Lối vào Nhà E6',    description:'Lối vào sảnh Nhà E6',                    x:755,  y:85  },

    // Trục ngang Công viên Nghĩa Tình Y = 110 & Lối lên sảnh E10
    'XE':  { id:'XE',  name:'Đường dọc Công viên',         description:'Lối qua Công viên Nghĩa tình sang E10',  x:410,  y:110 },
    'N1':  { id:'N1',  name:'Ngã ba N1',                  description:'Lối rẽ từ sảnh Tây E7 sang Đường Ánh Đạo', x:440, y:165 },
    'E10': { id:'E10', name:'Nhà E10',                    description:'Phòng học E10 (Cầu lát đá trước sảnh)', x:410,  y:85  },
    'A0':  { id:'A0',  name:'Ngã tư Ánh Đạo A0',          description:'Ngã tư đường Ánh Đạo và đường ngang mới E10-D0', x:575, y:85 },
    'Z6':  { id:'Z6',  name:'Hành lang Z6',               description:'Cửa đón đầu phía Bắc Nhà E1',            x:705,  y:85  },
    'E9_Front':{ id:'E9_Front', name:'Lối vào Nhà E9',    description:'Lối vào sảnh Nhà E9',                    x:920,  y:85  },
    'E9':  { id:'E9',  name:'Nhà E9',                     description:'Phòng học E9 (Giữa tòa nhà)',             x:920,  y:85  },

    // Trục hành lang nội bộ Nhà E1 (X = 630)
    'E1_North': { id:'E1_North', name:'Hành lang E1 (Bắc)', description:'Hành lang trong lòng Nhà E1 ngang Nhà E5', x:630, y:190 },
    'E1_Mid':   { id:'E1_Mid',   name:'Nhà E1',               description:'Sảnh chính Nhà E1 ngang Nhà E4',           x:630, y:315 },
    'E1_South': { id:'E1_South', name:'Hành lang E1 (Nam)', description:'Hành lang trong lòng Nhà E1 ngang Nhà E3', x:630, y:440 },

    // Cụm giảng đường E3, E4, E5 (Lối Tây qua trục Z X = 705 & Cầu nối vào E1)
    'Z3':  { id:'Z3',  name:'Hành lang Z3',               description:'Lối vào cửa Tây Nhà E3',                 x:705,  y:440 },
    'E3w': { id:'E3w', name:'Nhà E3 (Tây)',               description:'Phòng học E3 (Lối Tây)',                 x:740,  y:440 },
    'E3':  { id:'E3',  name:'Nhà E3',                     description:'Phòng học E3',                           x:740,  y:440 },
    'Z4':  { id:'Z4',  name:'Hành lang Z4',               description:'Lối vào cửa Tây Nhà E4',                 x:705,  y:315 },
    'E4w': { id:'E4w', name:'Nhà E4 (Tây)',               description:'Phòng học E4 (Lối Tây)',                 x:740,  y:315 },
    'E4':  { id:'E4',  name:'Nhà E4',                     description:'Phòng học E4',                           x:740,  y:315 },
    'Z5':  { id:'Z5',  name:'Hành lang Z5',               description:'Lối vào cửa Tây Nhà E5',                 x:705,  y:190 },
    'E5w': { id:'E5w', name:'Nhà E5 (Tây)',               description:'Phòng học E5 (Lối Tây)',                 x:740,  y:190 },
    'E5':  { id:'E5',  name:'Nhà E5',                     description:'Phòng học E5',                           x:740,  y:190 },

    // Trục dọc Đường phía Đông - X = 920 & Nhánh vào E8 Y = 342
    'R2':  { id:'R2',  name:'Đường sau Nhà xe R2',        description:'Lối vào trục nội bộ phía Đông',          x:920,  y:492 },
    'D3':  { id:'D3',  name:'Nhà E3 (Đông)',              description:'Phòng học E3 (Lối Đông)',                x:920,  y:440 },
    'D2':  { id:'D2',  name:'Nhà E4 (Đông)',              description:'Phòng học E4 (Lối Đông)',                x:920,  y:315 },
    'X1':  { id:'X1',  name:'Ngã ba X1',                  description:'Lối rẽ sang Nhà E8',                    x:920,  y:342 },
    'D1':  { id:'D1',  name:'Nhà E5 (Đông)',              description:'Phòng học E5 (Lối Đông)',                x:920,  y:190 },
    'D0':  { id:'D0',  name:'Ngã ba Đường Hạnh Hợp',      description:'Giao lộ phía Bắc Đường Hạnh Hợp',        x:920,  y:85  },
    'E8':  { id:'E8',  name:'Nhà E8',                     description:'Phòng thí nghiệm & Xưởng thực tập',      x:1130, y:342 },
};

/**
 * Trình đọc dữ liệu CSV (Parser) chuẩn 4 cột: source, target, weight, road_type
 * Hỗ trợ nạp chuỗi CSV an toàn, chống tràn dòng hoặc sai lệch chỉ mục
 */
function parseEdgesCSV(csvText) {
    if (!csvText || typeof csvText !== 'string') return [];
    const lines = csvText.split(/\r?\n/);
    const parsed = [];
    for (let i = 0; i < lines.length; i++) {
        const line = lines[i].trim();
        if (!line || line.startsWith('#')) continue;
        const parts = line.split(',').map(s => s.trim());
        if (parts.length < 3) continue;
        const u = parts[0];
        const v = parts[1];
        if (i === 0 && (u.toLowerCase() === 'src' || u.toLowerCase() === 'source' || u.toLowerCase() === 'from' || u.toLowerCase() === 'u')) {
            continue;
        }
        const w = parseFloat(parts[2]);
        if (isNaN(w) || w < 0) continue;
        const roadType = parts[3] ? parts[3].toLowerCase() : 'branch';
        parsed.push([u, v, w, roadType]);
    }
    return parsed;
}

/** Danh sách cạnh gốc (vô hướng) đồng bộ trực tiếp từ core-cpp/data/edges.csv (4 cột chuẩn) */
const UTC2_EDGES_RAW = [
    ['G','T',20,'main'],
    ['T','W1',35,'main'],
    ['T','J',15,'internal'],
    ['J','K',25,'internal'],
    ['K','C1_Front',15,'internal'],
    ['K','C2_Front',20,'internal'],
    ['C1_Front','V3',15,'internal'],
    ['C1','V4',12,'internal'],
    ['C1','V4_S',20,'internal'],
    ['V3','V4',18,'internal'],
    ['V3','C2_Front',15,'internal'],
    ['V4','H3',12,'internal'],
    ['J','V1',20,'internal'],
    ['V1','H1',34,'internal'],
    ['V1','C2_Front',16,'internal'],
    ['C2_Front','V5',18,'internal'],
    ['V5','V3',28,'internal'],
    ['V3','V5',28,'internal'],
    ['V5','H2',25,'internal'],
    ['H2','DN',10,'branch'],
    ['W1','H1',28,'main'],
    ['H1','H2',22,'main'],
    ['H2','DN_SE',15,'main'],
    ['DN_SE','H3',11,'main'],
    ['DN_SE','DN_East',9,'internal'],
    ['DN_East','DN_NE',9,'internal'],
    ['H3','HP',25,'main'],
    ['HP','PZ',9,'branch'],
    ['PZ','P_North',12,'branch'],
    ['P_North','P_Road',16,'branch'],
    ['HP','HE8B',38,'main'],
    ['HE8B','E8B',10,'branch'],
    ['H1','S1',25,'main'],
    ['S1','S2',24,'main'],
    ['S2','Q',13,'internal'],
    ['Q','DN_NE',25,'main'],
    ['DN_NE','R2',25,'main'],
    ['H3','R2',20,'main'],
    ['W1','W2',25,'main'],
    ['W2','S1',44,'main'],
    ['W2','W3',36,'main'],
    ['W3','W4',35,'internal'],
    ['W4','XE',18,'internal'],
    ['XE','A1',26,'main'],
    ['XE','E10',10,'branch'],
    ['N1','XE',12,'internal'],
    ['W3','E7',28,'main'],
    ['S1','E7',30,'internal'],
    ['E7','N1',25,'internal'],
    ['N1','A_North',18,'main'],
    ['S2','A2',21,'main'],
    ['A2','E1',16,'internal'],
    ['A1','A_North',10,'main'],
    ['A_North','A2',14,'main'],
    ['A_North','E1_North',16,'internal'],
    ['A2','E1_Mid',16,'internal'],
    ['Z4','E4w',6,'branch'],
    ['Z4','Z3',25,'internal'],
    ['Z3','E3w',6,'branch'],
    ['Z3','Q',11,'internal'],
    ['Z4','Z5',29,'internal'],
    ['Z5','E5w',6,'branch'],
    ['R2','D3',8,'internal'],
    ['D3','X1',10,'main'],
    ['X1','D2',6,'main'],
    ['D2','D1',16,'main'],
    ['D1','D0',16,'main'],
    ['X1','P_Road',18,'main'],
    ['P_Road','E8',21,'branch'],
    ['E4w','X1',22,'branch'],
    ['E10','A0',40,'internal'],
    ['A0','D0',60,'internal'],
    ['A0','A1',18,'internal'],
    ['A0','E6_W',10,'branch'],
    ['E6_W','D0',50,'internal'],
    ['A0','Z6',20,'internal'],
    ['Z6','Z5',16,'internal'],
    ['Z6','E6',8,'main'],
    
    ['E6','E6_W',0,'internal'],
    ['E6','D0',28,'main'],
    
    
    ['D0','E9',0,'branch'],
    ['V4','V4_S',18,'internal'],
    ['CT','V4',0,'internal'],
    ['E1_North','E1_Mid',22,'internal'],
    ['E1_Mid','E1_South',22,'internal'],
    ['E5','E1_North',15,'internal'],
    ['E5w','E1_North',15,'internal'],
    ['E4w','E1_Mid',15,'internal'],
    ['E3','E1_South',15,'internal'],
    ['E3w','E1_South',15,'internal'],
    ['E1','E1_Mid',0,'internal'],
    ['E3','E3w',0,'internal'],
    ['E4','E4w',0,'internal'],
    ['E5','E5w',0,'internal'],
];

/** Tòa nhà có 2 lối vào / lối ra: logical id → [sảnh_trước, sảnh_sau] 
 * (Nhà C2: C2_Front/K; Nhà E3: D3/E3w; Nhà E4: D2/E4w; Nhà E5: D1/E5w) */
const DUAL_ENTRY = {
    'C2': ['C2_Front', 'K'],
    'NHÀ C2': ['C2_Front', 'K'],
    'E3': ['D3', 'E3w'],
    'NHÀ E3': ['D3', 'E3w'],
    'E4': ['D2', 'E4w'],
    'NHÀ E4': ['D2', 'E4w'],
    'E5': ['D1', 'E5w'],
    'NHÀ E5': ['D1', 'E5w'],
};

/** Alias tên phòng / tòa nhà → mã nút */
const NODE_ALIASES = {
    'C2':'C2', 'NHÀ C2':'C2', 'NHAC2':'C2',
    'C2_FRONT':'C2_Front', 'SẢNH TRƯỚC C2':'C2_Front',
    'C1':'C1', 'NHÀ C1':'C1', 'HỘI TRƯỜNG C1':'C1',
    'C1_EAST':'C1', 'C1_East':'C1', 'SẢNH ĐÔNG C1':'C1',
    'C1_SANH':'C1', 'SẢNH C1':'C1',
    'C1_WEST':'C1_Front', 'C1_FRONT':'C1_Front', 'SẢNH TRƯỚC C1':'C1_Front', 'SẢNH TÂY C1':'C1_Front',
    'C3':'V4_S', 'NHÀ C3':'V4_S', 'THƯ VIỆN':'V4_S', 'V4_S':'V4_S',
    'V4':'V4', 'NGÃ BA C3':'V4', 'NGÃ BA TRƯỚC NHÀ C3':'V4',
    'ĐA NĂNG':'DN', 'ĐN':'DN', 'GIẢNG ĐƯỜNG ĐA NĂNG':'DN', 'DN':'DN', 'H2':'H2',
    'NHÀ XE':'PZ', 'NHÀ ĐỂ XE':'PZ', 'BÃI XE':'PZ', 'P*':'PZ', 'PZ':'PZ',
    'E1':'E1_Mid', 'A2':'A2', 'NHÀ E1':'E1_Mid', 'KHOA CNTT':'E1_Mid', 'E1_MID':'E1_Mid',
    'CĂN TIN':'V4', 'CANTEEN':'V4', 'CT':'V4', 'CĂN TIN SINH VIÊN':'V4',
    'CỔNG':'G', 'CỔNG 451':'G', 'CỔNG CHÍNH':'G',
    'E3':'E3', 'NHÀ E3':'E3', 'E3W':'E3w', 'E3_WEST':'E3w', 'E3_EAST':'D3', 'E3_East':'D3', 'E3_E':'D3',
    'E4':'E4', 'NHÀ E4':'E4', 'E4W':'E4w', 'E4_WEST':'E4w', 'E4_EAST':'D2', 'E4_East':'D2', 'E4_E':'D2',
    'E5':'E5', 'NHÀ E5':'E5', 'E5W':'E5w', 'E5_WEST':'E5w', 'E5_EAST':'D1', 'E5_East':'D1', 'E5_E':'D1',
    'E6':'E6', 'NHÀ E6':'E6', 'E6_W':'E6', 'PHÒNG E6':'E6',
    'A_NORTH':'A_North', 'A_North':'A_North',
    'A0':'A0', 'NGÃ TƯ A0':'A0',
    'Z6':'Z6', 'HÀNH LANG Z6':'Z6',
    'E9':'E9', 'NHÀ E9':'E9', 'D0':'E9', 'PHÒNG E9':'E9',
    'E7':'E7', 'NHÀ E7':'E7', 'NHÀ E8':'E8', 'NHÀ E8B':'E8B', 'NHÀ E10':'E10',
};

/** Tập hợp các nút và định danh thuộc Cụm C1/C2 (Nhà C2, Nhà C1 nằm trên trục sau C2) */
const CLUSTER_C1_C2_NODES = new Set([
    'K', 'C2', 'NHÀ C2', 'NHAC2', 'C2_FRONT', 'C2_Front',
    'C1_J', 'C1', 'NHÀ C1', 'HỘI TRƯỜNG C1', 'C1_SANH', 'C1_WEST', 'C1_FRONT', 'C1_Front'
]);

function isClusterC1C2(id) {
    if (!id) return false;
    const upper = id.toUpperCase().trim();
    if (CLUSTER_C1_C2_NODES.has(upper)) return true;
    const resolved = resolveAlias(id);
    return CLUSTER_C1_C2_NODES.has(resolved);
}

/** Tập hợp các nút và định danh thuộc Cụm C (C2, C1, C3, Căn tin, đường nội bộ C) */
const CLUSTER_C_NODES = new Set([
    'K', 'C2', 'NHÀ C2', 'NHAC2', 'C2_FRONT', 'C2_Front',
    'C1_J', 'C1', 'NHÀ C1', 'HỘI TRƯỜNG C1', 'C1_SANH', 'C1_EAST', 'C1_East', 'C1_FRONT', 'C1_Front',
    'V4', 'V4_S', 'C3', 'NHÀ C3', 'THƯ VIỆN',
    'CT', 'CĂN TIN', 'CANTEEN',
    'V1', 'V3', 'V5', 'C2_Front'
]);

function isClusterC(id) {
    if (!id) return false;
    const upper = id.toUpperCase().trim();
    if (CLUSTER_C_NODES.has(upper)) return true;
    const resolved = resolveAlias(id);
    return CLUSTER_C_NODES.has(resolved);
}

const WALKING_SPEED = 1.2; // m/s

// ============================================================================
//  PHÂN LOẠI TUYẾN ĐƯỜNG & HỆ SỐ ƯU TIÊN (Road Priority Multipliers)
//  - main / major (0.8): Đại lộ Thành Công, Đường Hội Nhập, Trục Sáng Tạo, Công viên
//  - branch / standard (1.0): Lối vào các tòa nhà, đường nhánh
//  - internal / minor (1.4): Đường nội bộ nhỏ hẹp, bọc sau xưởng, sau C2
// ============================================================================
const ROAD_MULTIPLIERS = {
    'main': 0.8,
    'major': 0.8,
    'branch': 1.0,
    'standard': 1.0,
    'internal': 1.4,
    'minor': 1.4,
};

const ROAD_TYPES = {};
UTC2_EDGES_RAW.forEach(([u, v, w, type]) => {
    ROAD_TYPES[`${u}-${v}`] = type;
    ROAD_TYPES[`${v}-${u}`] = type;
});

const CLUSTER_C_INTERNAL_EDGES = new Set([
    'T-J', 'J-T',
    'J-K', 'K-J',
    'K-C1_Front', 'C1_Front-K',
    'C1_Front-V3', 'V3-C1_Front',
    'C1-V4', 'V4-C1',
    'C1-V4_S', 'V4_S-C1',
    'V3-C2_Front', 'C2_Front-V3',
    'V3-V4', 'V4-V3',
    'V4-H3', 'H3-V4',
    'H3-R2', 'R2-H3',
    'R2-D3', 'D3-R2',
    'D3-D2', 'D2-D3',
    'D2-D1', 'D1-D2',
    'J-V1', 'V1-J',
    'V1-C2_Front', 'C2_Front-V1',
    'C2_Front-V5', 'V5-C2_Front',
    'V5-V3', 'V3-V5',
    'V1-H1', 'H1-V1',
    'V5-H2', 'H2-V5',
    'V4-V4_S', 'V4_S-V4',
    'CT-V4', 'V4-CT',
    'H2-DN', 'DN-H2'
]);

function getRoadMultiplier(u, v, isClusterRoute = false) {
    const edgeKey = `${u}-${v}`;
    if (isClusterRoute && CLUSTER_C_INTERNAL_EDGES.has(edgeKey)) {
        return 0.9; // Tuyến nội bộ Cụm C được ưu tiên mượt mà khi đích thuộc Cụm C
    }
    const type = ROAD_TYPES[edgeKey] || ROAD_TYPES[`${v}-${u}`] || 'branch';
    return ROAD_MULTIPLIERS[type] || 1.0;
}

// ============================================================================
//  CƠ CHẾ PHẠT CHUYỂN HƯỚNG (Turn Penalty)
//  - Góc lệch <= 25 độ: Xem như đi thẳng, phạt = 0
//  - Góc lệch > 25 độ: Phạt chuyển hướng theo độ lệch góc (góc 90 độ phạt 15m)
// ============================================================================
function calculateTurnAngle(pId, uId, vId) {
    const p = UTC2_NODES[pId], u = UTC2_NODES[uId], v = UTC2_NODES[vId];
    if (!p || !u || !v) return 0;

    const ax = u.x - p.x, ay = u.y - p.y;
    const bx = v.x - u.x, by = v.y - u.y;

    const cross = ax * by - ay * bx;
    const dot = ax * bx + ay * by;

    return Math.abs(Math.atan2(cross, dot) * (180 / Math.PI));
}

function getTurnPenalty(pId, uId, vId) {
    if (!pId) return 0;
    const angle = calculateTurnAngle(pId, uId, vId);
    if (angle <= 25.0) return 0; // Đi thẳng không phạt
    return 15.0 * (angle / 90.0); // Phạt chuyển hướng tỉ lệ theo góc rẽ
}

/** Tập các tòa nhà E phía Tây và Tây-Bắc: E7, E10 (không chứa E1, E6 vì đi qua Đường Sáng Tạo & Ánh Đạo) */
const E_WEST_NORTH_NODES = new Set([
    'E7', 'NHÀ E7',
    'E10', 'NHÀ E10',
    'XE', 'N1'
]);

function isEWestNorth(id) {
    if (!id) return false;
    const upper = id.toUpperCase().trim();
    if (E_WEST_NORTH_NODES.has(upper)) return true;
    const resolved = resolveAlias(id);
    return E_WEST_NORTH_NODES.has(resolved);
}

/** Trục Đông Đường Hạnh Hộp / Hội Nhập: D3, D2, D1, R2 */
const EAST_AXIS_NODES = new Set([
    'D3', 'D2', 'D1', 'R2',
    'E3_EAST', 'E4_EAST', 'E5_EAST', 'E3_E', 'E4_E', 'E5_E'
]);

function isEastAxis(id) {
    if (!id) return false;
    const upper = id.toUpperCase().trim();
    if (EAST_AXIS_NODES.has(upper)) return true;
    const resolved = resolveAlias(id);
    return EAST_AXIS_NODES.has(resolved);
}

/** Tập các tòa nhà E lối Tây qua nút Q: E3w, E4w, E5w */
const E_WEST_ENTRY_NODES = new Set([
    'E3W', 'E3_WEST',
    'E4W', 'E4_WEST',
    'E5W', 'E5_WEST'
]);

function isEWestEntry(id) {
    if (!id) return false;
    const upper = id.toUpperCase().trim();
    if (E_WEST_ENTRY_NODES.has(upper)) return true;
    const resolved = resolveAlias(id);
    return E_WEST_ENTRY_NODES.has(resolved);
}

/** Tập các tòa nhà/điểm đến khu E8 */
const E8_NODES = new Set([
    'E8', 'NHÀ E8', 'E8B', 'NHÀ E8B', 'X1'
]);

function isE8(id) {
    if (!id) return false;
    const upper = id.toUpperCase().trim();
    if (E8_NODES.has(upper)) return true;
    const resolved = resolveAlias(id);
    return E8_NODES.has(resolved);
}

// ============================================================================
//  QUY TẮC CỔNG 451 & PHÂN LUỒNG LỘ TRÌNH THỰC TẾ:
//  1. Đến Cụm C (C2, C1, C3, Căn tin):
//     - BẮT BUỘC ưu tiên đi nhánh nội bộ T -> J (ngang phía dưới/sau Cụm C)
//     - CHẶN cạnh T-W1 để tránh bắt người dùng đi vòng lên đường Hội Nhập rồi mới vòng ngược xuống
//     - Nếu cạnh T-J bị chặn (công trường thi công), cạnh T-W1 sẽ mở lại để đi đường vòng né tránh
//  2. Đến Nhà E1:
//     - Tuyến đường chuẩn: G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> E1
//     - Chặn cạnh A2-A1 để tuyệt đối không vòng qua W3 -> W4 -> Công viên Nghĩa tình -> A1 rồi mới vòng xuống E1
//  3. Đến các tòa nhà E phía Tây và Tây-Bắc (E7, E10, E6):
//     - Bắt buộc ưu tiên trục W1 -> W2 (dọc Đường 449 phía Tây), CHẶN W1-H1 để không đi qua Hội Nhập
//  4. Đến Nhà E3, E4, E5 (Lối Tây qua nút Q):
//     - Bắt buộc qua ... -> H1 -> S1 -> S2 -> Q rồi rẽ lên hành lang Tây:
//       + Vào E3: Q -> Z3 -> E3w
//       + Vào E4: Q -> Z3 -> Z4 -> E4w
//       + Vào E5: Q -> Z3 -> Z4 -> Z5 -> E5w
//     - Chặn cạnh E1-Z4 và chặn H3-R2, Q-R2 để không dẫn vòng ra hông nhà để xe (D3, D2, D1)
//  5. Đến Nhà E8:
//     - Đi thẳng theo trục chính Đường Hội Nhập: ... -> H1 -> H2 -> H3 -> D3 -> D2 -> X1 -> E8
//     - Chặn ngõ hẹp phía sau lưng Nhà E3/E4 (Q-R2) và chặn H1-S1 để không luồn lách
//  6. Chế độ lộ trình đa điểm (Multi-stop route):
//     - Cho phép cạnh T -> J linh hoạt, không bị khóa cứng như chế độ đơn điểm
// ============================================================================
function isEdgeRestricted(u, v, startId, endId, blockedSet, isMultiStop = false) {
    if (isMultiStop) {
        // Chế độ đa điểm: vẫn chặn T-W1 khi chặng đi vào hoặc đi ra Cụm C (C1, C2, C3)
        // để đảm bảo lộ trình Cổng 451 <-> Cụm C luôn đi qua T->J thay vì vòng ra đại lộ phía Tây
        const isTW1_multi = (u === 'T' && v === 'W1') || (u === 'W1' && v === 'T');
        if (isTW1_multi && (isClusterC(endId) || isClusterC(startId))) {
            const tjBlocked = blockedSet && (blockedSet.has('T-J') || blockedSet.has('J-T'));
            if (!tjBlocked) return true; // Chặn T-W1, bắt buộc đi T->J vào Cụm C
        }
        return false; // Mọi cạnh khác đều được phép trong đa điểm
    }

    const isTJ   = (u === 'T' && v === 'J')   || (u === 'J' && v === 'T');
    const isTW1  = (u === 'T' && v === 'W1')  || (u === 'W1' && v === 'T');
    const isW1H1 = (u === 'W1' && v === 'H1') || (u === 'H1' && v === 'W1');
    const isW2S1 = (u === 'W2' && v === 'S1') || (u === 'S1' && v === 'W2');
    const isW2W3 = (u === 'W2' && v === 'W3') || (u === 'W3' && v === 'W2');
    const isS1E7 = (u === 'S1' && v === 'E7') || (u === 'E7' && v === 'S1');
    const isH3R2 = (u === 'H3' && v === 'R2') || (u === 'R2' && v === 'H3');
    const isQR2  = (u === 'Q' && v === 'R2')  || (u === 'R2' && v === 'Q') || (u === 'DN_NE' && v === 'R2') || (u === 'R2' && v === 'DN_NE');
    const isE1Z4 = (u === 'E1' && v === 'Z4') || (u === 'Z4' && v === 'E1');
    const isH1S1 = (u === 'H1' && v === 'S1') || (u === 'S1' && v === 'H1');
    const isA2A1 = (u === 'A2' && v === 'A1') || (u === 'A1' && v === 'A2');
    const isD2D1 = (u === 'D2' && v === 'D1') || (u === 'D1' && v === 'D2');
    const isX1D1 = (u === 'X1' && v === 'D1') || (u === 'D1' && v === 'X1');
    const isZ6Z5 = (u === 'Z6' && v === 'Z5') || (u === 'Z5' && v === 'Z6');
    const isS2Q  = (u === 'S2' && v === 'Q')  || (u === 'Q' && v === 'S2');
    const isA2S2 = (u === 'A2' && v === 'S2') || (u === 'S2' && v === 'A2');
    const isXEE10 = (u === 'XE' && v === 'E10') || (u === 'E10' && v === 'XE');
    const isA0A1 = (u === 'A0' && v === 'A1') || (u === 'A1' && v === 'A0');
    const isH2S2 = (u === 'H2' && v === 'S2') || (u === 'S2' && v === 'H2');
    const isW3E7 = (u === 'W3' && v === 'E7') || (u === 'E7' && v === 'W3');
    const isE7A2 = (u === 'E7' && v === 'A2') || (u === 'A2' && v === 'E7');
    const isS1S2 = (u === 'S1' && v === 'S2') || (u === 'S2' && v === 'S1');
    const isV4H3 = (u === 'V4' && v === 'H3') || (u === 'H3' && v === 'V4');
    const isH2H3 = (u === 'H2' && v === 'H3') || (u === 'H3' && v === 'H2') || (u === 'H2' && v === 'DN_SE') || (u === 'DN_SE' && v === 'H2');

    const destResolved = resolveAlias(endId);
    const srcResolved = resolveAlias(startId);

    const destIsClusterC = isClusterC(endId);
    const srcIsClusterC  = isClusterC(startId);

    // 0. Kịch bản C2 lên Giảng đường Đa Năng (DN): Cấm đi vòng chữ U qua V4 -> H3
    const srcIsC2 = ['C2', 'K', 'C2_Front', 'C1_Front', 'C1_J'].includes(startId) || ['C2', 'K', 'C2_Front', 'C1_Front', 'C1_J'].includes(srcResolved);
    const destIsC2 = ['C2', 'K', 'C2_Front', 'C1_Front', 'C1_J'].includes(endId) || ['C2', 'K', 'C2_Front', 'C1_Front', 'C1_J'].includes(destResolved);
    const destIsDN = endId === 'DN' || endId === 'GIẢNG ĐƯỜNG ĐA NĂNG' || endId === 'H2' || destResolved === 'DN' || destResolved === 'H2';
    const srcIsDN = startId === 'DN' || startId === 'GIẢNG ĐƯỜNG ĐA NĂNG' || startId === 'H2' || srcResolved === 'DN' || srcResolved === 'H2';

    if ((srcIsC2 && destIsDN) || (srcIsDN && destIsC2)) {
        if (isV4H3 || isH2H3) {
            return true;
        }
    }

    // 0b. Kịch bản Nhà E1: Luôn vào qua sảnh chính Đường Ánh Đạo (A2 -> E1_Mid), không chui xuyên qua E4/E3/E5
    const destIsE1 = ['E1', 'E1_Mid', 'NHÀ E1', 'KHOA CNTT', 'A2'].includes(endId) || ['E1', 'E1_Mid', 'A2'].includes(destResolved);
    const srcIsE1 = ['E1', 'E1_Mid', 'NHÀ E1', 'KHOA CNTT', 'A2'].includes(startId) || ['E1', 'E1_Mid', 'A2'].includes(srcResolved);

    // Kịch bản Giảng đường Đa Năng <-> Nhà E1: Đi theo lối Đông Đa Năng lên Đường Sáng Tạo (như Ảnh 2 đường màu tím)
    const isDNE1Route = srcIsDN && destIsE1;
    const isE1DNRoute = destIsDN && srcIsE1;
    if (isDNE1Route || isE1DNRoute) {
        if ((u === 'E4w' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E4w')) return true;
        if ((u === 'E1_South' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E1_South')) return true;
        if ((u === 'E1_North' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E1_North')) return true;
        const eastIsBlocked = isEdgeBlocked('Q', 'DN_NE', blockedSet) ||
                              isEdgeBlocked('DN_SE', 'DN_East', blockedSet) ||
                              isEdgeBlocked('DN_East', 'DN_NE', blockedSet) ||
                              isEdgeBlocked('H2', 'DN_SE', blockedSet) ||
                              isEdgeBlocked('Q', 'R2', blockedSet);
        if (!eastIsBlocked) {
            if ((u === 'H2' && v === 'H1') || (u === 'H1' && v === 'H2')) return true;
        }
    }

    // Kịch bản C2 <-> Nhà E1: Đi xuống trước Đa Năng rồi men theo lối Đông Đa Năng lên Đường Sáng Tạo & Ánh Đạo
    if ((srcIsC2 && destIsE1) || (srcIsE1 && destIsC2)) {
        if ((u === 'E4w' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E4w')) return true;
        if ((u === 'E1_South' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E1_South')) return true;
        if ((u === 'E1_North' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E1_North')) return true;
        const eastIsBlocked = isEdgeBlocked('Q', 'DN_NE', blockedSet) ||
                              isEdgeBlocked('DN_SE', 'DN_East', blockedSet) ||
                              isEdgeBlocked('DN_East', 'DN_NE', blockedSet) ||
                              isEdgeBlocked('H2', 'DN_SE', blockedSet) ||
                              isEdgeBlocked('Q', 'R2', blockedSet);
        if (!eastIsBlocked) {
            if ((u === 'V1' && v === 'H1') || (u === 'H1' && v === 'V1')) return true;
            if ((u === 'H2' && v === 'H1') || (u === 'H1' && v === 'H2')) return true;
            if ((u === 'V3' && v === 'V4') || (u === 'V4' && v === 'V3')) return true;
            if ((u === 'V4' && v === 'H3') || (u === 'H3' && v === 'V4')) return true;
        }
    }


    // 1. Kịch bản Cụm C (C1, C2, C3, CT...): Tuyến chuẩn qua trục nội bộ C (T -> J)
    if (destIsClusterC || srcIsClusterC) {
        const isToC1 = (destResolved === 'C1' || endId === 'C1' || endId === 'NHÀ C1' || endId === 'HỘI TRƯỜNG C1' || endId === 'C1_SANH');
        const isFromC1 = (srcResolved === 'C1' || startId === 'C1' || startId === 'NHÀ C1' || startId === 'HỘI TRƯỜNG C1' || startId === 'C1_SANH');
        if (isToC1 || isFromC1) {
            if ((u === 'J' && v === 'K') || (u === 'K' && v === 'J')) return true;
            if ((u === 'C1_Front' && v === 'V3') || (u === 'V3' && v === 'C1_Front')) return true;
        }
        if (isTW1) {
            const tjBlocked = blockedSet && (blockedSet.has('T-J') || blockedSet.has('J-T'));
            if (!tjBlocked) {
                return true; // Chặn T-W1 để bắt buộc đi tuyến nội bộ T -> J
            }
        }
        return false;
    }

    // 2. Đi đến các tòa ngoài Cụm C (Khu E, PZ...): Ưu tiên trục lớn T -> W1
    if (isTJ) {
        const tw1Blocked = blockedSet && (blockedSet.has('T-W1') || blockedSet.has('W1-T'));
        if (!tw1Blocked) {
            return true;
        }
    }

    // 3. Kịch bản Nhà E1 (Đường Sáng Tạo & Ánh Đạo): Tuyến chuẩn G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> E1
    const endResolved = resolveAlias(endId);
    if (endResolved === 'E1' || endResolved === 'E1_Mid' || endId === 'E1' || endId === 'A2' || endId === 'NHÀ E1' || endId === 'KHOA CNTT' || endId === 'E1_Mid') {
        if (isA2A1) {
            return true; // Tuyệt đối không dẫn từ A1 xuống A2
        }
        if (isW1H1) {
            return true; // Bắt buộc đi dọc Đại lộ Thành Công W1 -> W2 rồi rẽ vào Đường Sáng Tạo S1
        }
    }

    // 4. Kịch bản Nhà E6 (Đường Sáng Tạo & Ánh Đạo): Tuyến chuẩn G -> T -> W1 -> W2 -> S1 -> S2 -> A2 -> A1 -> E6_W
    if (endResolved === 'E6_W' || endId === 'E6' || endId === 'E6_W' || endId === 'NHÀ E6') {
        if (isW1H1) {
            return true; // Bắt buộc đi dọc Đại lộ Thành Công W1 -> W2 rồi rẽ vào Đường Sáng Tạo S1
        }
        if (isW2W3) {
            return true; // Tuyệt đối không chạy vòng lên hết Đường 449 qua Công viên Nghĩa tình
        }
        if (isS1E7) {
            return true; // Bắt buộc đi thẳng S1 -> S2
        }
        if ((u === 'E4w' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E4w')) {
            return true; // Không đi xuyên qua hành lang Nhà E1
        }
    }
    const startResolved_e6 = resolveAlias(startId);
    if (startResolved_e6 === 'E6_W' || startId === 'E6' || startId === 'E6_W' || startId === 'NHÀ E6') {
        if ((u === 'E4w' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E4w')) {
            return true;
        }
    }
    if (endResolved === 'E9' || endId === 'E9' || endId === 'NHÀ E9') {
        if ((u === 'E4w' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E4w')) {
            return true;
        }
    }
    if (startResolved_e6 === 'E9' || startId === 'E9' || startId === 'NHÀ E9') {
        if ((u === 'E4w' && v === 'E1_Mid') || (u === 'E1_Mid' && v === 'E4w')) {
            return true;
        }
    }

    // 5. Kịch bản các tòa nhà E Tây-Bắc khác (E7, E10):
    if (endResolved === 'E10' || endId === 'E10' || endId === 'NHÀ E10') {
        if (isW3E7 || isE7A2 || isS1S2) {
            return true; // Cấm rẽ từ W3 vào E7, cấm từ E7 luồn qua cầu Ánh Đạo A2, và cấm đi lùi S1->S2 khi lên E10
        }
    }
    // Khi từ Nhà E7 sang Nhà E1: đi phía Nam hợp lý hơn (S1 -> S2 -> A2 -> E1)
    if (startId === 'E7' && (endResolved === 'E1' || endId === 'E1' || endId === 'NHÀ E1' || endId === 'KHOA CNTT' || endId === 'E1_Mid' || endResolved === 'E1_Mid')) {
        const isE7N1 = (u === 'E7' && v === 'N1') || (u === 'N1' && v === 'E7');
        if (isE7N1) return true;
    }
    const isE1Related = (endResolved === 'E1' || endId === 'E1' || endId === 'NHÀ E1' || endId === 'KHOA CNTT' || endId === 'E1_Mid' || endResolved === 'E1_Mid');
    const startIsERelated = (startId === 'E3' || startId === 'E3w' || startId === 'E4' || startId === 'E4w' || startId === 'E5' || startId === 'E5w' || startId === 'E3_East' || startId === 'E4_East' || startId === 'E5_East' || startId === 'D3' || startId === 'D2' || startId === 'D1');
    const startIsE1 = (startId === 'E1' || startId === 'NHÀ E1' || startId === 'KHOA CNTT' || startId === 'E1_Mid' || resolveAlias(startId) === 'E1_Mid');
    const destIsERelated = (endId === 'E3' || endId === 'E3w' || endId === 'E4' || endId === 'E4w' || endId === 'E5' || endId === 'E5w');

    if ((isE1Related && startIsERelated) || (startIsE1 && destIsERelated)) {
        if ((u === 'Z4' && v === 'Z5') || (u === 'Z5' && v === 'Z4') ||
            (u === 'Z3' && v === 'Z4') || (u === 'Z4' && v === 'Z3') ||
            (u === 'Q' && v === 'Z3') || (u === 'Z3' && v === 'Q') ||
            (u === 'S2' && v === 'Q') || (u === 'Q' && v === 'S2') ||
            (u === 'A2' && v === 'S2') || (u === 'S2' && v === 'A2') ||
            (u === 'A1' && v === 'A2') || (u === 'A2' && v === 'A1') ||
            (u === 'A0' && v === 'A1') || (u === 'A1' && v === 'A0') ||
            (u === 'Z6' && v === 'A0') || (u === 'A0' && v === 'Z6') ||
            (u === 'Z5' && v === 'Z6') || (u === 'Z6' && v === 'Z5')) {
            return true; // Đi qua hành lang bên trong E1, không ra khoảng sân ngoài trời bên hông hoặc đường xe
        }
    }
    const destIsEWestNorth = isEWestNorth(endId);
    if (destIsEWestNorth && (startId === 'G' || startId === 'T' || startId === 'W1')) {
        if (isW1H1) {
            return true; // Chặn W1-H1 để ép đi W1 -> W2 dọc Đường 449
        }
    }

    // 6. Kịch bản Nhà E3, E4, E5 (Lối Tây qua nút Q và phân luồng cửa vào):
    const destIsEWest = isEWestEntry(endId);
    const destIsEEast = isEastAxis(endId);
    const isEBuilding = destIsEWest || destIsEEast || endId === 'E3' || endId === 'E4' || endId === 'E5';
    if (isEBuilding) {
        if (startId === 'G' || startId === 'T' || startId === 'W1') {
            // Tuyến chuẩn từ Cổng tới Nhà E3, E4, E5 theo đúng sơ đồ màu đỏ:
            // Đi dọc Đường 449 (W1 -> W2) rồi rẽ vào Đường Sáng Tạo (W2 -> S1 -> S2)
            if (isW1H1) return true; // Cấm đi xuống Đường Hội Nhập H1 vòng qua Giảng đường Đa Năng
            if (isW2W3) return true; // Cấm chạy vượt qua nút W2 lên phía Bắc W3
            if (isS1E7) return true; // Cấm rẽ vào Nhà E7
            if (isH3R2 || isQR2) return true; // Chặn đường vòng ra phía Đông hông nhà để xe
            if (isZ6Z5) return true;

            const isDestE4 = (endId === 'E4' || endId === 'E4w' || destResolved === 'E4w' || destResolved === 'E4');
            const isDestE5 = (endId === 'E5' || endId === 'E5w' || destResolved === 'E5w' || destResolved === 'E5');
            const isDestE3 = (endId === 'E3' || endId === 'E3w' || destResolved === 'E3w' || destResolved === 'E3');

            if (isDestE4 || isDestE5) {
                // Vào Nhà E1 qua sảnh Ánh Đạo A2 -> E1_Mid:
                // Nhánh E4: E1_Mid -> E4w
                // Nhánh E5: E1_Mid -> E1_North -> E5w
                if (isS2Q) return true; // Đi vào trục Ánh Đạo A2 -> E1, không đi tiếp ra nút Q
                if ((u === 'A2' && v === 'A_North') || (u === 'A_North' && v === 'A2')) return true; // Bắt buộc đi trong lòng E1
                if ((u === 'Z4' && v === 'Z5') || (u === 'Z5' && v === 'Z4')) return true;
            } else if (isDestE3) {
                // Nhánh E3: Đi thẳng Đường Sáng Tạo S2 -> Q -> Z3 -> E3w
                const isA2E1Mid = (u === 'A2' && (v === 'E1_Mid' || v === 'E1')) || (v === 'A2' && (u === 'E1_Mid' || u === 'E1'));
                if (isA2E1Mid) return true;
            }
        } else if (startId === 'H1') {
            if (destIsEWest) {
                if (isW2S1) return true;
                if (isH2S2) return true;
                const isA2E1Mid = (u === 'A2' && (v === 'E1_Mid' || v === 'E1')) || (v === 'A2' && (u === 'E1_Mid' || u === 'E1'));
                if (isE1Z4 || isA2E1Mid) return true;
                if (isH3R2 || isQR2) return true;
                if (isZ6Z5) return true;
            } else {
                if (isH1S1) return true;
                if (isQR2) return true;
                if (isZ6Z5) return true;
            }
        } else if (isClusterC(startId) || startId === 'H2' || startId === 'DN') {
            // Xuất phát từ Cụm C (C2, C1, V4...) hoặc Đa Năng đi E3/E4/E5:
            // Tuyến vòng ngoài phía Đông qua Nhà để xe: H2 -> H3 -> R2 -> D3...
            if (isQR2) return true;
            if (isZ6Z5) return true;
        } else if (['E10', 'NHÀ E10', 'A0', 'NGÃ TƯ A0'].includes(startId) || ['E10', 'NHÀ E10', 'A0', 'NGÃ TƯ A0'].includes(srcResolved)) {
            // Xuất phát từ phía Bắc (E10, A0...):
            if (isS2Q || isA2S2) return true;
            if (startId === 'E10' || startId === 'NHÀ E10') {
                if (isXEE10 || isA0A1) return true;
                if ((u === 'A0' && v === 'D0') || (u === 'D0' && v === 'A0') ||
                    (u === 'E6_Front' && v === 'D0') || (u === 'D0' && v === 'E6_Front')) return true;
            }
        }
    }

    // 7. Kịch bản Nhà E8 / E8B:
    // Không đi ngõ hẹp Z6-Z5 phía sau Nhà E1 khi vào E5 (ưu tiên trục hành lang chính E1 hoặc Đường Ánh Đạo)
    if (endResolved === 'E5' || endId === 'E5' || endId === 'E5w' || endResolved === 'E5w') {
        if (startId !== 'E10' && startId !== 'NHÀ E10' && startId !== 'A0') {
            if (isZ6Z5) return true;
        }
    }
    if (isE8(endId)) {
        // Không đi xuyên qua Nhà để xe PZ (trừ khi xuất phát từ chính Nhà để xe)
        const isParking = (u === 'PZ' || v === 'PZ' || u === 'P*' || v === 'P*');
        if (startId !== 'PZ' && startId !== 'P*' && isParking) return true;

        // Từ E7 đi E8: đi thẳng theo trục Đường Sáng Tạo S1 -> S2 -> Q -> DN_NE -> R2 -> D3 -> X1 -> E8 (theo đúng bản vẽ người dùng)
        // Cấm rẽ xuống H1 vòng qua Giảng đường Đa Năng
        if (startId === 'E7' || startId === 'NHÀ E7' || startId === 'S1' || startId === 'S2') {
            if (isH1S1) return true;
        } else {
            if (isQR2) {
                return true; // Chặn ngõ hẹp sau lưng E3/E4
            }
        }
        const startResolved = resolveAlias(startId);
        if (startResolved === 'E10' || startId === 'E10' || startId === 'NHÀ E10') {
            if (isXEE10 || isA0A1 || isZ6Z5 || isA2A1) {
                return true; // Từ E10 sang E8 đi thẳng qua trục ngang phía Bắc A0 -> D0 -> Đường Hạnh Hợp
            }
        }
        if ((u === 'X1' && (v === 'D2' || v === 'D1')) || (v === 'X1' && (u === 'D2' || u === 'D1'))) {
            const isNorthSource = ['E10', 'NHÀ E10', 'D1', 'D0', 'E9', 'NHÀ E9', 'E6', 'E6_W', 'NHÀ E6', 'A0', 'A1'].includes(startId) ||
                                  ['E10', 'NHÀ E10', 'D1', 'D0', 'E9', 'NHÀ E9', 'E6', 'E6_W', 'NHÀ E6', 'A0', 'A1'].includes(startResolved);
            if (!isNorthSource) {
                return true; // Tuyệt đối không đi lố từ X1 lên D2/D1 rồi mới sang E8
            }
        }
        if (startId === 'G' || startId === 'T' || startId === 'W1' || startId === 'H1') {
            if (isH1S1) {
                return true; // Chặn rẽ vào Đường Sáng Tạo S1, ép đi thẳng trục chính Hội Nhập H1 -> H2 -> H3
            }
            if (isD2D1 || isX1D1) {
                return true; // Tuyến từ Cổng phía Nam không đi qua D1
            }
        }
    }


    return false;
}

// ============================================================================
//  XÂY DỰNG DANH SÁCH KỀ (Adjacency List)
//  adj[u] = [{ v, w }, ...]  —  vô hướng
// ============================================================================
const adj = {};
for (const id of Object.keys(UTC2_NODES)) adj[id] = [];

for (const [u, v, w, type] of UTC2_EDGES_RAW) {
    if (!adj[u]) adj[u] = [];
    if (!adj[v]) adj[v] = [];
    if (!adj[u].some(e => e.v === v)) {
        adj[u].push({ v, w, type: type || 'branch' });
    }
    if (!adj[v].some(e => e.v === u)) {
        adj[v].push({ v: u, w, type: type || 'branch' });
    }
}

/** Tính tổng khoảng cách thực tế (mét) của một lộ trình nút */
function calculatePathDistance(path) {
    if (!path || path.length < 2) return 0;
    let dist = 0;
    for (let i = 0; i < path.length - 1; i++) {
        const u = path[i], v = path[i + 1];
        const e = adj[u]?.find(x => x.v === v);
        if (e) dist += e.w;
    }
    return dist;
}

// ============================================================================
//  MIN-HEAP tự cài đặt (không dùng thư viện ngoài)
//  Lưu phần tử: { key: number, val: any }
// ============================================================================
class MinHeap {
    constructor() { this._data = []; }

    get size() { return this._data.length; }
    get isEmpty() { return this._data.length === 0; }

    _parent(i) { return (i - 1) >> 1; }
    _left(i)   { return (i << 1) + 1; }
    _right(i)  { return (i << 1) + 2; }

    _swap(i, j) {
        const tmp = this._data[i];
        this._data[i] = this._data[j];
        this._data[j] = tmp;
    }

    _siftUp(i) {
        while (i > 0) {
            const p = this._parent(i);
            if (this._data[i].key < this._data[p].key) {
                this._swap(i, p);
                i = p;
            } else break;
        }
    }

    _siftDown(i) {
        const n = this._data.length;
        while (true) {
            let s = i;
            const l = this._left(i), r = this._right(i);
            if (l < n && this._data[l].key < this._data[s].key) s = l;
            if (r < n && this._data[r].key < this._data[s].key) s = r;
            if (s !== i) { this._swap(i, s); i = s; } else break;
        }
    }

    /** Thêm phần tử { key, val } vào heap */
    insert(key, val) {
        this._data.push({ key, val });
        this._siftUp(this._data.length - 1);
    }

    /** Trích phần tử nhỏ nhất khỏi heap */
    extractMin() {
        if (this.isEmpty) return null;
        const top = this._data[0];
        const last = this._data.pop();
        if (this._data.length > 0) {
            this._data[0] = last;
            this._siftDown(0);
        }
        return top;
    }
}

// ============================================================================
//  PHÂN TÍCH MÃ NÚT (alias + dual-entry)
// ============================================================================

/** Chuyển tên/alias → mã nút cơ sở (không giải quyết dual-entry) */
function resolveAlias(id) {
    if (!id) return '';
    const upper = id.toUpperCase().trim();
    if (NODE_ALIASES[upper]) return NODE_ALIASES[upper];
    // Kiểm tra trực tiếp
    if (UTC2_NODES[id]) return id;
    // Tìm case-insensitive
    for (const k of Object.keys(UTC2_NODES)) {
        if (k.toUpperCase() === upper) return k;
    }
    return id;
}

/** Trả về danh sách các sảnh xuất phát ứng viên khi xuất phát từ src (hỗ trợ đa sảnh Nhà C2) */
function resolveOriginNodes(src) {
    if (!src) return ['G'];
    const upper = src.toUpperCase().trim();
    if (upper === 'C2' || upper === 'NHÀ C2' || upper === 'NHAC2' || upper === 'PHÒNG C2') {
        return ['C2_Front', 'K']; // Sảnh trước C2_Front (Bắc - nối Đa năng), Sảnh sau K (Nam - nối J/C1)
    }
    if (upper === 'C1' || upper === 'NHÀ C1' || upper === 'HỘI TRƯỜNG C1' || upper === 'C1_EAST') {
        return ['C1'];
    }
    const base = resolveAlias(src);
    if (base === 'C2') return ['C2_Front', 'K'];
    if (base === 'C1') return ['C1'];
    if (DUAL_ENTRY[base]) return DUAL_ENTRY[base];
    return [base];
}

/** Trả về danh sách các nút vật lý cần thử khi tìm đường đến dest (hỗ trợ đa sảnh C2) */
function resolveTargetNodes(dest) {
    if (!dest) return ['K'];
    const upper = dest.toUpperCase().trim();
    if (upper === 'C2' || upper === 'NHÀ C2' || upper === 'NHAC2' || upper === 'PHÒNG C2') {
        return ['C2_Front', 'K']; // Ưu tiên sảnh trước C2_Front (Bắc), tự động chọn sảnh gần nhất
    }
    if (upper === 'C1' || upper === 'NHÀ C1' || upper === 'HỘI TRƯỜNG C1' || upper === 'C1_EAST') {
        return ['C1'];
    }
    const base = resolveAlias(dest);
    if (base === 'C2') return ['C2_Front', 'K'];
    if (base === 'C1') return ['C1'];
    if (DUAL_ENTRY[base]) return DUAL_ENTRY[base];
    if (UTC2_NODES[base]) return [base];
    return [base];
}

/** Kiểm tra cạnh có trong tập blocked không */
function isEdgeBlocked(u, v, blockedSet) {
    if (!blockedSet || blockedSet.size === 0) return false;
    if (blockedSet.has(`${u}-${v}`) || blockedSet.has(`${v}-${u}`)) return true;

    // Tương thích ngược: Nếu chặn toàn tuyến Q-R2 thì các phân đoạn nhỏ Q-DN_NE và DN_NE-R2 cũng bị chặn
    if ((blockedSet.has('Q-R2') || blockedSet.has('R2-Q')) &&
        ((u === 'Q' && v === 'DN_NE') || (u === 'DN_NE' && v === 'Q') ||
         (u === 'DN_NE' && v === 'R2') || (u === 'R2' && v === 'DN_NE'))) {
        return true;
    }
    // Tương thích ngược: Nếu chặn H2-H3 thì H2-DN_SE và DN_SE-H3 cũng bị chặn
    if ((blockedSet.has('H2-H3') || blockedSet.has('H3-H2')) &&
        ((u === 'H2' && v === 'DN_SE') || (u === 'DN_SE' && v === 'H2') ||
         (u === 'DN_SE' && v === 'H3') || (u === 'H3' && v === 'DN_SE'))) {
        return true;
    }

    return false;
}

/** Parse danh sách cạnh bị chặn: nhận chuỗi "T-J,W1-H1", mảng ['T-J', 'W1-H1'], hoặc Set */
function parseBlocked(blockedStr) {
    const s = new Set();
    if (!blockedStr) return s;
    if (blockedStr instanceof Set) return blockedStr;

    let items = [];
    if (typeof blockedStr === 'string') {
        const trimmed = blockedStr.trim();
        if (!trimmed) return s;
        items = trimmed.split(/[,;\n]+/).map(p => p.trim()).filter(Boolean);
    } else if (Array.isArray(blockedStr)) {
        items = blockedStr;
    }

    const addEdge = (u, v) => {
        s.add(`${u}-${v}`);
        s.add(`${v}-${u}`);
        // Tự động phân rã các cạnh ghép thành các đoạn thực tế trên đồ thị
        if ((u === 'Q' && v === 'R2') || (u === 'R2' && v === 'Q')) {
            s.add('Q-DN_NE');
            s.add('DN_NE-Q');
            s.add('DN_NE-R2');
            s.add('R2-DN_NE');
        } else if ((u === 'H2' && v === 'H3') || (u === 'H3' && v === 'H2')) {
            s.add('H2-DN_SE');
            s.add('DN_SE-H2');
            s.add('DN_SE-H3');
            s.add('H3-DN_SE');
        }
    };

    for (const item of items) {
        if (typeof item === 'string' && item.includes('-')) {
            const parts = item.split('-').map(x => x.trim());
            if (parts.length >= 2 && parts[0] && parts[1]) {
                addEdge(parts[0], parts[1]);
            }
        } else if (item && typeof item === 'object') {
            const u = item.u || item.from || item.source;
            const v = item.v || item.to || item.target;
            if (u && v) {
                addEdge(String(u).trim(), String(v).trim());
            }
        }
    }
    return s;
}

// ============================================================================
//  THUẬT TOÁN DIJKSTRA (hỗ trợ Turn Penalty, Road Multipliers, Cổng 451)
//  Trả về: { cost: {}, dist: {}, prev: {}, visitedCount: number }
// ============================================================================
function dijkstraRaw(source, blockedSet, target, isMultiStop = false) {
    const cost = {};
    const prev = {};
    const visited = {};
    const heap = new MinHeap();

    for (const id of Object.keys(UTC2_NODES)) {
        cost[id] = Infinity;
        prev[id] = null;
    }
    cost[source] = 0;
    heap.insert(0, source);

    let visitedCount = 0;

    while (!heap.isEmpty) {
        const { key: c, val: u } = heap.extractMin();
        if (visited[u]) continue;
        visited[u] = true;
        visitedCount++;

        if (target && u === target) break;

        const isCluster = isClusterC(target) || isClusterC(source);
        for (const { v, w } of (adj[u] || [])) {
            if (visited[v]) continue;
            if (isEdgeBlocked(u, v, blockedSet)) continue;
            if (isEdgeRestricted(u, v, source, target, blockedSet, isMultiStop)) continue;

            // Tính chi phí bước đi kết hợp hệ số loại đường & Turn Penalty
            const roadMult = getRoadMultiplier(u, v, isCluster);
            const turnPen = getTurnPenalty(prev[u], u, v);
            const stepCost = (w * roadMult) + turnPen;
            const newCost = c + stepCost;

            if (newCost < cost[v]) {
                cost[v] = newCost;
                prev[v] = u;
                heap.insert(newCost, v);
            }
        }
    }

    // Tính khoảng cách mét thực tế từ đường đi truy vết
    const dist = {};
    for (const id of Object.keys(UTC2_NODES)) {
        if (cost[id] === Infinity) {
            dist[id] = Infinity;
        } else {
            const p = tracePath(prev, id);
            dist[id] = calculatePathDistance(p);
        }
    }

    return { cost, dist, prev, visitedCount };
}

/** Truy vết đường từ prev (kết quả Dijkstra / A*) */
function tracePath(prev, target) {
    const path = [];
    let cur = target;
    while (cur !== null) {
        path.unshift(cur);
        cur = prev[cur];
    }
    return path;
}

// ============================================================================
//  THUẬT TOÁN A* (Euclidean Heuristic + Turn Penalty + Road Multipliers)
// ============================================================================
function euclidean(u, v) {
    const nu = UTC2_NODES[u], nv = UTC2_NODES[v];
    if (!nu || !nv) return 0;
    const dx = nu.x - nv.x, dy = nu.y - nv.y;
    return Math.sqrt(dx * dx + dy * dy) * 0.12; // scale heuristic
}

function astarRaw(source, target, blockedSet) {
    const g = {};
    const f = {};
    const prev = {};
    const closed = {};
    const heap = new MinHeap();

    for (const id of Object.keys(UTC2_NODES)) {
        g[id] = Infinity;
        f[id] = Infinity;
        prev[id] = null;
    }
    g[source] = 0;
    f[source] = euclidean(source, target);
    heap.insert(f[source], source);

    let visitedCount = 0;

    while (!heap.isEmpty) {
        const { val: u } = heap.extractMin();
        if (closed[u]) continue;
        closed[u] = true;
        visitedCount++;

        if (u === target) break;

        for (const { v, w } of (adj[u] || [])) {
            if (closed[v]) continue;
            if (isEdgeBlocked(u, v, blockedSet)) continue;
            if (isEdgeRestricted(u, v, source, target)) continue;

            const roadMult = getRoadMultiplier(u, v);
            const turnPen = getTurnPenalty(prev[u], u, v);
            const stepCost = (w * roadMult) + turnPen;
            const ng = g[u] + stepCost;

            if (ng < g[v]) {
                g[v] = ng;
                f[v] = ng + euclidean(v, target);
                prev[v] = u;
                heap.insert(f[v], v);
            }
        }
    }

    const path = g[target] === Infinity ? [] : tracePath(prev, target);
    const dist = path.length > 0 ? calculatePathDistance(path) : null;

    return {
        dist,
        cost: g[target] === Infinity ? null : g[target],
        prev,
        visitedCount,
        path
    };
}

// ============================================================================
//  BFS (đếm số hop, không xét trọng số)
// ============================================================================
function bfsRaw(source, target, blockedSet) {
    const visited = {};
    const prev = {};
    const queue = [source];
    visited[source] = true;
    prev[source] = null;
    let visitedCount = 0;

    while (queue.length > 0) {
        const u = queue.shift();
        visitedCount++;
        if (u === target) break;
        for (const { v } of (adj[u] || [])) {
            if (visited[v]) continue;
            if (isEdgeBlocked(u, v, blockedSet)) continue;
            visited[v] = true;
            prev[v] = u;
            queue.push(v);
        }
    }

    if (!visited[target]) return { dist: null, path: [], visitedCount };

    const path = tracePath(prev, target);
    // Tính tổng khoảng cách theo trọng số thực
    let dist = 0;
    for (let i = 1; i < path.length; i++) {
        const u = path[i - 1], v = path[i];
        const e = adj[u].find(e => e.v === v);
        if (e) dist += e.w;
    }
    return { dist, path, visitedCount };
}

// ============================================================================
//  TURN-BY-TURN (Cross Product xác định hướng rẽ)
// ============================================================================
function getCoord(id) {
    const n = UTC2_NODES[id];
    return n ? { x: n.x, y: n.y } : null;
}

function crossProduct(ax, ay, bx, by) {
    return ax * by - ay * bx;
}

function angleDeg(ax, ay, bx, by) {
    return Math.atan2(crossProduct(ax, ay, bx, by), ax * bx + ay * by) * (180 / Math.PI);
}

function getTurnAction(angle) {
    const a = Math.abs(angle);
    if (a < 20)       return { action: 'straight',    action_text: 'Đi thẳng',        icon: '⬆️' };
    if (angle > 0)    return { action: 'slight_left', action_text: 'Rẽ trái',         icon: '↖️' };
    return              { action: 'slight_right', action_text: 'Rẽ phải',        icon: '↗️' };
}

function generateTurnByTurn(path) {
    if (!path || path.length < 2) return [];
    const steps = [];

    // Bước 1: xuất phát
    steps.push({
        step: 1,
        action: 'start',
        action_text: 'Xuất phát',
        icon: '🚩',
        angle: 0,
        instruction: `Từ ${UTC2_NODES[path[0]]?.name || path[0]} xuất phát, đi về phía ${UTC2_NODES[path[1]]?.name || path[1]}`,
    });

    for (let i = 1; i < path.length - 1; i++) {
        const prev = path[i - 1], cur = path[i], next = path[i + 1];
        const p = getCoord(prev), c = getCoord(cur), n = getCoord(next);
        if (!p || !c || !n) continue;

        const ax = c.x - p.x, ay = c.y - p.y; // vector đến đây
        const bx = n.x - c.x, by = n.y - c.y; // vector tiếp theo
        const angle = angleDeg(ax, ay, bx, by);

        const edgeNext = adj[cur]?.find(e => e.v === next);
        const dist = edgeNext ? edgeNext.w : 0;

        const turn = getTurnAction(angle);
        steps.push({
            step: i + 1,
            ...turn,
            angle: Math.round(angle),
            instruction: `${turn.action_text !== 'Đi thẳng' ? '(' + turn.action_text + ') ' : ''}Tại ${UTC2_NODES[cur]?.name || cur}, ${turn.action === 'straight' ? 'đi thẳng sang' : 'rẽ sang'} ${UTC2_NODES[next]?.name || next} (${dist}m)`,
        });
    }

    // Bước cuối: đến nơi
    const last = path[path.length - 1];
    steps.push({
        step: path.length,
        action: 'arrive',
        action_text: 'Đến nơi',
        icon: '🏁',
        angle: 0,
        instruction: `Đã đến điểm đích: ${UTC2_NODES[last]?.name || last}!`,
    });

    return steps;
}

// ============================================================================
//  TÌM ĐƯỜNG NGẮN NHẤT CHÍNH (hỗ trợ dual-entry & blocked edges)
// ============================================================================
function coreFindShortestPath(startId, endId, blockedSet) {
    const origins = resolveOriginNodes(startId);
    const endResolved = resolveAlias(endId);
    const targets = resolveTargetNodes(endId);

    let bestDist = Infinity, bestCost = Infinity, bestTarget = null, bestOrigin = null, bestPath = [];

    // Tìm lộ trình tối ưu nhất theo cost duyệt qua tất cả cặp (origin, target)
    for (const src of origins) {
        if (!UTC2_NODES[src]) continue;
        for (const t of targets) {
            if (!UTC2_NODES[t]) continue;
            const { cost, prev } = dijkstraRaw(src, blockedSet, t);
            if (prev[t] !== null || src === t) {
                const path = tracePath(prev, t);
                const dist = calculatePathDistance(path);
                // Ưu tiên sảnh có khoảng cách mét thực tế ngắn nhất
                const isBetter = (dist < bestDist) || (dist === bestDist && cost[t] < bestCost);
                if (isBetter) {
                    bestCost = cost[t];
                    bestOrigin = src;
                    bestTarget = t;
                    bestPath = path;
                    bestDist = dist;
                }
            }
        }
    }

    if (bestTarget === null || bestDist === Infinity || bestPath.length === 0) {
        return { error: `Không tìm thấy đường đi từ "${startId}" đến "${endId}". Kiểm tra cạnh bị chặn.` };
    }

    const walkingSec = Math.round(bestDist / WALKING_SPEED);
    const turnByTurn = generateTurnByTurn(bestPath);

    // Tạo directions (simple text)
    const directions = [];
    for (let i = 0; i < bestPath.length - 1; i++) {
        const u = bestPath[i], v = bestPath[i + 1];
        const e = adj[u]?.find(x => x.v === v);
        const w = e ? e.w : 0;
        const cumDist = calculatePathDistance(bestPath.slice(0, i + 2));
        directions.push(
            `${i === 0 ? 'Từ' : 'Đến'} ${UTC2_NODES[u]?.name || u} đi đến ${UTC2_NODES[v]?.name || v} (${w}m, tổng: ${cumDist}m)`
        );
    }
    directions.push(`Đã đến ${UTC2_NODES[bestTarget]?.name || bestTarget}`);

    return {
        start: resolveAlias(startId),
        start_node: bestOrigin,
        end: endResolved,
        entry_node: bestTarget,
        total_distance: bestDist,
        routing_cost: Math.round(bestCost * 10) / 10,
        walking_time_sec: walkingSec,
        path: bestPath,
        directions,
        turn_by_turn: turnByTurn,
    };
}

// ============================================================================
//  MULTI-STOP ROUTING
//  Brute-force (≤5 điểm trung gian), Nearest Neighbor (>5)
// ============================================================================

function computeSegmentDist(a, b, blockedSet) {
    const { dist } = dijkstraRaw(a, blockedSet, b, true);
    return dist[b] !== undefined ? dist[b] : Infinity;
}

function computeSegmentPath(a, b, blockedSet) {
    const { dist, prev } = dijkstraRaw(a, blockedSet, b, true);
    if (dist[b] === Infinity) return null;
    return { dist: dist[b], path: tracePath(prev, b) };
}

function permutations(arr) {
    if (arr.length <= 1) return [arr];
    const result = [];
    for (let i = 0; i < arr.length; i++) {
        const rest = [...arr.slice(0, i), ...arr.slice(i + 1)];
        for (const perm of permutations(rest)) {
            result.push([arr[i], ...perm]);
        }
    }
    return result;
}

function findMultiStop(stops, blockedSet) {
    if (!stops || stops.length < 2) return { error: 'Cần ít nhất 2 điểm dừng' };

    // Ánh xạ các điểm dừng logic (C2, E3, E4, E5) sang sảnh vật lý tối ưu
    const originRaw = resolveAlias(stops[0]);
    const resolvedStops = stops.map(s => {
        const alias = resolveAlias(s);
        if (alias === 'C2') {
            // Với C2: từ phía Nam (G, T, J) thì chọn K (Sảnh sau); từ phía Bắc thì chọn C2_Front
            if (originRaw === 'G' || originRaw === 'T' || originRaw === 'J') return 'K';
            return 'C2_Front';
        }
        if (alias === 'E3') {
            if (originRaw === 'E10' || originRaw === 'A0' || originRaw === 'E6_W' || originRaw === 'D0') return 'E3w';
            return 'D3';
        }
        if (alias === 'E4') {
            if (originRaw === 'E10' || originRaw === 'A0') return 'E4w';
            return 'D2';
        }
        if (alias === 'E5') {
            if (originRaw === 'E10' || originRaw === 'A0') return 'E5w';
            return 'D1';
        }
        if (DUAL_ENTRY[alias]) return DUAL_ENTRY[alias][0];
        return alias;
    });
    const origin = resolvedStops[0];
    
    // Khử trùng lặp và loại bỏ origin nếu xuất hiện trong waypoints
    const seen = new Set([origin]);
    const waypoints = [];
    for (let i = 1; i < resolvedStops.length; i++) {
        const s = resolvedStops[i];
        if (!seen.has(s)) {
            seen.add(s);
            waypoints.push(s);
        }
    }

    if (waypoints.length === 0) {
        return { error: 'Vui lòng chọn ít nhất 1 điểm dừng khác điểm xuất phát' };
    }

    // Tính ma trận khoảng cách giữa tất cả điểm (cho phép T-J linh hoạt tối đa)
    const allPoints = [origin, ...waypoints];
    const distMatrix = {};
    const pathMatrix = {};

    for (const a of allPoints) {
        distMatrix[a] = {};
        pathMatrix[a] = {};
        for (const b of allPoints) {
            if (a === b) {
                distMatrix[a][b] = 0;
                pathMatrix[a][b] = [a];
                continue;
            }
            const { dist, prev } = dijkstraRaw(a, blockedSet, b, true);
            const path = (dist[b] !== undefined && dist[b] < Infinity) ? tracePath(prev, b) : null;
            pathMatrix[a][b] = path;
            distMatrix[a][b] = path ? calculatePathDistance(path) : Infinity;
        }
    }

    let bestOrder = null, bestTotal = Infinity;
    let algorithm = 'Brute-force (Vét cạn tối ưu)';

    if (waypoints.length <= 7) {
        // Vét cạn tất cả hoán vị
        for (const perm of permutations(waypoints)) {
            let total = 0;
            let prev = origin;
            let ok = true;
            for (const p of perm) {
                const d = distMatrix[prev]?.[p] ?? Infinity;
                if (d === Infinity) { ok = false; break; }
                total += d;
                prev = p;
            }
            if (ok && total < bestTotal) {
                bestTotal = total;
                bestOrder = [origin, ...perm];
            }
        }
    }

    // Nếu không tìm được qua hoán vị hoặc danh sách trạm lớn hơn 6: Dùng Nearest Neighbor (Tham lam)
    if (!bestOrder) {
        algorithm = 'Nearest Neighbor (Láng giềng gần nhất)';
        const remaining = new Set(waypoints);
        bestOrder = [origin];
        bestTotal = 0;
        let cur = origin;
        while (remaining.size > 0) {
            let nearest = null, nearDist = Infinity;
            for (const p of remaining) {
                const d = distMatrix[cur]?.[p] ?? Infinity;
                if (d < nearDist) {
                    nearDist = d;
                    nearest = p;
                }
            }
            if (!nearest || nearDist === Infinity) {
                const fallbackNode = Array.from(remaining)[0];
                bestOrder.push(fallbackNode);
                remaining.delete(fallbackNode);
                cur = fallbackNode;
                continue;
            }
            bestTotal += nearDist;
            bestOrder.push(nearest);
            remaining.delete(nearest);
            cur = nearest;
        }

        // Tối ưu hóa 2-Opt để khử hoàn toàn đường zic-zắc, cắt chéo hoặc lộn ngược
        function calcRouteCost(r) {
            let sum = 0;
            for (let k = 0; k < r.length - 1; k++) {
                sum += distMatrix[r[k]]?.[r[k + 1]] ?? Infinity;
            }
            return sum;
        }

        let improved = true;
        const n = bestOrder.length;
        let iter = 0;
        while (improved && iter < 100) {
            improved = false;
            iter++;
            for (let i = 1; i < n - 1; i++) {
                for (let j = i + 1; j < n; j++) {
                    const candidate = bestOrder.slice(0, i).concat(bestOrder.slice(i, j + 1).reverse(), bestOrder.slice(j + 1));
                    if (calcRouteCost(candidate) < calcRouteCost(bestOrder) - 1e-4) {
                        bestOrder = candidate;
                        improved = true;
                        break;
                    }
                }
                if (improved) break;
            }
        }
        algorithm = 'Nearest Neighbor + 2-Opt (Tối ưu hóa hành trình)';
    }

    if (!bestOrder || bestOrder.length < 2) return { error: 'Không tìm được lộ trình đa điểm hợp lệ' };

    // Ghép chặng vẽ đầy đủ từng trạm tuần tự
    const fullPath = [];
    const legs = [];
    let totalDist = 0;

    for (let i = 0; i < bestOrder.length - 1; i++) {
        const from = bestOrder[i];
        const to = bestOrder[i + 1];
        let seg = pathMatrix[from]?.[to];
        let d = distMatrix[from]?.[to];

        if (!seg || d === Infinity) {
            const sub = findShortestPath(from, to, blockedSet);
            if (sub && sub.path && sub.path.length > 0) {
                seg = sub.path;
                d = sub.total_distance;
            }
        }

        if (!seg || seg.length === 0) {
            const fromName = UTC2_NODES[from]?.name || from;
            const toName = UTC2_NODES[to]?.name || to;
            return { error: `Không thể tìm thấy đường đi từ "${fromName}" đến "${toName}"! Vui lòng kiểm tra lại cạnh bị chặn.` };
        }

        totalDist += (d !== undefined && d < Infinity) ? d : calculatePathDistance(seg);

        if (fullPath.length === 0) {
            fullPath.push(...seg);
        } else {
            fullPath.push(...seg.slice(1));
        }

        legs.push({
            legIndex: i + 1,
            from,
            to,
            fromName: UTC2_NODES[from]?.name || from,
            toName: UTC2_NODES[to]?.name || to,
            distance: d !== undefined && d < Infinity ? d : calculatePathDistance(seg),
            path: seg,
            turnByTurn: generateTurnByTurn(seg)
        });
    }

    const walkSec = Math.round(totalDist / WALKING_SPEED);
    const turnByTurn = generateTurnByTurn(fullPath);

    return {
        algorithm_used: algorithm,
        ordered_stops: bestOrder,
        ordered_stop_names: bestOrder.map(id => UTC2_NODES[id]?.name || id),
        total_distance: totalDist,
        walking_time_sec: walkSec,
        full_path: fullPath,
        legs: legs,
        turn_by_turn: turnByTurn,
        directions: turnByTurn.map(s => s.instruction),
    };
}

// ============================================================================
//  SCHEDULE OPTIMIZER (kiểm tra thời khóa biểu, cảnh báo trễ học)
// ============================================================================
function scheduleCheck(room1, room2, breakTimeSec, blockedSet) {
    const src = resolveAlias(room1);
    const dst = resolveAlias(room2);

    // Với dual-entry: tìm entry node tối ưu
    const result = findShortestPath(src, dst, blockedSet || new Set());

    if (result.error) {
        return { error: result.error };
    }

    const dist = result.total_distance;
    const walkSec = result.walking_time_sec;
    const isAtRisk = walkSec > breakTimeSec;
    const margin = breakTimeSec - walkSec;

    const srcName = UTC2_NODES[src]?.name || src;
    const dstName = UTC2_NODES[dst]?.name || dst;

    let status, message, badgeClass;
    if (isAtRisk) {
        status = 'NGUY CƠ TRỄ HỌC';
        badgeClass = 'danger';
        message = `[CẢNH BÁO ĐỎ] NGUY CƠ TRỄ HỌC: Quãng đường ${Math.round(dist)}m, cần ${walkSec}s đi bộ (vượt quá giờ nghỉ ${breakTimeSec}s là ${-margin}s). Sinh viên cần rời lớp sớm hoặc tăng tốc!`;
    } else {
        status = 'AN TOÀN - KỊP GIỜ VÀO LỚP';
        badgeClass = 'success';
        message = `Lộ trình thuận lợi: ${Math.round(dist)}m, cần ${walkSec}s đi bộ (dưới ${breakTimeSec}s). Bạn còn dư ${margin}s giải lao.`;
    }

    return {
        room1: src,
        room2: dst,
        room1_name: srcName,
        room2_name: dstName,
        distance: Math.round(dist * 10) / 10,
        total_distance: Math.round(dist * 10) / 10,
        walking_time_sec: walkSec,
        break_time_sec: breakTimeSec,
        is_at_risk: isAtRisk,
        margin_sec: margin,
        status,
        message,
        badge_class: badgeClass,
        path: result.path,
        entry_node: result.entry_node,
        directions: result.directions,
        turn_by_turn: result.turn_by_turn,
    };
}

// ============================================================================
//  THUẬT TOÁN BELLMAN-FORD (tìm đường ngắn nhất & kiểm tra chu trình âm)
//  Độ phức tạp O(V * E)
// ============================================================================
function bellmanFordRaw(source, blockedSet, target) {
    const cost = {};
    const prev = {};
    const isCluster = isClusterC(target) || isClusterC(source);

    for (const id of Object.keys(UTC2_NODES)) {
        cost[id] = Infinity;
        prev[id] = null;
    }
    cost[source] = 0;

    // Lọc danh sách các cạnh hợp lệ
    const validEdges = [];
    for (const [u, v, w] of UTC2_EDGES_RAW) {
        if (isEdgeBlocked(u, v, blockedSet)) continue;
        if (!isEdgeRestricted(u, v, source, target, blockedSet)) {
            const mult = getRoadMultiplier(u, v, isCluster);
            validEdges.push({ u, v, cost: w * mult, w });
        }
        if (!isEdgeRestricted(v, u, source, target, blockedSet)) {
            const mult = getRoadMultiplier(v, u, isCluster);
            validEdges.push({ u: v, v: u, cost: w * mult, w });
        }
    }

    const vertices = Object.keys(UTC2_NODES);
    const numVertices = vertices.length;
    let visitedCount = 0;

    // Nới lỏng cạnh |V| - 1 lần
    for (let i = 1; i < numVertices; i++) {
        let changed = false;
        for (const { u, v, cost: stepCost } of validEdges) {
            visitedCount++;
            if (cost[u] !== Infinity) {
                const turnPen = getTurnPenalty(prev[u], u, v);
                const totalCost = cost[u] + stepCost + turnPen;
                if (totalCost < cost[v]) {
                    cost[v] = totalCost;
                    prev[v] = u;
                    changed = true;
                }
            }
        }
        if (!changed) break; // Dừng sớm nếu không còn nới lỏng được
    }

    // Kiểm tra chu trình âm
    let hasNegativeCycle = false;
    for (const { u, v, cost: stepCost } of validEdges) {
        if (cost[u] !== Infinity && cost[u] + stepCost < cost[v]) {
            hasNegativeCycle = true;
            break;
        }
    }

    const path = (target && cost[target] !== Infinity) ? tracePath(prev, target) : [];
    const dist = path.length > 0 ? calculatePathDistance(path) : null;

    return { cost, dist, prev, visitedCount, hasNegativeCycle, path };
}

// ============================================================================
//  THUẬT TOÁN PRIM (Cây khung nhỏ nhất - Minimum Spanning Tree MST)
//  Độ phức tạp O((V + E) log V)
// ============================================================================
function primMSTRaw(source, blockedSet) {
    const key = {};
    const parent = {};
    const inMST = {};
    const heap = new MinHeap();

    for (const id of Object.keys(UTC2_NODES)) {
        key[id] = Infinity;
        parent[id] = null;
        inMST[id] = false;
    }

    key[source] = 0;
    heap.insert(0, source);

    let visitedCount = 0;
    let totalMstWeight = 0;
    const mstEdges = [];

    while (!heap.isEmpty) {
        const { val: u } = heap.extractMin();
        if (inMST[u]) continue;
        inMST[u] = true;
        visitedCount++;

        if (parent[u] !== null) {
            const edge = adj[u]?.find(e => e.v === parent[u]);
            const w = edge ? edge.w : key[u];
            totalMstWeight += w;
            mstEdges.push({ u: parent[u], v: u, weight: w });
        }

        for (const { v, w } of (adj[u] || [])) {
            if (inMST[v]) continue;
            if (isEdgeBlocked(u, v, blockedSet)) continue;
            if (w < key[v]) {
                key[v] = w;
                parent[v] = u;
                heap.insert(w, v);
            }
        }
    }

    return {
        totalMstWeight,
        visitedCount,
        mstEdges,
        connectedVertices: visitedCount
    };
}

// ============================================================================
//  BENCHMARK: So sánh 3 giải thuật cốt lõi trong chương trình môn học DSA:
//  1. Dijkstra (Tìm đường ngắn nhất với Min-Heap)
//  2. Bellman-Ford (Tìm đường ngắn nhất + Kiểm tra chu trình âm)
//  3. Prim (Cây khung nhỏ nhất MST)
// ============================================================================
function runBenchmark(startId, endId, blockedSet) {
    const origins = resolveOriginNodes(startId);
    const targets = resolveTargetNodes(endId);
    const blocked = blockedSet || new Set();

    // Tự động tìm sảnh tối ưu nhất giữa các cửa vào (giống hệt thuật toán tìm đường chính coreFindShortestPath)
    let bestSrc = origins[0];
    let bestTarget = targets[0];
    let bestDist = Infinity;

    for (const s of origins) {
        if (!UTC2_NODES[s]) continue;
        for (const t of targets) {
            if (!UTC2_NODES[t]) continue;
            const r = dijkstraRaw(s, blocked, t);
            if (r.dist[t] < bestDist) {
                bestDist = r.dist[t];
                bestSrc = s;
                bestTarget = t;
            }
        }
    }

    const src = bestSrc;
    const target = bestTarget;

    const metrics = [];
    const RUNS = 20; // Đo trung bình 20 lượt

    // --- 1. Dijkstra (Min-Heap) ---
    let tStart = performance.now();
    let dijkDist = null, dijkVisited = 0;
    for (let i = 0; i < RUNS; i++) {
        const r = dijkstraRaw(src, blocked, target);
        dijkDist = r.dist[target];
        dijkVisited = r.visitedCount;
    }
    const dijkTime = ((performance.now() - tStart) / RUNS * 1000).toFixed(1);

    const doorSuffix = (target !== endId && UTC2_NODES[target]) ? ` (${UTC2_NODES[target].name})` : '';

    metrics.push({
        name: 'Dijkstra',
        category: 'Đường đi ngắn nhất',
        description: 'Tối ưu với Min-Heap, phù hợp đồ thị trọng số không âm',
        complexity: 'O((V + E) log V)',
        time_us: parseFloat(dijkTime),
        visited_nodes: dijkVisited,
        distance: dijkDist === Infinity ? null : Math.round(dijkDist * 10) / 10,
        result_label: dijkDist === Infinity ? 'Không có đường' : `${Math.round(dijkDist)} m${doorSuffix}`,
    });

    // --- 2. Bellman-Ford ---
    tStart = performance.now();
    let bfDist = null, bfVisited = 0, bfHasNeg = false;
    for (let i = 0; i < RUNS; i++) {
        const r = bellmanFordRaw(src, blocked, target);
        bfDist = r.dist;
        bfVisited = r.visitedCount;
        bfHasNeg = r.hasNegativeCycle;
    }
    const bfTime = ((performance.now() - tStart) / RUNS * 1000).toFixed(1);

    metrics.push({
        name: 'Bellman-Ford',
        category: 'Đường đi / Chu trình âm',
        description: 'Nới lỏng |V|-1 lần các cạnh, an toàn với trọng số âm và chu trình âm',
        complexity: 'O(V × E)',
        time_us: parseFloat(bfTime),
        visited_nodes: bfVisited,
        distance: bfDist === null ? null : Math.round(bfDist * 10) / 10,
        result_label: bfDist === null ? 'Không có đường' : `${Math.round(bfDist)} m (Không chu trình âm)${doorSuffix}`,
    });

    // --- 3. Prim (MST) ---
    tStart = performance.now();
    let primTotal = 0, primVisited = 0;
    for (let i = 0; i < RUNS; i++) {
        const r = primMSTRaw(src, blocked);
        primTotal = r.totalMstWeight;
        primVisited = r.visitedCount;
    }
    const primTime = ((performance.now() - tStart) / RUNS * 1000).toFixed(1);

    metrics.push({
        name: 'Prim (MST)',
        category: 'Cây khung nhỏ nhất',
        description: 'Xây dựng cây khung nhỏ nhất kết nối toàn khuôn viên với chi phí tối thiểu',
        complexity: 'O((V + E) log V)',
        time_us: parseFloat(primTime),
        visited_nodes: primVisited,
        distance: primTotal,
        result_label: `Tổng MST: ${primTotal} m (${primVisited} đỉnh liên thông)`,
    });

    const conclusion = `Cả Dijkstra và Bellman-Ford đều cho kết quả đường đi ngắn nhất hoàn toàn đồng nhất (${metrics[0].distance}m). Dijkstra với cấu trúc Min-Heap đạt tốc độ cao vượt trội (${metrics[0].time_us} &mu;s) so với Bellman-Ford (${metrics[1].time_us} &mu;s). Thuật toán Prim giải quyết bài toán Cây khung nhỏ nhất (MST), kết nối liên thông ${primVisited} địa điểm trong khuôn viên UTC2 với tổng chiều dài đường nối tối ưu chỉ ${primTotal}m.`;

    return {
        start: src,
        dest: target,
        metrics,
        conclusion,
    };
}

// ============================================================================
//  API CHUNG dành cho app.js gọi (thay thế fetch)
// ============================================================================
const GraphEngine = {

    /** Danh sách tất cả nút (tương đương GET /api/nodes) */
    getNodes() {
        return {
            count: Object.keys(UTC2_NODES).length,
            nodes: Object.values(UTC2_NODES).map(n => ({
                id: n.id,
                name: n.name,
                description: n.description,
                x: n.x,
                y: n.y,
            })),
        };
    },

    /** Tìm đường ngắn nhất (tương đương GET /api/route) */
    route(start, end, blockedStr) {
        const blocked = parseBlocked(blockedStr);
        const result = coreFindShortestPath(start, end, blocked);
        if (result.error) throw new Error(result.error);
        return result;
    },

    findShortestPath(start, end, blockedStr) {
        return this.route(start, end, blockedStr);
    },

    dijkstra(start, end, blockedStr) {
        return this.route(start, end, blockedStr);
    },

    /** Đa điểm (tương đương POST /api/multi-stop) */
    multiStop(stops, blockedStr) {
        const blocked = parseBlocked(blockedStr);
        const result = findMultiStop(stops, blocked);
        if (result.error) throw new Error(result.error);
        return result;
    },

    /** Giải bài toán người du lịch đa điểm (TSP: Brute-force <= 6, Nearest Neighbor > 6) */
    solveTSP(startNodeId, selectedStops, blockedStr) {
        let stops = [];
        if (Array.isArray(startNodeId)) {
            stops = startNodeId;
            blockedStr = selectedStops;
        } else {
            stops = [startNodeId, ...(Array.isArray(selectedStops) ? selectedStops : [])];
        }
        return this.multiStop(stops, blockedStr);
    },

    runTSP(startNodeId, selectedStops, blockedStr) {
        return this.solveTSP(startNodeId, selectedStops, blockedStr);
    },

    solveMultiStops(startNodeId, selectedStops, blockedStr) {
        return this.solveTSP(startNodeId, selectedStops, blockedStr);
    },

    nearestNeighbor(startNodeId, selectedStops, blockedStr) {
        return this.solveTSP(startNodeId, selectedStops, blockedStr);
    },

    /** Thời khóa biểu (tương đương POST /api/schedule-check) */
    scheduleCheck(room1, room2, breakTimeSec, blockedStr) {
        const blocked = parseBlocked(blockedStr);
        const result = scheduleCheck(room1, room2, breakTimeSec, blocked);
        if (result.error) throw new Error(result.error);
        return result;
    },

    /** Benchmark (tương đương GET /api/benchmark) */
    benchmark(start, end, blockedStr) {
        const blocked = parseBlocked(blockedStr);
        return runBenchmark(start, end, blocked);
    },
};

// Cung cấp các hàm API ra phạm vi toàn cục (Window / Node.js)
if (typeof window !== 'undefined') {
    window.GraphEngine = GraphEngine;
    window.solveTSP = (start, stops, blocked) => GraphEngine.solveTSP(start, stops, blocked);
    window.runTSP = (start, stops, blocked) => GraphEngine.solveTSP(start, stops, blocked);
    window.solveMultiStops = (start, stops, blocked) => GraphEngine.solveTSP(start, stops, blocked);
    window.nearestNeighbor = (start, stops, blocked) => GraphEngine.solveTSP(start, stops, blocked);
    window.findShortestPath = (start, end, blocked) => GraphEngine.route(start, end, blocked);
}

if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        GraphEngine,
        UTC2_NODES,
        UTC2_EDGES_RAW,
        findShortestPath: (start, end, b) => GraphEngine.route(start, end, b),
        findMultiStop,
        solveTSP: (start, stops, b) => GraphEngine.solveTSP(start, stops, b),
        runTSP: (start, stops, b) => GraphEngine.solveTSP(start, stops, b),
        solveMultiStops: (start, stops, b) => GraphEngine.solveTSP(start, stops, b),
        nearestNeighbor: (start, stops, b) => GraphEngine.solveTSP(start, stops, b),
        scheduleCheck,
        runBenchmark,
    };
}


