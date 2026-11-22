#include "elementdetailpanel.h"
#include <QLabel>
#include <QGridLayout>
#include <QFrame>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QFontMetrics>

// ==================== ElementHeroCard ====================

ElementHeroCard::ElementHeroCard(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumHeight(96);
}

void ElementHeroCard::setElement(const ElementData &e) {
    m_e = e;
    m_has = true;
    update();
}

void ElementHeroCard::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRectF card = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    QPainterPath clip;
    clip.addRoundedRect(card, 16, 16);

    p.save();
    p.setClipPath(clip);

    // 底色：元素分类色的低饱和渐变
    QColor accent = m_has ? m_e.color : QColor(120, 140, 170);
    {
        QLinearGradient g(card.topLeft(), card.bottomRight());
        g.setColorAt(0.0, QColor(accent.red(), accent.green(), accent.blue(), 110));
        g.setColorAt(0.55, QColor(accent.red() / 3, accent.green() / 3, accent.blue() / 3, 150));
        g.setColorAt(1.0, QColor(14, 20, 36, 200));
        p.fillRect(card, g);
    }
    // 右上高光
    {
        QRadialGradient g(card.topRight() + QPointF(-40, 10), 150);
        g.setColorAt(0.0, QColor(255, 255, 255, 70));
        g.setColorAt(1.0, QColor(255, 255, 255, 0));
        p.fillRect(card, g);
    }
    // 顶部玻璃高光
    {
        QLinearGradient g(card.topLeft(), card.topLeft() + QPointF(0, card.height() * 0.6));
        g.setColorAt(0.0, QColor(255, 255, 255, 46));
        g.setColorAt(1.0, QColor(255, 255, 255, 0));
        p.fillRect(card, g);
    }
    p.restore();

    // 边框
    p.setPen(QPen(QColor(255, 255, 255, 46), 1));
    p.setBrush(Qt::NoBrush);
    p.drawPath(clip);

    if (!m_has) {
        p.setPen(QColor(203, 213, 225));
        QFont f = p.font();
        f.setPixelSize(15);
        p.setFont(f);
        p.drawText(card, Qt::AlignCenter, QStringLiteral("请选择元素"));
        return;
    }

    // ---- 大号元素符号（按卡片宽度自适应）----
    const qreal cw = card.width(), ch = card.height();
    const qreal symSize = qMin(qreal(58), ch * 0.5);
    const QRectF symRect(card.left() + 12, card.top() + 12, symSize * 1.15, ch - 40);
    {
        QFont f = p.font();
        f.setPixelSize(qMax(20, int(symSize)));
        f.setBold(true);
        p.setFont(f);
        p.setPen(QColor(255, 255, 255, 235));
        p.drawText(symRect, Qt::AlignLeft | Qt::AlignVCenter, m_e.symbol);
    }

    // ---- 右上角原子序数 ----
    {
        QFont f = p.font();
        f.setPixelSize(13);
        f.setBold(true);
        p.setFont(f);
        p.setPen(QColor(255, 255, 255, 170));
        p.drawText(card.adjusted(0, 12, -16, 0),
                   Qt::AlignTop | Qt::AlignRight,
                   QStringLiteral("Z = %1").arg(m_e.atomicNumber));
    }

    // ---- 右侧文字：中文名 / 英文名（按卡片高度自适应）----
    const qreal textLeft = symRect.right() + 6;
    const qreal textW = qMax(qreal(20), card.right() - textLeft - 8);
    {
        QFont f = p.font();
        f.setPixelSize(qMax(13, int(ch * 0.19)));
        f.setBold(true);
        p.setFont(f);
        p.setPen(QColor(255, 255, 255, 240));
        {
            const QFontMetrics fm(f);
            p.drawText(QRectF(textLeft, card.top() + ch * 0.22, textW, ch * 0.3),
                       Qt::AlignLeft | Qt::AlignVCenter,
                       fm.elidedText(m_e.nameCn, Qt::ElideRight, int(textW)));
        }

        f.setPixelSize(qMax(9, int(ch * 0.085)));
        f.setBold(false);
        p.setFont(f);
        p.setPen(QColor(255, 255, 255, 165));
        {
            const QFontMetrics fm(f);
            p.drawText(QRectF(textLeft, card.top() + ch * 0.55, textW, ch * 0.18),
                       Qt::AlignLeft | Qt::AlignVCenter,
                       fm.elidedText(m_e.name, Qt::ElideRight, int(textW)));
        }
    }

    // ---- 底部：分类胶囊 + 原子量 ----
    {
        QFont f = p.font();
        f.setPixelSize(11);
        f.setBold(true);
        p.setFont(f);

        const QFontMetrics fm(f);
        const QString pill = m_e.category;
        const int pw = fm.horizontalAdvance(pill) + 20;
        const qreal pillH = qMin(qreal(24), ch * 0.2);
        const QRectF pillRect(card.left() + 16, card.bottom() - pillH - 8, pw, pillH);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 46));
        p.drawRoundedRect(pillRect, pillH / 2, pillH / 2);
        p.setPen(QColor(255, 255, 255, 235));
        p.drawText(pillRect, Qt::AlignCenter, pill);

        f.setBold(false);
        f.setPixelSize(qMax(9, int(ch * 0.1)));
        p.setFont(f);
        p.setPen(QColor(255, 255, 255, 170));
        p.drawText(QRectF(pillRect.right() + 10, pillRect.top(),
                          qMax(qreal(20), card.right() - pillRect.right() - 16), pillH),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("原子量 %1").arg(QString::number(m_e.atomicMass, 'f', 3)));
    }
}

// ==================== ElementDetailPanel ====================

ElementDetailPanel::ElementDetailPanel(QWidget *parent) : QWidget(parent) {
    setObjectName("glassPanel");
    setMinimumWidth(200);                                  // 允许随窗口自由缩小
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(16, 16, 16, 16);
    lay->setSpacing(12);

    m_title = new QLabel("选择元素", this);
    m_title->setObjectName("titleText");
    QFont f = m_title->font(); f.setPointSize(20); f.setBold(true);
    m_title->setFont(f);

    m_subtitle = new QLabel("点击周期表任意元素查看详细信息", this);
    m_subtitle->setObjectName("subTitle");
    m_subtitle->setWordWrap(true);

    lay->addWidget(m_title);
    lay->addWidget(m_subtitle);

    m_hero = new ElementHeroCard(this);
    lay->addWidget(m_hero);

    auto *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFixedHeight(1);
    line->setStyleSheet("background: rgba(148,163,184,45); border:none;");
    lay->addWidget(line);

    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet("QScrollArea{background:transparent;border:none;} "
                          "QScrollBar:vertical{background:transparent;width:6px;margin:0;} "
                          "QScrollBar::handle:vertical{background:rgba(148,163,184,60);border-radius:3px;min-height:30px;} "
                          "QScrollBar::handle:vertical:hover{background:rgba(148,163,184,110);} "
                          "QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{border:none;background:none;height:0;}");

    auto *inner = new QWidget();
    inner->setObjectName("detailInner");
    m_grid = new QGridLayout(inner);
    m_grid->setContentsMargins(0, 2, 4, 2);
    m_grid->setHorizontalSpacing(12);
    m_grid->setVerticalSpacing(0);
    scroll->setWidget(inner);
    lay->addWidget(scroll, 1);
}

void ElementDetailPanel::clearGrid() {
    QLayoutItem *it;
    while ((it = m_grid->takeAt(0))) {
        if (it->widget()) delete it->widget();
        delete it;
    }
}

void ElementDetailPanel::setElement(const ElementData &e) {
    m_title->setText(QString("%1 · %2").arg(e.nameCn, e.symbol));
    m_subtitle->setText(QString("第 %1 周期  ·  第 %2 族  ·  %3")
                            .arg(e.period).arg(e.group).arg(e.category));
    m_hero->setElement(e);

    clearGrid();
    int row = 0;
    auto add = [&](const QString &k, const QString &v) {
        auto *lk = new QLabel(k, this);
        lk->setObjectName("propKey");
        lk->setStyleSheet("color:#8fa3bd; font-size:12px; background:transparent;");
        lk->setMinimumWidth(60);

        auto *lv = new QLabel(v, this);
        lv->setObjectName("propVal");
        lv->setStyleSheet("color:#e8eef7; font-size:13px; background:transparent;");
        lv->setWordWrap(true);
        lv->setTextInteractionFlags(Qt::TextSelectableByMouse);

        m_grid->addWidget(lk, row, 0, Qt::AlignTop | Qt::AlignLeft);
        m_grid->addWidget(lv, row, 1, Qt::AlignTop);
        m_grid->setRowMinimumHeight(row, 26);
        row++;
    };

    const QString dash = QStringLiteral("—");
    auto num = [&](double v, int prec) {
        return v > 0.0 ? QString::number(v, 'f', prec) : dash;
    };

    add("原子序数", QString::number(e.atomicNumber));
    add("元素符号", e.symbol);
    add("中文名", e.nameCn);
    add("英文名", e.name);
    add("分类", e.category);
    add("原子量", QString::number(e.atomicMass, 'f', 4));
    add("周期 / 族", QString("%1 / %2").arg(e.period).arg(e.group));
    add("区块", e.block.toUpper());
    add("电子构型", e.electronConfig);
    add("电负性", e.electronegativity > 0 ? QString::number(e.electronegativity, 'f', 2) : dash);
    add("密度 (g/cm³)", e.density > 0 ? QString::number(e.density, 'g', 4) : dash);
    add("熔点 (K)", num(e.meltingPoint, 2));
    add("沸点 (K)", num(e.boilingPoint, 2));
    add("常温状态", e.phaseAtSTP == "Solid" ? QStringLiteral("固态")
                          : e.phaseAtSTP == "Liquid" ? QStringLiteral("液态")
                          : e.phaseAtSTP == "Gas" ? QStringLiteral("气态")
                          : QStringLiteral("未知"));
    add("价电子数", QString::number(e.valenceElectrons));
    add("发现年份", e.discovery == "Ancient" ? QStringLiteral("古代已知") : e.discovery);

    m_grid->setColumnStretch(1, 1);
}