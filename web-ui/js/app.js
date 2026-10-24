/**
 * app.js - Xử lý logic giao diện UTC2 Campus Navigation (Standalone Mode)
 *
 * Toàn bộ thuật toán chạy trực tiếp trên JavaScript (graphEngine.js):
 *   - Tìm đường đơn điểm (Dijkstra)
 *   - Tìm đường đa điểm (Multi-stop Routing)
 *   - Kiểm tra thời khóa biểu & Cảnh báo trễ học (Schedule Optimizer)
 *   - So sánh hiệu năng giải thuật (Benchmark Dijkstra vs A* vs BFS)
 *   - Hướng dẫn rẽ từng bước kèm icon mũi tên (Turn-by-turn)
 *
 * Không phụ thuộc Backend Server — hoạt động khi mở bằng Live Server hoặc file://
 */

// Danh sách các địa điểm công trình sạch hiển thị trên dropdown (không tiền tố, không nút giao)
const MAIN_DESTINATIONS = [
    { id: 'C1',   name: 'Nhà C1',              group: 'Khu C & Giảng đường' },
    { id: 'C2',   name: 'Nhà C2',              group: 'Khu C & Giảng đường' },
    { id: 'V4_S', name: 'Nhà C3',              group: 'Khu C & Giảng đường' },
    { id: 'DN',   name: 'Giảng đường Đa Năng', group: 'Khu C & Giảng đường' },
    { id: 'E1_Mid', name: 'Nhà E1',              group: 'Khu E' },
    { id: 'E3',   name: 'Nhà E3',              group: 'Khu E' },
    { id: 'E4',   name: 'Nhà E4',              group: 'Khu E' },
    { id: 'E5',   name: 'Nhà E5',              group: 'Khu E' },
    { id: 'E6_W', name: 'Nhà E6',              group: 'Khu E' },
    { id: 'E7',   name: 'Nhà E7',              group: 'Khu E' },
    { id: 'E8',   name: 'Nhà E8',              group: 'Khu E' },
    { id: 'D0',   name: 'Nhà E9',              group: 'Khu E' },
    { id: 'E10',  name: 'Nhà E10',             group: 'Khu E' },
    { id: 'G',    name: 'Cổng số 451',         group: 'Cổng & Tiện ích' },
    { id: 'CT',   name: 'Căn tin sinh viên',   group: 'Cổng & Tiện ích' },
    { id: 'PZ',   name: 'Nhà để xe',           group: 'Cổng & Tiện ích' },
];

document.addEventListener('DOMContentLoaded', () => {
    // 1. Khởi tạo Canvas Map
    const canvasMap = new CanvasMap('mapCanvas');
    window.canvasMap = canvasMap;
    window.drawRoute = (path) => canvasMap.setRoute(path);

    // 2. Tham chiếu DOM elements
    const startSelect        = document.getElementById('startSelect');
    const endSelect          = document.getElementById('endSelect');
    const btnFindRoute       = document.getElementById('btnFindRoute');
    const btnReset           = document.getElementById('btnReset');
    const resultCard         = document.getElementById('resultCard');
    const statDistance       = document.getElementById('statDistance');
    const statTime           = document.getElementById('statTime');
    const doorBadge          = document.getElementById('doorBadge');
    const directionsList     = document.getElementById('directionsList');
    const statusDot          = document.getElementById('statusDot');
    const statusText         = document.getElementById('statusText');

    // Multi-stop elements
    const multiStartSelect   = document.getElementById('multiStartSelect');
    const chipGrid           = document.getElementById('chipGrid');
    const addStopSelect      = document.getElementById('addStopSelect');
    const btnAddCustomStop   = document.getElementById('btnAddCustomStop');
    const selectedStopsList  = document.getElementById('selectedStopsList');
    const btnClearStops      = document.getElementById('btnClearStops');
    const btnFindMultiStop   = document.getElementById('btnFindMultiStop');
    const multiStopBadge     = document.getElementById('multiStopBadge');
    const multiStopAlgo      = document.getElementById('multiStopAlgo');
    const multiStopOrder     = document.getElementById('multiStopOrder');

    // Schedule Optimizer elements
    const scheduleRoom1      = document.getElementById('scheduleRoom1');
    const scheduleRoom2      = document.getElementById('scheduleRoom2');
    const scheduleBreakTime  = document.getElementById('scheduleBreakTime');
    const btnCheckSchedule   = document.getElementById('btnCheckSchedule');
    const scheduleAlertBadge = document.getElementById('scheduleAlertBadge');
    const scheduleAlertIcon  = document.getElementById('scheduleAlertIcon');
    const scheduleAlertStatus = document.getElementById('scheduleAlertStatus');
    const scheduleAlertMessage = document.getElementById('scheduleAlertMessage');

    // Benchmark elements
    const btnOpenBenchmark   = document.getElementById('btnOpenBenchmark');
    const benchmarkModal     = document.getElementById('benchmarkModal');
    const btnCloseBenchmark  = document.getElementById('btnCloseBenchmark');
    const benchStart         = document.getElementById('benchStart');
    const benchEnd           = document.getElementById('benchEnd');
    const btnRunBenchmark    = document.getElementById('btnRunBenchmark');
    const benchmarkBody      = document.getElementById('benchmarkBody');
    const benchmarkInsight   = document.getElementById('benchmarkInsight');
    const benchmarkLoading   = document.getElementById('benchmarkLoading');
    const benchmarkTable     = document.getElementById('benchmarkTable');

    // Auth & User Profile elements
    const userProfileBadge   = document.getElementById('userProfileBadge');
    const headerUserName     = document.getElementById('headerUserName');
    const headerUserMssv     = document.getElementById('headerUserMssv');
    const btnLogout          = document.getElementById('btnLogout');
    const authModal          = document.getElementById('authModal');
    const tabBtnLogin        = document.getElementById('tabBtnLogin');
    const tabBtnRegister     = document.getElementById('tabBtnRegister');
    const formLogin          = document.getElementById('formLogin');
    const formRegister       = document.getElementById('formRegister');
    const authAlert          = document.getElementById('authAlert');
    const loginMssv          = document.getElementById('loginMssv');
    const loginPassword      = document.getElementById('loginPassword');
    const regFullName        = document.getElementById('regFullName');
    const regMssv            = document.getElementById('regMssv');
    const regClass           = document.getElementById('regClass');
    const regPassword        = document.getElementById('regPassword');
    const regPasswordConfirm = document.getElementById('regPasswordConfirm');

    // Toolbar buttons
    document.getElementById('btnZoomIn').addEventListener('click', () => canvasMap.zoom(1.2));
    document.getElementById('btnZoomOut').addEventListener('click', () => canvasMap.zoom(0.8));
    document.getElementById('btnResetView').addEventListener('click', () => canvasMap.resetView());
    document.getElementById('btnFitRoute').addEventListener('click', () => canvasMap.focusRoute());

    // State danh sách điểm ghé Multi-stop
    let selectedStops = [];

    // 3. Khởi tạo giao diện
    initAuth();
    initTabs();
    initDropdowns();
    initMultiStopChips();
    initBenchmarkModal();

    // 4. Hiển thị trạng thái "Online" ngay lập tức (engine tích hợp sẵn)
    setEngineOnline();

    // 5. Gắn sự kiện
    btnFindRoute.addEventListener('click', handleFindRoute);
    btnFindMultiStop.addEventListener('click', handleFindMultiStop);
    btnCheckSchedule.addEventListener('click', handleCheckSchedule);

    btnReset.addEventListener('click', () => {
        startSelect.value = 'G';
        endSelect.value = 'K';
        document.querySelectorAll('.blocked-checkbox').forEach(cb => cb.checked = false);
        canvasMap.setBlockedEdges([]);
        canvasMap.clearRoute();
        resultCard.classList.remove('active');
        scheduleAlertBadge.style.display = 'none';
        multiStopBadge.style.display = 'none';
        doorBadge.style.display = 'none';
        directionsList.innerHTML = '';
    });

    document.querySelectorAll('.blocked-checkbox').forEach(cb => {
        cb.addEventListener('change', updateBlockedEdgesOnMap);
    });

    // =========================================================================
    // Trạng thái Online (không cần server)
    // =========================================================================
    function setEngineOnline() {
        statusDot.classList.add('online');
        statusText.textContent = 'Standalone';

        // Tu dong kiem tra neu dang ket noi toi C++ Backend Server
        fetch('/api/health')
            .then(res => res.json())
            .then(data => {
                if (data && data.status === 'ok') {
                    statusText.textContent = 'C++ Server Online';
                }
            })
            .catch(() => {
                // Che do offline / Standalone (neu mo truc tiep bang file://)
            });
    }

    // =========================================================================
    // Quản lý Tabs
    // =========================================================================
    function initTabs() {
        const tabs = document.querySelectorAll('.nav-tab');
        const panes = document.querySelectorAll('.tab-pane');
        tabs.forEach(tab => {
            tab.addEventListener('click', () => {
                const targetId = tab.getAttribute('data-tab');
                tabs.forEach(t => t.classList.remove('active'));
                panes.forEach(p => p.classList.remove('active'));
                tab.classList.add('active');
                const pane = document.getElementById(targetId);
                if (pane) pane.classList.add('active');
                scheduleAlertBadge.style.display = 'none';
                multiStopBadge.style.display = 'none';
            });
        });
    }

    // =========================================================================
    // Khởi tạo Dropdown
    // =========================================================================
    function initDropdowns() {
        // Danh sách công trình sạch cho điểm xuất phát
        const origins = [
            { v: 'G',    t: 'Cổng số 451' },
            { v: 'C1',   t: 'Nhà C1' },
            { v: 'C2',   t: 'Nhà C2' },
            { v: 'V4_S', t: 'Nhà C3' },
            { v: 'DN',   t: 'Giảng đường Đa Năng' },
            { v: 'E1',   t: 'Nhà E1' },
            { v: 'E3',   t: 'Nhà E3' },
            { v: 'E4',   t: 'Nhà E4' },
            { v: 'E5',   t: 'Nhà E5' },
            { v: 'E6_W', t: 'Nhà E6' },
            { v: 'E7',   t: 'Nhà E7' },
            { v: 'E8',   t: 'Nhà E8' },
            { v: 'D0',   t: 'Nhà E9' },
            { v: 'E10',  t: 'Nhà E10' },
            { v: 'CT',   t: 'Căn tin sinh viên' },
            { v: 'PZ',   t: 'Nhà để xe' },
        ];

        startSelect.innerHTML = '';
        multiStartSelect.innerHTML = '';
        for (const { v, t } of origins) {
            startSelect.add(new Option(t, v, v === 'G', v === 'G'));
            multiStartSelect.add(new Option(t, v, v === 'G', v === 'G'));
        }

        // Điểm đến (endSelect) theo nhóm công trình sạch
        endSelect.innerHTML = '';
        addStopSelect.innerHTML = '';
        const groups = {};
        MAIN_DESTINATIONS.forEach(item => {
            if (!groups[item.group]) groups[item.group] = [];
            groups[item.group].push(item);
        });

        for (const [groupName, items] of Object.entries(groups)) {
            const og = document.createElement('optgroup');
            og.label = groupName;
            items.forEach(it => {
                const opt = new Option(it.name, it.id, it.id === 'C2', it.id === 'C2');
                og.appendChild(opt);
                addStopSelect.add(new Option(it.name, it.id));
            });
            endSelect.appendChild(og);
        }
    }

    // =========================================================================
    // Cạnh bị chặn
    // =========================================================================
    function getBlockedEdges() {
        const blockedCheckboxes = document.querySelectorAll('input[type="checkbox"]:checked');
        const blockedEdges = [];

        blockedCheckboxes.forEach(cb => {
            const val = cb.value || (cb.dataset && cb.dataset.edge) || '';
            if (typeof val === 'string' && val.includes('-')) {
                const parts = val.split('-').map(s => s.trim());
                if (parts.length >= 2 && parts[0] && parts[1]) {
                    blockedEdges.push(`${parts[0]}-${parts[1]}`);
                }
            }
        });
        return blockedEdges;
    }

    function updateBlockedEdgesOnMap() {
        canvasMap.setBlockedEdges(getBlockedEdges());
    }

    // =========================================================================
    // Tìm đường đơn điểm (Dijkstra, engine nội bộ)
    // =========================================================================
    function handleFindRoute() {
        const start   = startSelect.value;
        const end     = endSelect.value;
        const blocked = getBlockedEdges().join(',');

        btnFindRoute.disabled = true;
        btnFindRoute.innerHTML = '<span>⏳ Đang tìm...</span>';
        scheduleAlertBadge.style.display = 'none';
        multiStopBadge.style.display = 'none';
        updateBlockedEdgesOnMap();

        // Chạy đồng bộ (engine thuần JS, tức thì)
        try {
            const data = GraphEngine.route(start, end, blocked);
            console.log('Tìm đường từ', start, 'đến', end, 'Kết quả path:', data ? data.path : null);
            if (!data || !data.path || data.path.length === 0) {
                alert(`Không tìm thấy đường đi từ "${start}" đến "${end}"! Hãy kiểm tra lại liên kết cạnh.`);
                return;
            }
            displayResults(data);
            canvasMap.setRoute(data.path, data.entry_node);
        } catch (err) {
            console.error('Lỗi tìm đường:', err);
            alert(err.message || `Không tìm thấy đường đi từ "${start}" đến "${end}"! Hãy kiểm tra lại liên kết cạnh.`);
        } finally {
            btnFindRoute.disabled = false;
            btnFindRoute.innerHTML = '<span>🔍 Tìm đường tối ưu</span>';
        }
    }

    // =========================================================================
    // Multi-stop Chips & Selection
    // =========================================================================
    function initMultiStopChips() {
        chipGrid.querySelectorAll('.stop-chip').forEach(chip => {
            chip.addEventListener('click', () => {
                const id = chip.getAttribute('data-id');
                const text = chip.textContent.trim();
                toggleStopSelection(id, text, chip);
            });
        });

        btnAddCustomStop.addEventListener('click', () => {
            const id   = addStopSelect.value;
            const text = addStopSelect.options[addStopSelect.selectedIndex].text;
            addStopToSelection(id, text);
        });

        btnClearStops.addEventListener('click', () => {
            selectedStops = [];
            chipGrid.querySelectorAll('.stop-chip').forEach(c => c.classList.remove('selected'));
            renderSelectedStops();
        });
    }

    function toggleStopSelection(id, name, chipEl) {
        const idx = selectedStops.findIndex(s => s.id === id);
        if (idx !== -1) {
            selectedStops.splice(idx, 1);
            if (chipEl) chipEl.classList.remove('selected');
        } else {
            selectedStops.push({ id, name });
            if (chipEl) chipEl.classList.add('selected');
        }
        renderSelectedStops();
    }

    function addStopToSelection(id, name) {
        if (!selectedStops.some(s => s.id === id)) {
            selectedStops.push({ id, name });
            const chip = chipGrid.querySelector(`.stop-chip[data-id="${id}"]`);
            if (chip) chip.classList.add('selected');
            renderSelectedStops();
        }
    }

    function renderSelectedStops() {
        selectedStopsList.innerHTML = '';
        if (selectedStops.length === 0) {
            selectedStopsList.innerHTML = '<div style="color:var(--text-muted);font-size:0.8rem;padding:6px 0">Chưa chọn điểm ghé nào. Vui lòng bấm vào các thẻ phía trên.</div>';
            return;
        }
        selectedStops.forEach((stop, index) => {
            const item = document.createElement('div');
            item.className = 'selected-stop-item';
            item.innerHTML = `
                <div>
                    <span class="stop-order">${index + 1}</span>
                    <span><strong>${stop.name}</strong></span>
                </div>
                <button type="button" class="btn-remove-stop" data-id="${stop.id}">&times;</button>
            `;
            item.querySelector('.btn-remove-stop').addEventListener('click', () => {
                selectedStops = selectedStops.filter(s => s.id !== stop.id);
                const chip = chipGrid.querySelector(`.stop-chip[data-id="${stop.id}"]`);
                if (chip) chip.classList.remove('selected');
                renderSelectedStops();
            });
            selectedStopsList.appendChild(item);
        });
    }

    // =========================================================================
    // Đa điểm (Multi-stop, engine nội bộ)
    // =========================================================================
    function handleFindMultiStop() {
        const startNodeId = multiStartSelect.value;
        const multiStopList = selectedStops;
        if (!multiStopList || multiStopList.length === 0) {
            alert('Vui lòng chọn ít nhất 1 điểm cần ghé thăm!');
            return;
        }

        // Lấy đúng mảng node được người dùng add vào danh sách trên UI:
        const selectedStopIds = multiStopList.map(item => item.id);
        const blocked = getBlockedEdges().join(',');

        btnFindMultiStop.disabled = true;
        btnFindMultiStop.innerHTML = '<span>⏳ Đang tối ưu...</span>';
        scheduleAlertBadge.style.display = 'none';
        updateBlockedEdgesOnMap();

        try {
            // Chạy thuật toán xuất phát từ startNodeId qua toàn bộ selectedStopIds
            const data = GraphEngine.solveTSP(startNodeId, selectedStopIds, blocked);

            // Cập nhật đầy đủ danh sách trạm và chặng vào box Kết quả chỉ đường
            displayMultiStopResults(data);

            // Truyền duy nhất một mảng fullPath cho canvas vẽ lộ trình đơn tuyến liên tục
            if (window.canvasMap && data.full_path) {
                canvasMap.setRoute(data.full_path, '', data.ordered_stops);
            }
        } catch (err) {
            alert(err.message || 'Lỗi khi tối ưu lộ trình đa điểm!');
        } finally {
            btnFindMultiStop.disabled = false;
            btnFindMultiStop.innerHTML = '<span>🚀 Tối ưu lộ trình đa điểm</span>';
        }
    }

    // =========================================================================
    // Thời khóa biểu (Schedule Check, engine nội bộ)
    // =========================================================================
    function handleCheckSchedule() {
        const room1     = scheduleRoom1.value;
        const room2     = scheduleRoom2.value;
        const breakTime = parseInt(scheduleBreakTime.value, 10) || 300;
        const blocked   = getBlockedEdges().join(',');

        btnCheckSchedule.disabled = true;
        btnCheckSchedule.innerHTML = '<span>⏳ Đang kiểm tra...</span>';
        multiStopBadge.style.display = 'none';
        updateBlockedEdgesOnMap();

        try {
            const data = GraphEngine.scheduleCheck(room1, room2, breakTime, blocked);
            displayResults(data);

            scheduleAlertBadge.style.display = 'flex';
            scheduleAlertBadge.className = `schedule-alert-box ${data.badge_class}`;
            scheduleAlertIcon.textContent = data.is_at_risk ? '🚨' : '✅';
            scheduleAlertStatus.textContent = data.status.toUpperCase();
            scheduleAlertMessage.textContent = data.message;

            canvasMap.setRoute(data.path);
        } catch (err) {
            alert(err.message || 'Lỗi khi kiểm tra thời khóa biểu!');
        } finally {
            btnCheckSchedule.disabled = false;
            btnCheckSchedule.innerHTML = '<span>⚡ Kiểm tra kịp giờ học</span>';
        }
    }

    // =========================================================================
    // Hiển thị kết quả & Turn-by-turn
    // =========================================================================
    function displayMultiStopResults(data) {
        resultCard.classList.add('active');

        const dist = data.total_distance || 0;
        statDistance.textContent = `${Math.round(dist * 10) / 10} m`;

        const totalSec = data.walking_time_sec || Math.round(dist / 1.2);
        const mins = Math.floor(totalSec / 60);
        const secs = totalSec % 60;
        statTime.textContent = mins > 0 ? `${mins}p ${secs}s` : `${secs}s`;

        doorBadge.style.display = 'none';

        // Badge trạng thái đa điểm phía trên
        multiStopBadge.style.display = 'flex';
        multiStopAlgo.textContent = data.algorithm_used || 'Tối ưu lộ trình đa điểm (TSP)';
        multiStopOrder.innerHTML = `Thứ tự tối ưu: <strong>${data.ordered_stop_names.join(' &rarr; ')}</strong>`;

        // Làm sạch danh sách chỉ đường để hiển thị kết quả đa điểm
        directionsList.innerHTML = '';

        // Box giải thích tổng quan lộ trình
        const summaryCard = document.createElement('div');
        summaryCard.className = 'turn-step-item';
        summaryCard.style.cssText = 'background:rgba(59, 130, 246, 0.08);border:1px solid #3b82f6;border-radius:8px;padding:10px;margin-bottom:10px;';
        summaryCard.innerHTML = `
            <div class="turn-icon-badge" style="background:#3b82f6;color:#fff">🚀</div>
            <div class="turn-content">
                <div style="font-weight:700;color:var(--text-primary);font-size:0.85rem;margin-bottom:4px">
                    Lộ trình tối ưu (${data.ordered_stops.length} điểm ghé):
                </div>
                <div style="font-size:0.85rem;color:#2563eb;font-weight:600;line-height:1.5">
                    ${data.ordered_stop_names.join(' &rarr; ')}
                </div>
                <div style="font-size:0.75rem;color:var(--text-muted);margin-top:6px">
                    Thuật toán: <strong>${data.algorithm_used}</strong> &bull; Tổng cự ly: <strong>${Math.round(dist * 10) / 10} m</strong> &bull; Thời gian ước tính: <strong>${mins > 0 ? `${mins}p ${secs}s` : `${secs}s`}</strong>
                </div>
            </div>
        `;
        directionsList.appendChild(summaryCard);

        // Hiển thị chi tiết từng chặng (Legs)
        if (data.legs && data.legs.length > 0) {
            data.legs.forEach(leg => {
                const legHeader = document.createElement('div');
                legHeader.style.cssText = 'margin-top:12px;margin-bottom:6px;padding:7px 10px;background:var(--bg-secondary);border-radius:6px;font-size:0.8rem;font-weight:700;color:var(--text-primary);display:flex;justify-content:space-between;align-items:center;border-left:3px solid #3b82f6;';
                legHeader.innerHTML = `
                    <span>Chặng ${leg.legIndex}: <strong>${leg.from}</strong> (${leg.fromName}) &rarr; <strong>${leg.to}</strong> (${leg.toName})</span>
                    <span style="color:#2563eb;font-weight:700">${Math.round(leg.distance * 10) / 10} m</span>
                `;
                directionsList.appendChild(legHeader);

                if (leg.turnByTurn && leg.turnByTurn.length > 0) {
                    leg.turnByTurn.forEach(step => {
                        const item = document.createElement('div');
                        item.className = 'turn-step-item';
                        let iconClass = 'straight';
                        if (step.action === 'arrive') iconClass = 'arrive';
                        else if (step.action === 'start') iconClass = 'start';
                        else if (step.action.includes('right')) iconClass = 'right';
                        else if (step.action.includes('left'))  iconClass = 'left';

                        const angleBadge = (step.angle && Math.abs(step.angle) > 20)
                            ? ` <small>(${Math.round(step.angle)}°)</small>` : '';

                        item.innerHTML = `
                            <div class="turn-icon-badge ${iconClass}">
                                ${step.icon || '➡️'}
                            </div>
                            <div class="turn-content">
                                <div>
                                    <span class="turn-action-tag ${iconClass}">${step.action_text || 'Đi tiếp'}</span>
                                    ${angleBadge}
                                </div>
                                <div style="margin-top:3px">
                                    ${step.instruction.replace(/\(([^)]+)\)/g, '<span class="step-dist">($1)</span>')}
                                </div>
                            </div>
                        `;
                        directionsList.appendChild(item);
                    });
                }
            });
        } else if (data.turn_by_turn && data.turn_by_turn.length > 0) {
            renderTurnByTurn(data.turn_by_turn, data.directions);
        }
    }

    function displayResults(data) {
        resultCard.classList.add('active');

        const dist = data.total_distance || data.distance || 0;
        statDistance.textContent = `${Math.round(dist * 10) / 10} m`;

        const totalSec = data.walking_time_sec || Math.round(dist / 1.2);
        const mins = Math.floor(totalSec / 60);
        const secs = totalSec % 60;
        statTime.textContent = mins > 0 ? `${mins}p ${secs}s` : `${secs}s`;

        let badgeHtml = '';
        if (data.start_node && data.start_node !== data.start) {
            const sName = UTC2_NODES[data.start_node]?.name || data.start_node;
            badgeHtml += `🚩 Xuất phát tối ưu: <strong>${data.start_node}</strong> (${sName})`;
        }
        if (data.entry_node && data.entry_node !== data.end) {
            if (badgeHtml) badgeHtml += ' &nbsp;|&nbsp; ';
            badgeHtml += `🚪 Cửa vào tối ưu: <strong>${data.entry_node}</strong> (${UTC2_NODES[data.entry_node]?.name || ''})`;
        }
        if (badgeHtml) {
            doorBadge.style.display = 'flex';
            doorBadge.innerHTML = badgeHtml;
        } else {
            doorBadge.style.display = 'none';
        }

        renderTurnByTurn(data.turn_by_turn, data.directions);
    }

    function renderTurnByTurn(turnByTurn, simpleDirections) {
        directionsList.innerHTML = '';

        if (turnByTurn && turnByTurn.length > 0) {
            turnByTurn.forEach(step => {
                const item = document.createElement('div');
                item.className = 'turn-step-item';

                let iconClass = 'straight';
                if (step.action === 'arrive') iconClass = 'arrive';
                else if (step.action === 'start') iconClass = 'start';
                else if (step.action.includes('right')) iconClass = 'right';
                else if (step.action.includes('left'))  iconClass = 'left';

                const angleBadge = (step.angle && Math.abs(step.angle) > 20)
                    ? ` <small>(${Math.round(step.angle)}°)</small>` : '';

                item.innerHTML = `
                    <div class="turn-icon-badge ${iconClass}">
                        ${step.icon || '➡️'}
                    </div>
                    <div class="turn-content">
                        <div>
                            <span class="turn-action-tag ${iconClass}">${step.action_text || 'Đi tiếp'}</span>
                            ${angleBadge}
                        </div>
                        <div style="margin-top:3px">
                            ${step.instruction.replace(/\(([^)]+)\)/g, '<span class="step-dist">($1)</span>')}
                        </div>
                    </div>
                `;
                directionsList.appendChild(item);
            });
        } else if (simpleDirections && simpleDirections.length > 0) {
            simpleDirections.forEach((dir, i) => {
                const item = document.createElement('div');
                item.className = 'turn-step-item';
                item.innerHTML = `
                    <div class="turn-icon-badge straight">🚶</div>
                    <div class="turn-content">
                        <strong>Bước ${i + 1}:</strong> ${dir.replace(/\(([^)]+)\)/g, '<span class="step-dist">($1)</span>')}
                    </div>
                `;
                directionsList.appendChild(item);
            });
        }
    }

    // =========================================================================
    // Quản lý Đăng nhập / Đăng ký Sinh viên (MSSV Authentication & localStorage)
    // =========================================================================
    function initAuth() {
        const DEFAULT_USERS = [
            { mssv: '6351071001', name: 'Nguyễn Văn An', className: 'CNTT K63', password: '123456' },
            { mssv: '6351071002', name: 'Trần Thị Mai', className: 'Kỹ thuật GT K63', password: '123456' },
        ];

        // Khởi tạo danh sách người dùng trong localStorage nếu chưa có
        if (!localStorage.getItem('utc2_users')) {
            localStorage.setItem('utc2_users', JSON.stringify(DEFAULT_USERS));
        }

        // Kiểm tra phiên đăng nhập đã lưu
        const savedUserJson = localStorage.getItem('utc2_current_user');
        if (savedUserJson) {
            try {
                const currentUser = JSON.parse(savedUserJson);
                loginSuccess(currentUser, false);
            } catch (e) {
                showAuthModal();
            }
        } else {
            showAuthModal();
        }

        // Chuyển tab Đăng nhập / Đăng ký
        tabBtnLogin.addEventListener('click', () => {
            tabBtnLogin.classList.add('active');
            tabBtnRegister.classList.remove('active');
            formLogin.classList.add('active');
            formRegister.classList.remove('active');
            hideAuthAlert();
        });

        tabBtnRegister.addEventListener('click', () => {
            tabBtnRegister.classList.add('active');
            tabBtnLogin.classList.remove('active');
            formRegister.classList.add('active');
            formLogin.classList.remove('active');
            hideAuthAlert();
        });

        // Click điền nhanh tài khoản demo
        document.querySelectorAll('.btn-demo-fill').forEach(btn => {
            btn.addEventListener('click', () => {
                const mssv = btn.getAttribute('data-mssv');
                const pass = btn.getAttribute('data-pass');
                loginMssv.value = mssv;
                loginPassword.value = pass;
                hideAuthAlert();
            });
        });

        // Xử lý submit Đăng nhập
        formLogin.addEventListener('submit', (e) => {
            e.preventDefault();
            hideAuthAlert();
            const mssv = loginMssv.value.trim();
            const pass = loginPassword.value;

            const users = getUsersList();
            const foundUser = users.find(u => u.mssv === mssv && u.password === pass);

            if (foundUser) {
                showAuthAlert('Đăng nhập thành công! Đang vào hệ thống...', 'success');
                setTimeout(() => {
                    loginSuccess(foundUser, true);
                }, 300);
            } else {
                showAuthAlert('MSSV hoặc Mật khẩu không chính xác! Hãy kiểm tra lại hoặc dùng tài khoản demo bên dưới.', 'error');
            }
        });

        // Xử lý submit Đăng ký
        formRegister.addEventListener('submit', (e) => {
            e.preventDefault();
            hideAuthAlert();
            const fullName = regFullName.value.trim();
            const mssv = regMssv.value.trim();
            const className = regClass.value.trim();
            const pass = regPassword.value;
            const passConfirm = regPasswordConfirm.value;

            if (pass !== passConfirm) {
                showAuthAlert('Mật khẩu xác nhận không khớp! Vui lòng nhập lại.', 'error');
                return;
            }

            const users = getUsersList();
            if (users.some(u => u.mssv === mssv)) {
                showAuthAlert(`MSSV ${mssv} đã tồn tại trong hệ thống! Vui lòng chuyển sang tab Đăng nhập.`, 'error');
                return;
            }

            const newUser = { mssv, name: fullName, className, password: pass };
            users.push(newUser);
            localStorage.setItem('utc2_users', JSON.stringify(users));

            showAuthAlert(`Đăng ký thành công cho sinh viên ${fullName} (${mssv})! Đang vào hệ thống...`, 'success');
            setTimeout(() => {
                loginSuccess(newUser, true);
            }, 500);
        });

        // Xử lý Đăng xuất
        btnLogout.addEventListener('click', () => {
            if (confirm('Bạn có chắc chắn muốn đăng xuất khỏi tài khoản sinh viên?')) {
                localStorage.removeItem('utc2_current_user');
                userProfileBadge.style.display = 'none';
                loginMssv.value = '';
                loginPassword.value = '';
                clearNavigationState();
                showAuthModal();
                showAuthAlert('Đã đăng xuất thành công.', 'success');
            }
        });

        function getUsersList() {
            try {
                return JSON.parse(localStorage.getItem('utc2_users')) || DEFAULT_USERS;
            } catch (e) {
                return DEFAULT_USERS;
            }
        }

        function showAuthModal() {
            authModal.classList.add('active');
            userProfileBadge.style.display = 'none';
        }

        // Xoa toan bo trang thai dan duong (goi khi dang xuat / dang nhap lai)
        function clearNavigationState() {
            // Xoa duong di va marker tren ban do
            if (typeof canvasMap !== 'undefined' && canvasMap) {
                canvasMap.clearRoute();
                canvasMap.setBlockedEdges([]);
            }
            // An the ket qua
            if (resultCard) resultCard.classList.remove('active');
            if (doorBadge) doorBadge.style.display = 'none';
            if (directionsList) directionsList.innerHTML = '';
            if (scheduleAlertBadge) scheduleAlertBadge.style.display = 'none';
            if (multiStopBadge) multiStopBadge.style.display = 'none';
            // Xoa danh sach diem ghe da chon
            if (typeof selectedStops !== 'undefined') {
                selectedStops = [];
            }
            if (typeof chipGrid !== 'undefined' && chipGrid) {
                chipGrid.querySelectorAll('.stop-chip').forEach(c => c.classList.remove('selected'));
            }
            if (typeof selectedStopsList !== 'undefined' && selectedStopsList) {
                selectedStopsList.innerHTML = '';
            }
        }

        function loginSuccess(user, saveToStorage = true) {
            clearNavigationState();
            if (saveToStorage) {
                localStorage.setItem('utc2_current_user', JSON.stringify(user));
            }
            authModal.classList.remove('active');
            userProfileBadge.style.display = 'flex';
            headerUserName.textContent = user.name;
            headerUserMssv.textContent = `MSSV: ${user.mssv} (${user.className || 'UTC2'})`;
        }

        function showAuthAlert(msg, type = 'error') {
            authAlert.textContent = msg;
            authAlert.className = `auth-alert ${type}`;
            authAlert.style.display = 'block';
        }

        function hideAuthAlert() {
            authAlert.style.display = 'none';
        }
    }

    // =========================================================================
    // Modal So sánh giải thuật (Benchmark — 3 giải thuật cốt lõi DSA)
    // =========================================================================
    function initBenchmarkModal() {
        btnOpenBenchmark.addEventListener('click', () => {
            benchmarkModal.classList.add('active');
            runBenchmarkUI();
        });
        btnCloseBenchmark.addEventListener('click', () => benchmarkModal.classList.remove('active'));
        benchmarkModal.addEventListener('click', e => {
            if (e.target === benchmarkModal) benchmarkModal.classList.remove('active');
        });
        btnRunBenchmark.addEventListener('click', runBenchmarkUI);
    }

    function runBenchmarkUI() {
        const start = benchStart.value;
        const end   = benchEnd.value;

        benchmarkLoading.style.display = 'block';
        benchmarkTable.style.opacity   = '0.4';

        // Dùng setTimeout để trình duyệt kịp render trạng thái loading
        setTimeout(() => {
            try {
                const data = GraphEngine.benchmark(start, end, '');
                benchmarkBody.innerHTML = '';

                data.metrics.forEach(m => {
                    const tr = document.createElement('tr');
                    let tagClass = 'dijkstra';
                    if (m.name.includes('Bellman')) tagClass = 'bellman';
                    if (m.name.includes('Prim'))    tagClass = 'prim';

                    tr.innerHTML = `
                        <td>
                            <span class="algo-tag ${tagClass}">${m.name}</span>
                            <div style="font-size:0.75rem;font-weight:600;color:#e2e8f0;margin-top:2px">${m.category}</div>
                            <div style="font-size:0.72rem;color:var(--text-secondary);margin-top:1px">${m.description}</div>
                        </td>
                        <td><strong style="color:var(--accent-gold);">${m.time_us} &mu;s</strong></td>
                        <td><code style="font-size:0.75rem;padding:2px 4px;background:rgba(15,23,42,0.8);border-radius:4px;color:#38bdf8;">${m.complexity}</code></td>
                        <td>${m.visited_nodes} lượt xét</td>
                        <td><strong style="color:#f8fafc;">${m.result_label}</strong></td>
                    `;
                    benchmarkBody.appendChild(tr);
                });

                benchmarkInsight.innerHTML = `💡 <strong>Đối chiếu học thuật DSA:</strong> ${data.conclusion}`;
            } catch (err) {
                benchmarkInsight.textContent = 'Lỗi khi chạy Benchmark: ' + err.message;
            } finally {
                benchmarkLoading.style.display = 'none';
                benchmarkTable.style.opacity   = '1';
            }
        }, 50);
    }
});
