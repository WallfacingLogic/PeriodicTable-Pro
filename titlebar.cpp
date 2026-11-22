#include "titlebar.h"

// Qt6: globalPos() 已弃用，统一用 globalPosition()
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define GLOBAL_POS(e) ((e)->globalPosition().toPoint())
#else
#define GLOBAL_POS(e) ((e)->globalPos())
#endif
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QIcon>
#include <QLabel>
#include <QHBoxLayout>
#include <QWindow>

TitleButton::TitleButton(Kind kind, QWidget *parent)
    : QPushButton(parent), m_kind(kind) {
    setFixedSize(46, 40);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void TitleButton::setRestorable(bool r) {
    if (m_restorable == r) return;
    m_restorable = r;
    update();
}

void TitleButton::enterEvent(QEnterEvent *) { m_hovered = true; update(); }
void TitleButton::leaveEvent(QEvent *) { m_hovered = false; update(); }

void TitleButton::paintEvent(QPaintEvent *) {
    QPainter p(this);
    QColor bg;
    if (m_kind == Close && m_hovered) bg = QColor(230, 68, 70);
    else if (m_hovered) bg = QColor(255, 255, 255, 32);
    if (bg.isValid()) {
        p.setPen(Qt::NoPen);
        p.setBrush(bg);
        p.drawRect(rect());
    }
    const QColor fg = (m_kind == Close && m_hovered) ? QColor(Qt::white)
                                                      : QColor(203, 213, 225);
    p.setPen(QPen(fg, 1.4, Qt::SolidLine, Qt::RoundCap));
    p.setBrush(Qt::NoBrush);
    const int cx = width() / 2, cy = height() / 2;

    if (m_kind == Minimize) {
        p.drawLine(QPoint(cx - 6, cy), QPoint(cx + 6, cy));
    } else if (m_kind == Maximize) {
        if (m_restorable) {
            p.drawRect(QRect(cx - 7, cy - 5, 10, 10));
            p.drawRect(QRect(cx - 3, cy - 1, 10, 10));
        } else {
            p.drawRect(QRect(cx - 7, cy - 6, 14, 12));
        }
    } else {
        p.drawLine(QPoint(cx - 6, cy - 6), QPoint(cx + 6, cy + 6));
        p.drawLine(QPoint(cx + 6, cy - 6), QPoint(cx - 6, cy + 6));
    }
}

TitleBar::TitleBar(QWidget *parent) : QWidget(parent) {
    setFixedHeight(44);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setMouseTracking(true);

    auto *row = new QHBoxLayout(this);
    row->setContentsMargins(16, 0, 0, 0);
    row->setSpacing(0);

    QLabel *icon = new QLabel(this);
    QPixmap pm = QIcon(":/resources/PeriodicTable.png").pixmap(22, 22);
    icon->setPixmap(pm);
    icon->setFixedSize(24, 24);
    row->addWidget(icon);
    row->addSpacing(12);

    QLabel *title = new QLabel(QStringLiteral("元素周期表"), this);
    title->setObjectName("tbTitle");
    row->addWidget(title);
    row->addSpacing(10);

    QLabel *sub = new QLabel(QStringLiteral("PERIODIC TABLE PRO"), this);
    sub->setObjectName("tbSub");
    row->addWidget(sub);

    row->addStretch(1);

    m_min = new TitleButton(TitleButton::Minimize, this);
    m_max = new TitleButton(TitleButton::Maximize, this);
    m_close = new TitleButton(TitleButton::Close, this);
    m_min->setObjectName("tbBtn");
    m_max->setObjectName("tbBtn");
    m_close->setObjectName("tbBtn");
    row->addWidget(m_min);
    row->addWidget(m_max);
    row->addWidget(m_close);

    connect(m_min, &QPushButton::clicked, this, &TitleBar::minimizeClicked);
    connect(m_max, &QPushButton::clicked, this, &TitleBar::maximizeClicked);
    connect(m_close, &QPushButton::clicked, this, &TitleBar::closeClicked);
}

void TitleBar::setMaximized(bool maximized) { m_max->setRestorable(maximized); }

void TitleBar::mousePressEvent(QMouseEvent *e) {
    if (e->button() != Qt::LeftButton) return;
    QWidget *win = window();
    if (!win) return;
    m_dragging = true;
    m_dragPos = GLOBAL_POS(e);

    // Qt 5.15+ 提供原生窗口拖拽：支持 Win11 贴靠布局、拖拽动画，最稳。
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    if (win->windowHandle() && win->windowHandle()->startSystemMove()) {
        m_dragging = false;
        return;
    }
#endif
}

void TitleBar::mouseMoveEvent(QMouseEvent *e) {
    if (!m_dragging) return;
    QWidget *win = window();
    if (!win) return;
    const QPoint delta = GLOBAL_POS(e) - m_dragPos;
    win->move(win->pos() + delta);
    m_dragPos = GLOBAL_POS(e);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *) { m_dragging = false; }

void TitleBar::mouseDoubleClickEvent(QMouseEvent *) {
    QWidget *win = window();
    if (!win) return;
    if (win->isMaximized()) win->showNormal();
    else win->showMaximized();
}
