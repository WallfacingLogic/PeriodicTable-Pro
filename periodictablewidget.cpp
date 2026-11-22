#include "periodictablewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QFont>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QFontMetrics>

PeriodicTableWidget::PeriodicTableWidget(QWidget *parent) : QWidget(parent) {
    setMouseTracking(true);
    setMinimumSize(360, 240);          // 允许随窗口自由缩小
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    m_hoverZ = 0;
    m_selectedZ = 0;
    m_catMask = ElementDatabase::kAllCategories;
    m_resizeTimer.setSingleShot(true);
    m_resizeTimer.setInterval(180);
    connect(&m_resizeTimer, &QTimer::timeout, this, [this]() {
        m_resizing = false;
        m_tableDirty = true;      // 停止缩放后精绘表格
        m_bgDirty = true;         // 重建完整背景光晕
        update();
    });
}

void PeriodicTableWidget::beginResizing() {
    m_resizing = true;
    m_resizeTimer.start();   // 每次 resize 重置计时器
}

void PeriodicTableWidget::setElements(const QVector<ElementData> &els) {
    Q_UNUSED(els);
    rebuildLayout();
    update();
}

void PeriodicTableWidget::setCategoryFilter(quint32 catMask) {
    if (m_catMask == catMask) return;
    m_catMask = catMask;
    m_tableDirty = true;
    update();
}

QSize PeriodicTableWidget::sizeHint() const { return QSize(1180, 560); }

void PeriodicTableWidget::demoHover(int z) { refreshHover(z); }

void PeriodicTableWidget::rebuildLayout() {
    m_cells.clear();
    const int marginTop = 26;
    const int marginBottom = 40;
    const int marginX = 8;

    const int availW0 = qMax(360, width() - 2 * marginX);
    const int availH = qMax(200, height() - marginTop - marginBottom);
    int side = qMin(availW0 / 18, availH / 10);
    if (side < 20) side = 20;
    if (side > MAX_CELL) side = MAX_CELL;

    const int gutter = qMax(26, side / 2 + 8);
    const int availW = qMax(360, width() - gutter - 2 * marginX);
    side = qMin(availW / 18, availH / 10);
    if (side < 20) side = 20;
    if (side > MAX_CELL) side = MAX_CELL;
    m_side = side;

    const int gridW = side * 18;
    m_startX = marginX + gutter + qMax(0, (availW - gridW) / 2);
    m_startY = marginTop + qMax(0, (availH - side * 10) / 2);

    auto cellAt = [&](int row, int col) {
        return QRect(m_startX + col * side, m_startY + row * side, side - 2, side - 2);
    };

    m_cells.reserve(120);
    for (quint32 i = 0; i < ElementDatabase::elementCount(); i++) {
        const ElementData &e = ElementDatabase::elementAt(i);
        if (e.atomicNumber == 0) continue;
        if (e.period < 1 || e.period > 7 || e.group < 1 || e.group > 18) continue;

        const bool lanthanide = (e.atomicNumber >= 57 && e.atomicNumber <= 71);
        const bool actinide = (e.atomicNumber >= 89 && e.atomicNumber <= 103);

        Cell cell;
        cell.z = e.atomicNumber;
        cell.elementIndex = i;
        if (lanthanide || actinide) {
            const int idx = lanthanide ? (e.atomicNumber - 57) : (e.atomicNumber - 89);
            const int row = lanthanide ? 8 : 9;
            cell.rect = cellAt(row, 2 + idx);
        } else {
            cell.rect = cellAt(e.period - 1, e.group - 1);
        }
        m_cells.append(cell);
    }

    for (int period : {6, 7}) {
        Cell ph;
        ph.rect = cellAt(period - 1, 2);
        ph.placeholder = true;
        ph.placeholderText = (period == 6) ? QStringLiteral("57-71") : QStringLiteral("89-103");
        m_cells.append(ph);
    }

    // O(1) 命中网格
    m_hitGrid.fill(0, 18 * 10);
    for (const auto &cell : m_cells) {
        const int col = (cell.rect.left() - m_startX) / side;
        const int row = (cell.rect.top() - m_startY) / side;
        if (col >= 0 && col < 18 && row >= 0 && row < 10)
            m_hitGrid[row * 18 + col] = cell.z;
    }

    m_lastHoverRect = QRect();
    m_lastCardRect = QRect();
    m_tableDirty = true;
}

void PeriodicTableWidget::resizeEvent(QResizeEvent *e) {
    Q_UNUSED(e)
    beginResizing();
    rebuildLayout();       // 仅重算格子坐标（轻量，无分配大对象）
    // 缩放期间不重建昂贵缓存，直接快速刷新；精绘在 timer 超时后进行
    update();
}

void PeriodicTableWidget::ensureBackground() {
    // 缩放期间不重建任何缓存——直接拉伸上一帧快照。
    if (m_resizing) return;
    if (m_bgLogical == size() && !m_bgPix.isNull() && !m_bgDirty) return;

    const qreal dpr = devicePixelRatioF();
    m_bgPix = QPixmap(size() * dpr);
    m_bgPix.setDevicePixelRatio(dpr);
    QPainter p(&m_bgPix);
    const int W = width(), H = height();
    QLinearGradient bg(0, 0, 0, H);
    bg.setColorAt(0.0, QColor(8, 12, 24));
    bg.setColorAt(0.5, QColor(13, 20, 38));
    bg.setColorAt(1.0, QColor(6, 10, 22));
    p.fillRect(0, 0, W, H, bg);
    {
        QRectF b1(W * 0.15, H * 0.1, 380, 380);
        QRadialGradient g(b1.center(), b1.width() / 2);
        g.setColorAt(0.0, QColor(56, 189, 248, 70));
        g.setColorAt(1.0, QColor(56, 189, 248, 0));
        p.fillRect(b1, g);
    }
    {
        QRectF b2(W * 0.65, H * 0.45, 420, 420);
        QRadialGradient g(b2.center(), b2.width() / 2);
        g.setColorAt(0.0, QColor(168, 85, 247, 55));
        g.setColorAt(1.0, QColor(168, 85, 247, 0));
        p.fillRect(b2, g);
    }
    m_bgDirty = false;
    m_bgLogical = size();
}

// 合成完整静态画面快照（供缩放期拉伸）。仅在非缩放、缓存更新后调用。
void PeriodicTableWidget::ensureFrame() {
    if (m_resizing) return;
    if (m_bgPix.isNull() || m_tablePix.isNull()) return;
    if (m_framePix.size() == m_bgPix.size() && !m_framePix.isNull() && !m_tableDirty)
        return;
    m_framePix = QPixmap(m_bgPix.size());
    m_framePix.setDevicePixelRatio(m_bgPix.devicePixelRatio());
    QPainter p(&m_framePix);
    p.drawPixmap(0, 0, m_bgPix);
    p.drawPixmap(0, 0, m_tablePix);
}

void PeriodicTableWidget::ensureTableCache() {
    if (m_resizing) return; // 缩放期间不重建昂贵表格缓存
    if (!m_tableDirty && m_tableLogical == size() && !m_tablePix.isNull()) return;
    const qreal dpr = devicePixelRatioF();
    m_tablePix = QPixmap(size() * dpr);
    m_tablePix.setDevicePixelRatio(dpr);
    m_tablePix.fill(Qt::transparent);
    QPainter p(&m_tablePix);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    drawAxisLabels(p);
    for (const auto &cell : m_cells)
        drawCell(p, cell, false, cell.z == m_selectedZ);
    drawLegend(p);
    m_tableLogical = size();
    m_tableDirty = false;
}

int PeriodicTableWidget::hitTest(const QPoint &pt) const {
    const int col = (pt.x() - m_startX) / m_side;
    const int row = (pt.y() - m_startY) / m_side;
    if (col < 0 || col >= 18 || row < 0 || row >= 10) return 0;
    return m_hitGrid[row * 18 + col];
}

QRect PeriodicTableWidget::hoverCellRect(int z) const {
    for (const auto &c : m_cells)
        if (c.z == z && !c.placeholder) return c.rect;
    return QRect();
}

QRect PeriodicTableWidget::hoverCardRect(const QRect &cr) const {
    const int W = 210, H = 96;
    if (cr.isNull()) return QRect();
    int x = cr.center().x() - W / 2;
    int y = cr.top() - H - 8;
    if (y < 4) y = cr.bottom() + 8;
    x = qBound(4, x, width() - W - 4);
    y = qBound(4, y, height() - H - 4);
    return QRect(x, y, W, H);
}

void PeriodicTableWidget::refreshHover(int z) {
    if (z == m_hoverZ) return;
    // 外扩 2px 覆盖抗锯齿边缘，确保旧痕迹被完全擦除
    QRect dirty = m_lastHoverRect.united(m_lastCardRect).adjusted(-2, -2, 2, 2);
    m_hoverZ = z;
    if (z == 0) {
        m_lastHoverRect = QRect();
        m_lastCardRect = QRect();
    } else {
        const QRect cr = hoverCellRect(z);
        m_lastHoverRect = cr;
        m_lastCardRect = hoverCardRect(cr);
        dirty = dirty.united(cr.adjusted(-2, -2, 2, 2))
                     .united(m_lastCardRect.adjusted(-2, -2, 2, 2));
    }
    update(dirty);
}

void PeriodicTableWidget::mouseMoveEvent(QMouseEvent *e) {
    const int z = hitTest(e->pos());
    if (z != m_hoverZ) refreshHover(z);
}

void PeriodicTableWidget::leaveEvent(QEvent *) { refreshHover(0); }

void PeriodicTableWidget::mousePressEvent(QMouseEvent *e) {
    const int z = hitTest(e->pos());
    if (z) {
        m_selectedZ = z;
        m_tableDirty = true;
        update();
        emit elementSelected(z);
    }
}

void PeriodicTableWidget::drawAxisLabels(QPainter &p) const {
    const int side = m_side, startX = m_startX, startY = m_startY;
    p.setPen(QColor(148, 163, 184, 170));
    QFont hf = p.font();
    hf.setPixelSize(qMax(8, side / 3));
    hf.setBold(true);
    p.setFont(hf);
    for (int g = 1; g <= 18; g++)
        p.drawText(QRect(startX + (g - 1) * side, startY - side / 2, side, side / 2),
                   Qt::AlignCenter, QString::number(g));
    for (int per = 1; per <= 7; per++)
        p.drawText(QRect(startX - side / 2, startY + (per - 1) * side, side / 2, side),
                   Qt::AlignCenter, QString::number(per));
    p.drawText(QRect(startX - side / 2, startY + 8 * side, side / 2, side), Qt::AlignCenter, "Ln");
    p.drawText(QRect(startX - side / 2, startY + 9 * side, side / 2, side), Qt::AlignCenter, "Ac");
}

void PeriodicTableWidget::drawCell(QPainter &p, const Cell &cell, bool hover, bool sel) const {
    const QRect r = cell.rect;
    const int side = m_side;

    if (cell.placeholder) {
        QPainterPath path;
        path.addRoundedRect(r, side / 5, side / 5);
        p.fillPath(path, QColor(148, 163, 184, 26));
        p.setPen(QPen(QColor(148, 163, 184, 60), 1.0, Qt::DashLine));
        p.drawPath(path);
        p.setPen(QColor(148, 163, 184, 150));
        QFont pf = p.font();
        pf.setPixelSize(qMax(7, r.height() / 6));
        p.setFont(pf);
        p.drawText(r, Qt::AlignCenter, cell.placeholderText);
        return;
    }

    const ElementData &e = ElementDatabase::elementAt(cell.elementIndex);
    const bool dim = ((m_catMask & (quint32(1) << e.categoryIndex)) == 0);

    QColor base = e.color;
    if (dim) base = QColor(base.red(), base.green(), base.blue(), 40);
    if (hover && !dim) base = base.lighter(135);
    if (sel) base = QColor("#fbbf24");

    QPainterPath path;
    path.addRoundedRect(r, side / 5, side / 5);

    QLinearGradient grad(r.topLeft(), r.bottomRight());
    QColor c1 = base.lighter(118), c2 = base.darker(118);
    if (dim) { c1.setAlpha(60); c2.setAlpha(60); }
    grad.setColorAt(0.0, c1);
    grad.setColorAt(1.0, c2);
    p.fillPath(path, grad);

    QLinearGradient shine(r.topLeft(), r.topLeft() + QPointF(0, r.height() * 0.55));
    shine.setColorAt(0.0, QColor(255, 255, 255, 120));
    shine.setColorAt(1.0, QColor(255, 255, 255, 0));
    p.fillPath(path, shine);

    p.setPen(QPen(sel ? QColor("#f59e0b") : QColor(255, 255, 255, 60), sel ? 2.0 : 1.0));
    p.drawPath(path);

    QColor txt = dim ? QColor(148, 163, 184, 120) : QColor(15, 23, 42);
    if (sel) txt = QColor(15, 23, 42);
    p.setPen(txt);

    QFont f = p.font();
    f.setBold(true);
    f.setPixelSize(qMax(8, r.height() / 4));
    p.setFont(f);
    p.drawText(r.adjusted(2, 1, -2, -2), Qt::AlignTop | Qt::AlignLeft, QString::number(e.atomicNumber));

    QFont f2 = f; f2.setPixelSize(qMax(10, r.height() / 3));
    p.setFont(f2);
    p.drawText(r, Qt::AlignCenter, e.symbol);

    QFont f3 = f; f3.setPixelSize(qMax(7, r.height() / 5)); f3.setBold(false);
    p.setFont(f3);
    p.drawText(r.adjusted(0, 0, 0, -1), Qt::AlignBottom | Qt::AlignCenter, e.nameCn);
}

void PeriodicTableWidget::drawLegend(QPainter &p) const {
    QFont lf = p.font();
    lf.setPixelSize(11);
    p.setFont(lf);
    const QFontMetrics fm(lf);
    const int dot = 8, gap = 5, itemGap = 14;
    int x = m_startX;
    const int y = height() - 18;
    for (quint32 c = 0; c < ElementDatabase::categoryCount(); ++c) {
        QColor swatch(148, 163, 184);
        const quint32 n = ElementDatabase::countInCategory(c);
        if (n > 0) swatch = ElementDatabase::elementAt(ElementDatabase::elementAtInCategory(c, 0)).color;
        const QString label = QString("%1 %2").arg(ElementDatabase::categoryName(c)).arg(n);
        const int w = dot + gap + fm.horizontalAdvance(label) + itemGap;
        if (x + w > width() - 8) break;
        const bool active = ((m_catMask & (quint32(1) << c)) != 0);
        p.setPen(Qt::NoPen);
        p.setBrush(swatch);
        p.drawEllipse(QRectF(x, y - dot / 2.0 - 2, dot, dot));
        p.setPen(active ? QColor(248, 250, 252) : QColor(148, 163, 184));
        p.drawText(QPointF(x + dot + gap, y), label);
        if (active) {
            p.setPen(QPen(QColor(251, 191, 36, 180), 1));
            p.drawLine(QPointF(x, y + 5), QPointF(x + w - itemGap, y + 5));
        }
        x += w;
    }
}

void PeriodicTableWidget::paintHoverCard(QPainter &p, const QRect &r) const {
    const ElementData *e = ElementDatabase::byAtomicNumber(m_hoverZ);
    if (!e) return;
    QPainterPath path;
    path.addRoundedRect(r, 12, 12);
    p.save();
    p.setClipPath(path);
    p.fillPath(path, QColor(16, 24, 42, 226));
    {
        QRadialGradient g(r.topLeft() + QPointF(30, 0), 130);
        g.setColorAt(0.0, QColor(e->color.red(), e->color.green(), e->color.blue(), 90));
        g.setColorAt(1.0, QColor(e->color.red(), e->color.green(), e->color.blue(), 0));
        p.fillPath(path, g);
    }
    {
        QLinearGradient g(r.topLeft(), r.topLeft() + QPointF(0, r.height() * 0.55));
        g.setColorAt(0.0, QColor(255, 255, 255, 42));
        g.setColorAt(1.0, QColor(255, 255, 255, 0));
        p.fillPath(path, g);
    }
    p.restore();
    p.setPen(QPen(QColor(255, 255, 255, 70), 1));
    p.setBrush(Qt::NoBrush);
    p.drawPath(path);

    p.setPen(QColor(148, 163, 184));
    QFont f = p.font(); f.setPixelSize(10); p.setFont(f);
    p.drawText(QRect(r.left() + 14, r.top() + 10, 60, 16), Qt::AlignLeft | Qt::AlignVCenter,
               QString("Z = %1").arg(e->atomicNumber));
    QFont fs = f; fs.setPixelSize(26); fs.setBold(true); p.setFont(fs);
    p.setPen(QColor(248, 250, 252));
    p.drawText(QRect(r.left() + 14, r.top() + 22, 70, 40), Qt::AlignLeft | Qt::AlignVCenter, e->symbol);
    QFont fc = f; fc.setPixelSize(16); fc.setBold(true); p.setFont(fc);
    p.setPen(QColor(226, 232, 240));
    p.drawText(QRect(r.left() + 84, r.top() + 26, r.width() - 96, 26), Qt::AlignLeft | Qt::AlignVCenter, e->nameCn);
    QFont fe = f; fe.setPixelSize(11); p.setFont(fe);
    p.setPen(QColor(148, 163, 184));
    p.drawText(QRect(r.left() + 84, r.top() + 50, r.width() - 96, 18), Qt::AlignLeft | Qt::AlignVCenter, e->name);
    p.setPen(QColor(e->color.lighter(140)));
    p.drawText(QRect(r.left() + 14, r.bottom() - 26, r.width() - 28, 18), Qt::AlignLeft | Qt::AlignVCenter, e->category);
}

void PeriodicTableWidget::paintEvent(QPaintEvent *e) {
    QPainter p(this);
    const QRect exposed = e->rect();

    // ===== 缩放快速路径：把上一帧完整玻璃画面做一次 GPU 拉伸 blit =====
    // 保留圆角/渐变/光晕/抗锯齿全部质感；每帧只有一次纹理缩放，极快。
    if (m_resizing) {
        if (!m_framePix.isNull()) {
            p.setRenderHint(QPainter::SmoothPixmapTransform, true); // 平滑拉伸，文字/圆角不锯齿
            p.drawPixmap(rect(), m_framePix, m_framePix.rect());
        } else {
            p.fillRect(rect(), QColor(10, 16, 30));
        }
        return;
    }

    // ===== 静止精绘路径：重建缓存 + 1:1 精绘 + 更新快照 =====
    ensureBackground();
    ensureTableCache();
    ensureFrame();

    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.setClipRect(exposed);
    p.drawPixmap(0, 0, m_bgPix);
    p.drawPixmap(0, 0, m_tablePix);

    if (m_hoverZ != 0) {
        for (const auto &cell : m_cells)
            if (cell.z == m_hoverZ && !cell.placeholder) {
                drawCell(p, cell, true, cell.z == m_selectedZ);
                break;
            }
        const QRect cr = hoverCellRect(m_hoverZ);
        if (!cr.isNull()) paintHoverCard(p, hoverCardRect(cr));
    }
}