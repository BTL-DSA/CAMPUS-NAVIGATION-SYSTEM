/**
 * canvasMap.js - Bộ điều khiển tương tác Canvas bản đồ UTC2
 * Hỗ trợ Zoom, Pan, vẽ lộ trình nét đứt phát sáng (glow) và nhấp nháy điểm đến
 */

// Bảng tọa độ pixel chuẩn hóa 48 nút trên ảnh map-utc2.png (1451 x 1084)
const NODE_COORDINATES = {
    // Trục Cổng chính & Đại lộ Thành công (Đường 449 mép Tây) - thẳng đứng X = 200
    'G':   { x: 200, y: 980, name: 'Cổng số 451', isGate: true },
    'T':   { x: 200, y: 840, name: 'Ngã ba sau Cổng 451' },
    'W1':  { x: 200, y: 630, name: 'Đại lộ Thành công W1' },
    'W2':  { x: 200, y: 492, name: 'Đại lộ Thành công W2' },
    'W3':  { x: 200, y: 315, name: 'Đại lộ Thành công W3' },
    'W4':  { x: 200, y: 110, name: 'Đại lộ Thành công W4' },

    // Trục ngang sau lưng C2 & Cụm C (Y = 840), Trục trước C2 & C1 (Y = 745)
    'J':    { x: 430, y: 840, name: 'Ngã tư dưới CV Thành công' },
    'K':    { x: 635, y: 840, name: 'Nhà C2 (Sảnh sau)', isBuilding: true },
    'C2':   { x: 635, y: 745, name: 'Nhà C2', isBuilding: true },
    'C1_J': { x: 780, y: 840, name: 'Sảnh Tây Nhà C1' },
    'C1_Front': { x: 780, y: 840, name: 'Nhà C1 (Sảnh Tây)', isBuilding: true },
    'C1':   { x: 890, y: 780, name: 'Nhà C1', isBuilding: true },
    'C1_East': { x: 890, y: 780, name: 'Nhà C1 (Phía Đông)' },
    'V1':   { x: 430, y: 745, name: 'Đường nội bộ V1' },
    'C2_Front': { x: 635, y: 745, name: 'Nhà C2 (Sảnh trước)', isBuilding: true },
    'V5':   { x: 665, y: 745, name: 'Ngã ba tim đường V5' },
    'V3':   { x: 780, y: 745, name: 'Đường nội bộ V3' },
    'V4':   { x: 920, y: 745, name: 'Ngã ba trước Nhà C3' },
    'V4_S': { x: 920, y: 840, name: 'Nhà C3', isBuilding: true },
    'C3':   { x: 970, y: 710, name: 'Nhà C3', isBuilding: true },
    'CT':   { x: 920, y: 710, name: 'Căn tin sinh viên', isBuilding: true },

    // Trục ngang Đại lộ Hội Nhập - thẳng tắp Y = 630
    'H1':  { x: 405, y: 630, name: 'Đường Hội nhập H1' },
    'H2':  { x: 665, y: 630, name: 'Đường Hội nhập H2' },
    'DN':  { x: 665, y: 600, name: 'Giảng đường Đa năng', isBuilding: true },
    'DN_SE':   { x: 820, y: 630, name: 'Lối Đông Đa năng (Nam)' },
    'DN_East': { x: 820, y: 560, name: 'Giảng đường Đa năng (Lối Đông)', isBuilding: true },
    'DN_NE':   { x: 820, y: 492, name: 'Lối Đông Đa năng (Bắc)' },
    'H3':  { x: 920, y: 630, name: 'Đường Hội nhập H3' },
    'HP':  { x: 1040, y: 630, name: 'Đường Hội nhập HP' },
    'HE8B':{ x: 1225, y: 630, name: 'Đường Hội nhập E8B' },
    'P*':  { x: 1040, y: 560, name: 'Nhà để xe', isBuilding: true },
    'PZ':  { x: 1040, y: 560, name: 'Nhà để xe', isBuilding: true },
    'P_North': { x: 1040, y: 440, name: 'Lối Bắc Nhà để xe' },
    'P_Road':  { x: 1040, y: 342, name: 'Ngã ba đường sau Nhà để xe' },
    'E8B': { x: 1225, y: 590, name: 'Nhà E8B', isBuilding: true },

    // Trục ngang Đường Sáng Tạo - thẳng tắp Y = 492
    'S1':  { x: 440, y: 492, name: 'Đường Sáng tạo S1' },
    'S2':  { x: 575, y: 492, name: 'Đường Sáng tạo S2' },
    'Q':   { x: 705, y: 492, name: 'Đường Sáng tạo Q' },
    'E7':  { x: 440, y: 315, name: 'Nhà E7', isBuilding: true },

    // Trục dọc Đường Ánh Đạo - thẳng đứng X = 575
    'A_North': { x: 575, y: 175, name: 'Đường Ánh Đạo (Bắc)' },
    'A2':  { x: 575, y: 315, name: 'Đường Ánh Đạo A2' },
    'E1':  { x: 630, y: 315, name: 'Nhà E1', isBuilding: true },
    'A1':  { x: 575, y: 110, name: 'Đường Ánh Đạo A1' },
    'E6':  { x: 755, y: 85,  name: 'Nhà E6', isBuilding: true },
    'E6_W':{ x: 755, y: 85,  name: 'Nhà E6', isBuilding: true },

    // Trục ngang Công viên Nghĩa Tình Y = 110 & Lối lên sảnh E10, Trục ngang mới Y = 85
    'XE':  { x: 410, y: 110, name: 'Đường dọc Công viên' },
    'N1':  { x: 440, y: 165, name: 'Ngã ba N1' },
    'E10': { x: 410, y: 85,  name: 'Nhà E10', isBuilding: true },
    'A0':  { x: 575, y: 85,  name: 'Ngã tư Ánh Đạo A0' },
    'Z6':  { x: 705, y: 85,  name: 'Hành lang Z6' },
    'E9':  { x: 920, y: 85,  name: 'Nhà E9', isBuilding: true },

    // Trục hành lang nội bộ Nhà E1 (X = 630)
    'E1_North': { x: 630, y: 190, name: 'Hành lang E1 (Bắc)' },
    'E1_Mid':   { x: 630, y: 315, name: 'Nhà E1', isBuilding: true },
    'E1_South': { x: 630, y: 440, name: 'Hành lang E1 (Nam)' },

    // Cụm giảng đường E3, E4, E5 (Lối Tây qua trục Z X = 705 & Cầu nối vào E1)
    'Z3':  { x: 705, y: 440, name: 'Hành lang Z3' },
    'E3w': { x: 740, y: 440, name: 'Nhà E3 (Tây)', isBuilding: true },
    'E3':  { x: 740, y: 440, name: 'Nhà E3', isBuilding: true },
    'Z4':  { x: 705, y: 315, name: 'Hành lang Z4' },
    'E4w': { x: 740, y: 315, name: 'Nhà E4 (Tây)', isBuilding: true },
    'E4':  { x: 740, y: 315, name: 'Nhà E4', isBuilding: true },
    'Z5':  { x: 705, y: 190, name: 'Hành lang Z5' },
    'E5w': { x: 740, y: 190, name: 'Nhà E5 (Tây)', isBuilding: true },
    'E5':  { x: 740, y: 190, name: 'Nhà E5', isBuilding: true },

    // Trục dọc Đường phía Đông - X = 920 & Nhánh vào E8 Y = 342
    'R2':  { x: 920, y: 492, name: 'Đường sau Nhà xe R2' },
    'D3':  { x: 920, y: 440, name: 'Nhà E3 (Đông)', isBuilding: true },
    'D2':  { x: 920, y: 315, name: 'Nhà E4 (Đông)', isBuilding: true },
    'X1':  { x: 920, y: 342, name: 'Ngã ba X1' },
    'D1':  { x: 920, y: 190, name: 'Nhà E5 (Đông)', isBuilding: true },
    'D0':  { x: 920, y: 85,  name: 'Ngã ba Đường Hạnh Hợp' },
    'E8':  { x: 1130, y: 342, name: 'Nhà E8', isBuilding: true }
};

class CanvasMap {
    constructor(canvasId) {
        this.canvas = document.getElementById(canvasId);
        this.ctx = this.canvas.getContext('2d');
        
        // Tải ảnh bản đồ
        this.mapImage = new Image();
        this.imageLoaded = false;
        this.initImage();
        
        // Trạng thái Pan & Zoom (Không bao giờ để lộ khoảng đen)
        this.scale = 1.0;
        this.minScale = 1.0;
        this.maxScale = 4.0;
        this.offsetX = 0;
        this.offsetY = 0;
        this.isDragging = false;
        this.dragStartX = 0;
        this.dragStartY = 0;
        
        // Dữ liệu lộ trình đang hiển thị
        this.currentRoute = [];      // Mảng mã nút ['G', 'T', 'J', 'K']
        this.blockedEdges = [];      // Mảng các cạnh bị chặn ['T-J', ...]
        this.selectedEntryNode = '';
        
        // Animation
        this.dashOffset = 0;
        this.pulsePhase = 0;
        this.animationFrameId = null;
        
        // Thiết lập sự kiện
        this.bindEvents();
        this.resizeCanvas();
        this.startAnimation();
    }

    initImage() {
        // Danh sách các đường dẫn và định dạng ảnh có thể có (.png, .jpg, .jpeg)
        const candidatePaths = [
            'assets/map-utc2.png',
            'assets/images/map-utc2.png',
            'assets/images/campus_map.png',
            'assets/campus_map.png',
            'assets/images/campus_map.jpg',
            'assets/campus_map.jpg',
            'assets/images/campus_map.jpeg',
            'assets/campus_map.jpeg'
        ];

        let pathIdx = 0;
        const tryNext = () => {
            if (pathIdx >= candidatePaths.length) {
                console.error('[CanvasMap] Không thể tìm thấy file ảnh bản đồ trong assets/ hoặc assets/images/');
                return;
            }
            const src = candidatePaths[pathIdx++];
            this.mapImage.src = src;
        };

        this.mapImage.onload = () => {
            this.imageLoaded = true;
            console.log(`[CanvasMap] Tải thành công ảnh sơ đồ bản đồ từ: ${this.mapImage.src}`);
            this.resetView();
            this.render();
        };

        this.mapImage.onerror = () => {
            console.warn(`[CanvasMap] Không tải được: ${this.mapImage.src}, đang thử đường dẫn tiếp theo...`);
            tryNext();
        };

        tryNext();
    }

    /**
     * Tỷ lệ scale tối thiểu để ảnh bản đồ luôn phủ kín 100% khung nhìn canvas,
     * ngăn chặn hoàn toàn việc thu nhỏ quá mức làm lộ khoảng màu đen.
     */
    getMinScale() {
        if (!this.imageLoaded || !this.mapImage.width || !this.mapImage.height) return 0.5;
        const scaleX = this.canvas.width / this.mapImage.width;
        const scaleY = this.canvas.height / this.mapImage.height;
        return Math.max(scaleX, scaleY);
    }

    /**
     * Giới hạn tọa độ dịch chuyển offsetX, offsetY và scale để bản đồ không bao giờ
     * bị kéo tuột ra ngoài làm hở khoảng đen ở các mép.
     */
    clampOffsets() {
        if (!this.imageLoaded || !this.mapImage.width || !this.mapImage.height) return;
        const minScale = this.getMinScale();
        this.minScale = minScale;
        if (this.scale < minScale) {
            this.scale = minScale;
        }

        const mapW = this.mapImage.width * this.scale;
        const mapH = this.mapImage.height * this.scale;

        // Khóa biên ngang: không cho mép ảnh hở ra khỏi canvas
        if (mapW >= this.canvas.width) {
            this.offsetX = Math.min(0, Math.max(this.canvas.width - mapW, this.offsetX));
        } else {
            this.offsetX = (this.canvas.width - mapW) / 2;
        }

        // Khóa biên dọc: không cho mép ảnh hở ra khỏi canvas
        if (mapH >= this.canvas.height) {
            this.offsetY = Math.min(0, Math.max(this.canvas.height - mapH, this.offsetY));
        } else {
            this.offsetY = (this.canvas.height - mapH) / 2;
        }
    }

    resizeCanvas() {
        const rect = this.canvas.parentElement.getBoundingClientRect();
        this.canvas.width = rect.width;
        this.canvas.height = rect.height;
        this.clampOffsets();
        this.render();
    }

    resetView() {
        if (!this.imageLoaded) return;
        // Tự động scale phủ kín khung nhìn canvas mà không lộ viền đen
        this.scale = this.getMinScale();
        this.offsetX = (this.canvas.width - this.mapImage.width * this.scale) / 2;
        this.offsetY = (this.canvas.height - this.mapImage.height * this.scale) / 2;
        this.clampOffsets();
        this.render();
    }

    zoom(delta, clientX, clientY) {
        const rect = this.canvas.getBoundingClientRect();
        const mouseX = clientX ? clientX - rect.left : this.canvas.width / 2;
        const mouseY = clientY ? clientY - rect.top : this.canvas.height / 2;

        const minScale = this.getMinScale();
        const newScale = Math.min(Math.max(this.scale * delta, minScale), this.maxScale);
        const factor = newScale / this.scale;

        this.offsetX = mouseX - (mouseX - this.offsetX) * factor;
        this.offsetY = mouseY - (mouseY - this.offsetY) * factor;
        this.scale = newScale;

        this.clampOffsets();
        this.render();
    }

    bindEvents() {
        window.addEventListener('resize', () => this.resizeCanvas());

        // Mouse Drag / Pan
        this.canvas.addEventListener('mousedown', (e) => {
            if (e.button === 0) { // Chuột trái
                this.isDragging = true;
                this.dragStartX = e.clientX - this.offsetX;
                this.dragStartY = e.clientY - this.offsetY;
            }
        });

        window.addEventListener('mousemove', (e) => {
            if (this.isDragging) {
                this.offsetX = e.clientX - this.dragStartX;
                this.offsetY = e.clientY - this.dragStartY;
                this.clampOffsets();
                this.render();
            }
        });

        window.addEventListener('mouseup', () => {
            this.isDragging = false;
        });

        // Mouse Wheel Zoom
        this.canvas.addEventListener('wheel', (e) => {
            e.preventDefault();
            const delta = e.deltaY < 0 ? 1.15 : 0.85;
            this.zoom(delta, e.clientX, e.clientY);
        }, { passive: false });

        // Touch events for Mobile/Tablet
        let initialDistance = 0;
        this.canvas.addEventListener('touchstart', (e) => {
            if (e.touches.length === 1) {
                this.isDragging = true;
                this.dragStartX = e.touches[0].clientX - this.offsetX;
                this.dragStartY = e.touches[0].clientY - this.offsetY;
            } else if (e.touches.length === 2) {
                this.isDragging = false;
                initialDistance = Math.hypot(
                    e.touches[0].clientX - e.touches[1].clientX,
                    e.touches[0].clientY - e.touches[1].clientY
                );
            }
        }, { passive: false });

        this.canvas.addEventListener('touchmove', (e) => {
            e.preventDefault();
            if (e.touches.length === 1 && this.isDragging) {
                this.offsetX = e.touches[0].clientX - this.dragStartX;
                this.offsetY = e.touches[0].clientY - this.dragStartY;
                this.render();
            } else if (e.touches.length === 2 && initialDistance > 0) {
                const dist = Math.hypot(
                    e.touches[0].clientX - e.touches[1].clientX,
                    e.touches[0].clientY - e.touches[1].clientY
                );
                const delta = dist / initialDistance;
                const midX = (e.touches[0].clientX + e.touches[1].clientX) / 2;
                const midY = (e.touches[0].clientY + e.touches[1].clientY) / 2;
                this.zoom(delta > 1 ? 1.05 : 0.95, midX, midY);
                initialDistance = dist;
            }
        }, { passive: false });

        this.canvas.addEventListener('touchend', () => {
            this.isDragging = false;
            initialDistance = 0;
        });
    }

    setRoute(path, entryNode = '', waypoints = []) {
        this.currentRoute = path || [];
        this.selectedEntryNode = entryNode;
        this.waypoints = waypoints || [];
        this.render();

        // Tự động căn giữa lộ trình nếu có ít nhất 2 nút
        if (this.currentRoute.length >= 2) {
            this.focusRoute();
        }
    }

    setBlockedEdges(blockedList) {
        this.blockedEdges = blockedList || [];
        this.render();
    }

    clearRoute() {
        this.currentRoute = [];
        this.selectedEntryNode = '';
        this.render();
    }

    focusRoute() {
        if (!this.currentRoute.length) return;
        
        let minX = Infinity, maxX = -Infinity, minY = Infinity, maxY = -Infinity;
        this.currentRoute.forEach(nodeId => {
            const pt = NODE_COORDINATES[nodeId];
            if (pt) {
                minX = Math.min(minX, pt.x);
                maxX = Math.max(maxX, pt.x);
                minY = Math.min(minY, pt.y);
                maxY = Math.max(maxY, pt.y);
            }
        });

        if (minX === Infinity) return;

        const pad = 120;
        const routeW = Math.max(maxX - minX + pad * 2, 300);
        const routeH = Math.max(maxY - minY + pad * 2, 300);

        const scaleX = this.canvas.width / routeW;
        const scaleY = this.canvas.height / routeH;
        this.scale = Math.min(scaleX, scaleY, 1.8);

        const centerX = (minX + maxX) / 2;
        const centerY = (minY + maxY) / 2;

        this.offsetX = this.canvas.width / 2 - centerX * this.scale;
        this.offsetY = this.canvas.height / 2 - centerY * this.scale;

        this.render();
    }

    startAnimation() {
        const animate = () => {
            this.dashOffset = (this.dashOffset - 0.75) % 24;
            this.pulsePhase = (this.pulsePhase + 0.05) % (Math.PI * 2);
            this.render();
            this.animationFrameId = requestAnimationFrame(animate);
        };
        this.animationFrameId = requestAnimationFrame(animate);
    }

    render() {
        const ctx = this.ctx;
        ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);

        ctx.save();
        ctx.translate(this.offsetX, this.offsetY);
        ctx.scale(this.scale, this.scale);

        // 1. Vẽ ảnh nền bản đồ
        if (this.imageLoaded) {
            ctx.drawImage(this.mapImage, 0, 0);
        }

        // 2. Vẽ các cạnh bị chặn (nếu có)
        this.drawBlockedEdges(ctx);

        // 3. Vẽ đường đi tối ưu phát sáng
        this.drawRoute(ctx);

        ctx.restore();
    }

    drawBlockedEdges(ctx) {
        if (!this.blockedEdges || !this.blockedEdges.length) return;

        ctx.save();
        const drawSegment = (u, v) => {
            const p1 = NODE_COORDINATES[u];
            const p2 = NODE_COORDINATES[v];
            if (!p1 || !p2) return;

            // Vẽ vệt đỏ cảnh báo
            ctx.beginPath();
            ctx.moveTo(p1.x, p1.y);
            ctx.lineTo(p2.x, p2.y);
            ctx.strokeStyle = 'rgba(239, 68, 68, 0.7)';
            ctx.lineWidth = 6;
            ctx.setLineDash([8, 6]);
            ctx.stroke();

            // Biểu tượng chữ X ở giữa đoạn
            const midX = (p1.x + p2.x) / 2;
            const midY = (p1.y + p2.y) / 2;
            ctx.fillStyle = '#ef4444';
            ctx.beginPath();
            ctx.arc(midX, midY, 10, 0, Math.PI * 2);
            ctx.fill();

            ctx.strokeStyle = '#ffffff';
            ctx.lineWidth = 2.5;
            ctx.setLineDash([]);
            ctx.beginPath();
            ctx.moveTo(midX - 5, midY - 5);
            ctx.lineTo(midX + 5, midY + 5);
            ctx.moveTo(midX + 5, midY - 5);
            ctx.lineTo(midX - 5, midY + 5);
            ctx.stroke();
        };

        this.blockedEdges.forEach(edgeStr => {
            if (typeof edgeStr !== 'string' || !edgeStr.includes('-')) return;
            const parts = edgeStr.split('-').map(s => s.trim());
            if (parts.length === 2) {
                const u = parts[0], v = parts[1];
                if ((u === 'Q' && v === 'R2') || (u === 'R2' && v === 'Q')) {
                    drawSegment('Q', 'DN_NE');
                    drawSegment('DN_NE', 'R2');
                } else if ((u === 'H2' && v === 'H3') || (u === 'H3' && v === 'H2')) {
                    drawSegment('H2', 'DN_SE');
                    drawSegment('DN_SE', 'H3');
                } else {
                    drawSegment(u, v);
                }
            }
        });
        ctx.restore();
    }

    /**
     * Nội suy danh sách điểm ảnh để mọi đoạn di chuyển đều bám theo tim đường ô bàn cờ vuông góc 90 độ
     */
    getOrthogonalPathPoints(route) {
        if (!route || !route.length) return [];
        const rawPts = route.map(id => NODE_COORDINATES[id]).filter(Boolean);
        if (rawPts.length <= 1) return rawPts;

        const pts = [rawPts[0]];
        for (let i = 0; i < rawPts.length - 1; i++) {
            const p1 = rawPts[i];
            const p2 = rawPts[i + 1];

            // Nếu cùng X hoặc cùng Y thì đã là đoạn thẳng vuông góc
            if (p1.x === p2.x || p1.y === p2.y) {
                pts.push(p2);
            } else {
                // Hai đỉnh chéo nhau: chèn điểm góc vuông theo trục tim đường chính
                const candidate1 = { x: p2.x, y: p1.y };
                const candidate2 = { x: p1.x, y: p2.y };
                const p3 = rawPts[i + 2];

                const MAIN_X = [200, 405, 410, 430, 540, 575, 580, 615, 630, 665, 705, 715, 755, 780, 820, 920, 940, 1040, 1060, 1225];
                const MAIN_Y = [55, 70, 85, 110, 160, 315, 342, 440, 492, 560, 570, 600, 630, 710, 745, 840];

                const p1_is_main_x = MAIN_X.includes(p1.x);
                const p2_is_main_x = MAIN_X.includes(p2.x);
                const p1_is_main_y = MAIN_Y.includes(p1.y);
                const p2_is_main_y = MAIN_Y.includes(p2.y);

                let chosen = null;

                // Ưu tiên 1: Nút đón nằm lệch trục (như C1 ở x=890) khi nối ra/vào trục tim đường chính (x=920):
                // Luôn bẻ vuông góc trực tiếp ra trục chính tim đường, tuyệt đối không chạy dọc theo vỉa hè lệch trục
                if (p2_is_main_x && !p1_is_main_x) {
                    chosen = candidate1; // ra trục X chính tại y = p1.y
                } else if (p1_is_main_x && !p2_is_main_x) {
                    chosen = candidate2; // bám trục X chính tới y = p2.y rồi rẽ vào
                } else if (p2_is_main_y && !p1_is_main_y) {
                    chosen = candidate2; // ra trục Y chính tại x = p1.x
                } else if (p1_is_main_y && !p2_is_main_y) {
                    chosen = candidate1; // bám trục Y chính tới x = p2.x rồi rẽ vào
                } else if (p3) {
                    // Kiểm tra xem candidate nào gây ra hiện tượng quay đầu ngược trên cùng một trục (collinear backtrack)
                    const dx1 = p2.x - p1.x;
                    const dx2 = p3.x - p2.x;
                    const backtrack2 = (p2.y === p3.y && dx1 * dx2 < 0);

                    const dy1 = p2.y - p1.y;
                    const dy2 = p3.y - p2.y;
                    const backtrack1 = (p2.x === p3.x && dy1 * dy2 < 0);

                    if (backtrack2 && !backtrack1) {
                        chosen = candidate1;
                    } else if (backtrack1 && !backtrack2) {
                        chosen = candidate2;
                    }
                }

                if (!chosen) {
                    if (p1_is_main_x) {
                        chosen = candidate2;
                    } else if (p1_is_main_y) {
                        chosen = candidate1;
                    } else if (p2_is_main_x) {
                        chosen = candidate1;
                    } else {
                        chosen = candidate2;
                    }
                }
                pts.push(chosen);
                pts.push(p2);
            }
        }

        // Rút gọn các điểm trùng nhau và các điểm thẳng hàng liên tiếp cùng hướng (bảo toàn điểm quay đầu rẽ nhánh)
        const simplified = [];
        for (let i = 0; i < pts.length; i++) {
            const cur = pts[i];
            if (simplified.length === 0) {
                simplified.push(cur);
                continue;
            }
            const prev = simplified[simplified.length - 1];
            if (prev.x === cur.x && prev.y === cur.y) {
                continue; // Bỏ điểm trùng lặp
            }
            if (simplified.length >= 2) {
                const prevPrev = simplified[simplified.length - 2];
                // Chỉ gộp khi 3 điểm liên tiếp thẳng hàng và tiếp tục cùng chiều di chuyển (không quay đầu ngược)
                const sameHorizontal = (prevPrev.y === prev.y && prev.y === cur.y && (prev.x - prevPrev.x) * (cur.x - prev.x) > 0);
                const sameVertical = (prevPrev.x === prev.x && prev.x === cur.x && (prev.y - prevPrev.y) * (cur.y - prev.y) > 0);
                if (sameHorizontal || sameVertical) {
                    simplified[simplified.length - 1] = cur; // Kéo dài đoạn thẳng, bỏ điểm giữa
                    continue;
                }
            }
            simplified.push(cur);
        }

        return simplified;
    }

    drawRoute(ctx) {
        if (!this.currentRoute || this.currentRoute.length < 2) return;

        const pts = this.getOrthogonalPathPoints(this.currentRoute);
        if (!pts || pts.length < 2) return;

        ctx.save();

        // --- Lớp 1: Phát sáng đỏ nổi bật (Red Glow) bên ngoài ---
        ctx.beginPath();
        for (let i = 0; i < pts.length; i++) {
            const pt = pts[i];
            if (i === 0) ctx.moveTo(pt.x, pt.y);
            else ctx.lineTo(pt.x, pt.y);
        }
        ctx.strokeStyle = 'rgba(255, 0, 51, 0.4)';
        ctx.lineWidth = 14;
        ctx.lineCap = 'round';
        ctx.lineJoin = 'round';
        ctx.shadowColor = 'rgba(255, 0, 51, 0.8)';
        ctx.shadowBlur = 16;
        ctx.stroke();

        // --- Lớp 2: Đường chỉ đỏ rực rỡ sắc nét (Red Base Line 7px) ---
        ctx.beginPath();
        for (let i = 0; i < pts.length; i++) {
            const pt = pts[i];
            if (i === 0) ctx.moveTo(pt.x, pt.y);
            else ctx.lineTo(pt.x, pt.y);
        }
        ctx.strokeStyle = '#FF0033';
        ctx.lineWidth = 7.0;
        ctx.lineCap = 'round';
        ctx.lineJoin = 'round';
        ctx.shadowColor = 'rgba(255, 0, 51, 0.7)';
        ctx.shadowBlur = 12;
        ctx.stroke();

        // --- Lớp 3: Luồng vệt sáng nét đứt chuyển động liên tục (Animated White Dashed Stream) ---
        ctx.beginPath();
        for (let i = 0; i < pts.length; i++) {
            const pt = pts[i];
            if (i === 0) ctx.moveTo(pt.x, pt.y);
            else ctx.lineTo(pt.x, pt.y);
        }
        ctx.strokeStyle = '#ffffff';
        ctx.lineWidth = 3.2;
        ctx.lineCap = 'round';
        ctx.lineJoin = 'round';
        ctx.setLineDash([14, 12]);
        ctx.lineDashOffset = this.dashOffset;
        ctx.shadowColor = '#ffffff';
        ctx.shadowBlur = 6;
        ctx.stroke();

        // --- Lớp 4: Mũi tên định hướng di chuyển (Directional Arrowheads) trôi dọc lộ trình ---
        ctx.setLineDash([]);
        ctx.shadowBlur = 0;
        const arrowSpacing = 42; // Khoảng cách giữa các mũi tên
        const arrowProgress = (Math.abs(this.dashOffset) * 1.8) % arrowSpacing;

        for (let i = 0; i < pts.length - 1; i++) {
            const p1 = pts[i];
            const p2 = pts[i + 1];
            if (!p1 || !p2) continue;

            const dx = p2.x - p1.x;
            const dy = p2.y - p1.y;
            const segDist = Math.hypot(dx, dy);
            if (segDist < 18) continue;

            const angle = Math.atan2(dy, dx);
            const cos = Math.cos(angle);
            const sin = Math.sin(angle);

            // Bố trí mũi tên chạy từ p1 sang p2
            for (let dist = (arrowSpacing - arrowProgress) % arrowSpacing; dist < segDist - 6; dist += arrowSpacing) {
                if (dist < 6) continue;
                const ax = p1.x + cos * dist;
                const ay = p1.y + sin * dist;

                const arrowSize = 5.0;
                ctx.beginPath();
                ctx.moveTo(ax - arrowSize * cos + arrowSize * 0.7 * sin, ay - arrowSize * sin - arrowSize * 0.7 * cos);
                ctx.lineTo(ax, ay);
                ctx.lineTo(ax - arrowSize * cos - arrowSize * 0.7 * sin, ay - arrowSize * sin + arrowSize * 0.7 * cos);
                ctx.strokeStyle = '#ffffff';
                ctx.lineWidth = 2.4;
                ctx.lineCap = 'round';
                ctx.lineJoin = 'round';
                ctx.stroke();
            }
        }

        ctx.restore();

        // --- Vẽ Marker các nút trung gian (Chấm tròn đỏ viền trắng) ---
        for (let i = 1; i < this.currentRoute.length - 1; i++) {
            const nodeId = this.currentRoute[i];
            const pt = NODE_COORDINATES[nodeId];
            if (pt) {
                const isWp = this.waypoints && this.waypoints.slice(1).includes(nodeId);
                if (!isWp) {
                    ctx.beginPath();
                    ctx.arc(pt.x, pt.y, 5.0, 0, Math.PI * 2);
                    ctx.fillStyle = '#FF0033';
                    ctx.shadowColor = 'rgba(255, 0, 51, 0.8)';
                    ctx.shadowBlur = 8;
                    ctx.fill();
                    ctx.strokeStyle = '#ffffff';
                    ctx.lineWidth = 2.0;
                    ctx.stroke();
                    ctx.shadowBlur = 0;
                }
            }
        }

        // --- Vẽ Huy hiệu số thứ tự (1, 2, 3...) cho từng trạm dừng đa điểm ---
        if (this.waypoints && this.waypoints.length > 1) {
            for (let wIdx = 1; wIdx < this.waypoints.length; wIdx++) {
                const wpNodeId = this.waypoints[wIdx];
                const pt = NODE_COORDINATES[wpNodeId];
                if (pt) {
                    ctx.save();
                    ctx.beginPath();
                    ctx.arc(pt.x, pt.y, 10.0, 0, Math.PI * 2);
                    ctx.fillStyle = '#f59e0b';
                    ctx.shadowColor = '#f59e0b';
                    ctx.shadowBlur = 10;
                    ctx.fill();
                    ctx.strokeStyle = '#ffffff';
                    ctx.lineWidth = 2.5;
                    ctx.stroke();

                    // Số thứ tự trạm: 1, 2, 3...
                    ctx.font = 'bold 11px Segoe UI, sans-serif';
                    ctx.fillStyle = '#ffffff';
                    ctx.textAlign = 'center';
                    ctx.textBaseline = 'middle';
                    ctx.fillText(String(wIdx), pt.x, pt.y);
                    ctx.restore();
                }
            }
        }

        // --- Vẽ Marker Điểm Xuất Phát (Màu xanh lá) ---
        const startPt = NODE_COORDINATES[this.currentRoute[0]];
        if (startPt) {
            ctx.save();
            ctx.beginPath();
            ctx.arc(startPt.x, startPt.y, 8, 0, Math.PI * 2);
            ctx.fillStyle = '#10b981';
            ctx.shadowColor = '#10b981';
            ctx.shadowBlur = 12;
            ctx.fill();
            ctx.strokeStyle = '#ffffff';
            ctx.lineWidth = 2.5;
            ctx.stroke();
            ctx.restore();
        }

        // --- Vẽ Marker Điểm Đến (Màu đỏ nhấp nháy phát sáng) ---
        const endNodeId = this.currentRoute[this.currentRoute.length - 1];
        const endPt = NODE_COORDINATES[endNodeId];
        if (endPt) {
            ctx.save();
            const pulse = (Math.sin(this.pulsePhase) + 1) / 2; // Dao động 0 -> 1
            const pulseRadius = 10 + pulse * 14;
            const pulseAlpha = 0.8 - pulse * 0.7;

            // Vòng tròn sóng lan tỏa
            ctx.beginPath();
            ctx.arc(endPt.x, endPt.y, pulseRadius, 0, Math.PI * 2);
            ctx.strokeStyle = `rgba(255, 0, 51, ${pulseAlpha})`;
            ctx.lineWidth = 2.5;
            ctx.stroke();

            // Chấm tâm màu đỏ phát sáng
            ctx.beginPath();
            ctx.arc(endPt.x, endPt.y, 9, 0, Math.PI * 2);
            ctx.fillStyle = '#FF0033';
            ctx.shadowColor = '#FF0033';
            ctx.shadowBlur = 16;
            ctx.fill();
            ctx.strokeStyle = '#ffffff';
            ctx.lineWidth = 2.5;
            ctx.stroke();

            // Nhãn tên điểm đến
            ctx.font = 'bold 12px Segoe UI, sans-serif';
            ctx.fillStyle = '#ffffff';
            ctx.shadowColor = '#000000';
            ctx.shadowBlur = 4;
            ctx.fillText(endPt.name, endPt.x + 14, endPt.y + 4);

            ctx.restore();
        }
    }
}

