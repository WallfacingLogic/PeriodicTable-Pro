#include "glassstyle.h"
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QPalette>
#include <QGraphicsDropShadowEffect>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

GlassHost::GlassHost(QWidget *parent) : QWidget(parent) {
    setObjectName("glassHost");
    setAttribute(Qt::WA_StyledBackground, false);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAttribute(Qt::WA_NoSystemBackground, true);
    m_resizeTimer.setSingleShot(true);
    m_resizeTimer.setInterval(180);
    connect(&m_resizeTimer, &QTimer::timeout, this, [this]() {
        m_resizing = false;
        m_bgCache = QPixmap();   // 强制重建完整背景
        update();
    });
}

void GlassHost::paintEvent(QPaintEvent *) {
    // 缩放期间：直接拉伸已有完整玻璃背景快照（保留渐变/光晕），一次纹理缩放，极快
    if (m_resizing) {
        QPainter p(this);
        p.setRenderHint(QPainter::SmoothPixmapTransform, true);
        if (!m_bgCache.isNull())
            p.drawPixmap(rect(), m_bgCache, m_bgCache.rect());
        else
            p.fillRect(rect(), QColor(11, 17, 32));
        return;
    }

    const qreal dpr = devicePixelRatioF();
    if (m_bgCache.size() != size() * dpr || m_bgCache.devicePixelRatio() != dpr) {
        m_bgCache = QPixmap(size() * dpr);
        m_bgCache.setDevicePixelRatio(dpr);
        QPainter p(&m_bgCache);
        const int W = width(), H = height();

        QLinearGradient bg(0, 0, W * 0.35, H);
        bg.setColorAt(0.0, QColor(11, 17, 32));
        bg.setColorAt(0.45, QColor(14, 21, 40));
        bg.setColorAt(1.0, QColor(7, 11, 22));
        p.fillRect(0, 0, W, H, bg);
        {
            QRadialGradient g(QPointF(W * 0.12, H * 0.02), W * 0.55);
            g.setColorAt(0.0, QColor(56, 189, 248, 60));
            g.setColorAt(0.55, QColor(56, 189, 248, 16));
            g.setColorAt(1.0, QColor(56, 189, 248, 0));
            p.fillRect(0, 0, W, H, g);
        }
        {
            QRadialGradient g(QPointF(W * 0.95, H * 0.98), W * 0.6);
            g.setColorAt(0.0, QColor(147, 51, 234, 52));
            g.setColorAt(0.55, QColor(147, 51, 234, 14));
            g.setColorAt(1.0, QColor(147, 51, 234, 0));
            p.fillRect(0, 0, W, H, g);
        }
        {
            QRadialGradient g(QPointF(W * 0.62, H * 0.18), W * 0.38);
            g.setColorAt(0.0, QColor(16, 185, 129, 26));
            g.setColorAt(1.0, QColor(16, 185, 129, 0));
            p.fillRect(0, 0, W, H, g);
        }
    }
    QPainter p(this);
    p.drawPixmap(0, 0, m_bgCache);
}

void GlassHost::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    m_resizing = true;
    m_resizeTimer.start();
}

void GlassStyle::enableAcrylic(quintptr winHandle, bool dark) {
    Q_UNUSED(winHandle)
    Q_UNUSED(dark)
#ifdef Q_OS_WIN
    // 性能优先：Acrylic 让 DWM 每帧对整窗做模糊合成，鼠标滑动时明显掉帧。
    // 背景已由 GlassHost 自绘（渐变 + 光晕），这里不再启用系统背景材质。
    // 如需原生效果可改用 enableMica（Win11 Mica 开销更低）。
#endif
}

void GlassStyle::enableMica(quintptr winHandle, bool dark) {
    Q_UNUSED(winHandle)
    Q_UNUSED(dark)
#ifdef Q_OS_WIN
    HWND hwnd = (HWND)winHandle;
    if (!hwnd) return;
    enum : int { DWMWA_SYSTEMBACKDROP_TYPE = 38 };
    int backdrop = 1; // Mica
    DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
    int darkMode = dark ? 1 : 0;
    DwmSetWindowAttribute(hwnd, 20, &darkMode, sizeof(darkMode));
#endif
}

QString GlassStyle::themeStyleSheet() {
    return R"(
    QMainWindow {
        background-color: #0b1120;
    }
    QWidget#glassHost {
        background: transparent;
    }
    QWidget#centralHost {
        background: transparent;
    }
    /* frosted glass panel */
    QWidget#glassPanel {
        background-color: rgba(22, 33, 55, 150);
        border: 1px solid rgba(255,255,255,26);
        border-radius: 14px;
    }
    QLabel {
        color: #e2e8f0;
        font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
        background: transparent;
    }
    QLabel#titleText {
        color: #f8fafc;
        font-size: 22px;
        font-weight: 800;
        letter-spacing: 1px;
    }
    QLabel#subTitle {
        color: #94a3b8;
        font-size: 12px;
        letter-spacing: 2px;
    }
    /* custom title bar */
    QWidget#tbTitle, QLabel#tbTitle {
        color: #f8fafc;
        font-size: 14px;
        font-weight: 700;
        font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
        background: transparent;
    }
    QLabel#tbSub {
        color: #64748b;
        font-size: 10px;
        font-weight: 600;
        letter-spacing: 2px;
        background: transparent;
    }
    QPushButton#tbBtn {
        background: transparent;
        border: none;
    }
    /* search box - glass */
    QLineEdit#searchBox {
        background-color: rgba(30, 41, 59, 180);
        border: 1px solid rgba(148, 163, 184, 60);
        border-radius: 10px;
        padding: 7px 14px;
        color: #e2e8f0;
        selection-background-color: rgba(59,130,246,180);
        font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
        font-size: 13px;
    }
    QLineEdit#searchBox:focus {
        border: 1px solid rgba(59,130,246,200);
    }
    /* combo - glass */
    QComboBox#categoryCombo {
        background-color: rgba(30, 41, 59, 180);
        border: 1px solid rgba(148, 163, 184, 60);
        border-radius: 10px;
        padding: 6px 12px;
        color: #e2e8f0;
        min-width: 130px;
    }
    QComboBox#categoryCombo::drop-down {
        border: none;
        background: transparent;
        width: 18px;
    }
    QComboBox#categoryCombo::down-arrow {
        image: none;
        border-left: 1px solid #94a3b8;
        border-bottom: 1px solid #94a3b8;
        width: 6px;
        height: 6px;
        margin-top: -2px;
    }
    QComboBox#categoryCombo QAbstractItemView {
        background-color: #1e293b;
        border: 1px solid #334155;
        color: #e2e8f0;
        selection-background-color: rgba(59,130,246,200);
    }
    /* primary button - glass gradient */
    QPushButton#searchBtn {
        background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
            stop:0 rgba(56,189,248,200), stop:1 rgba(59,130,246,220));
        border: 1px solid rgba(255,255,255,30);
        border-radius: 10px;
        padding: 7px 20px;
        color: #f8fafc;
        font-weight: 600;
        font-size: 13px;
    }
    QPushButton#searchBtn:hover {
        background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
            stop:0 rgba(56,189,248,240), stop:1 rgba(59,130,246,255));
    }
    QPushButton#searchBtn:pressed {
        background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
            stop:0 rgba(37,99,235,255), stop:1 rgba(29,78,216,255));
    }
    /* category chip buttons */
    QPushButton#catChip {
        background: rgba(30, 41, 59, 140);
        border: 1px solid rgba(148, 163, 184, 50);
        border-radius: 10px;
        padding: 5px 12px;
        color: #cbd5e1;
        font-size: 12px;
    }
    QPushButton#catChip:hover {
        background: rgba(51, 65, 85, 180);
        color: #f8fafc;
    }
    QPushButton#catChip:checked {
        background: rgba(59, 130, 246, 200);
        border-color: rgba(96, 165, 250, 220);
        color: #f8fafc;
    }
    /* scroll area transparent */
    QScrollArea {
        background: transparent;
        border: none;
    }
    QScrollArea > QWidget > QWidget {
        background: transparent;
    }
    /* splitter handle */
    QSplitter::handle {
        background: transparent;
        width: 4px;
    }
    QSplitter::handle:hover {
        background: rgba(148, 163, 184, 40);
    }
    /* status bar glass */
    QStatusBar {
        background: rgba(10, 16, 30, 120);
        border-top: 1px solid rgba(148, 163, 184, 20);
        color: #64748b;
    }
    QStatusBar::item {
        border: none;
    }
    /* tooltip */
    QToolTip {
        background-color: #1e293b;
        color: #e2e8f0;
        border: 1px solid #334155;
        border-radius: 6px;
        padding: 5px 9px;
        font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
    }
    )";
}