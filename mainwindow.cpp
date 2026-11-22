#include "mainwindow.h"
#include "elementdata.h"
#include "glassstyle.h"
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QButtonGroup>
#include <QScrollArea>
#include <QFrame>
#include <QApplication>
#include <QStringList>
#include <QEvent>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("元素周期表 · Periodic Table Pro");
    resize(1460, 820);
    setMinimumSize(720, 480);   // 允许缩小到更小尺寸而不遮挡

    // ---------- central host: 自绘深色玻璃背景 ----------
    auto *central = new GlassHost(this);
    central->setObjectName("centralHost");
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ---------- 自定义标题栏（无边框窗口） ----------
    m_titleBar = new TitleBar(this);
    root->addWidget(m_titleBar);
    connect(m_titleBar, &TitleBar::minimizeClicked, this, &QWidget::showMinimized);
    connect(m_titleBar, &TitleBar::maximizeClicked, this, &MainWindow::onMaximizeClicked);
    connect(m_titleBar, &TitleBar::closeClicked, this, &QWidget::close);

    // 内容区域留边距
    auto *content = new QVBoxLayout();
    content->setContentsMargins(18, 10, 18, 10);
    content->setSpacing(10);
    root->addLayout(content, 1);

    // ---------- top bar（标题已移入自定义标题栏，这里只放搜索） ----------
    auto *top = new QHBoxLayout();
    top->setSpacing(10);
    top->addStretch(1);

    m_search = new QLineEdit(this);
    m_search->setObjectName("searchBox");
    m_search->setPlaceholderText("搜索元素（符号 / 名称 / 原子序数）");
    m_search->setMaximumWidth(420);
    m_search->setMinimumWidth(120);
    m_search->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_search->setClearButtonEnabled(true);
    top->addWidget(m_search, 1);

    auto *btn = new QPushButton("搜索", this);
    btn->setObjectName("searchBtn");
    btn->setCursor(Qt::PointingHandCursor);
    top->addWidget(btn);

    content->addLayout(top);

    // ---------- category chips row（多选，可横向滚动）----------
    auto *chipHost = new QWidget(this);
    chipHost->setObjectName("chipHost");
    auto *chipRow = new QHBoxLayout(chipHost);
    chipRow->setContentsMargins(0, 0, 0, 0);
    chipRow->setSpacing(6);
    m_catChips = new QButtonGroup(this);
    m_catChips->setExclusive(false); // 允许多选

    auto *allChip = new QPushButton("全部", this);
    allChip->setObjectName("catChip");
    allChip->setCheckable(true);
    allChip->setChecked(true);
    allChip->setCursor(Qt::PointingHandCursor);
    m_catChips->addButton(allChip, 99);
    chipRow->addWidget(allChip);

    for (quint32 c = 0; c < ElementDatabase::categoryCount(); c++) {
        auto *chip = new QPushButton(ElementDatabase::categoryName(c), this);
        chip->setObjectName("catChip");
        chip->setCheckable(true);
        chip->setChecked(true); // 默认全选，与"全部"一致
        chip->setCursor(Qt::PointingHandCursor);
        m_catChips->addButton(chip, (int)c);
        chipRow->addWidget(chip);
    }
    chipRow->addStretch(1);

    auto *chipScroll = new QScrollArea(this);
    chipScroll->setObjectName("chipScroll");
    chipScroll->setWidget(chipHost);
    chipScroll->setWidgetResizable(true);
    chipScroll->setFrameShape(QFrame::NoFrame);
    chipScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    chipScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    chipScroll->setFixedHeight(38);
    chipScroll->setStyleSheet(
        "QScrollArea#chipScroll{background:transparent;border:none;}"
        "QScrollBar:horizontal{background:transparent;height:0px;}"
        "QScrollBar::handle:horizontal{background:transparent;}");
    content->addWidget(chipScroll);

    connect(m_catChips, QOverload<int, bool>::of(&QButtonGroup::idToggled),
            this, &MainWindow::onCatChipToggled);

    // ---------- main split: table + detail ----------
    auto *split = new QSplitter(Qt::Horizontal, this);
    m_table = new PeriodicTableWidget(this);
    m_detail = new ElementDetailPanel(this);
    split->addWidget(m_table);
    split->addWidget(m_detail);
    // 两侧按比例伸缩：表格占约 76%，详情占约 24%，窗口变化时同步缩放
    split->setStretchFactor(0, 3);
    split->setStretchFactor(1, 1);
    split->setCollapsible(0, false);
    split->setCollapsible(1, false);
    content->addWidget(split, 1);

    setCentralWidget(central);
    setStyleSheet(GlassStyle::themeStyleSheet());

    // ---------- status bar ----------
    m_status = new QLabel(this);
    m_status->setObjectName("statusLabel");
    m_status->setText("元素总数 118  ·  点击元素查看详情");
    statusBar()->addWidget(m_status);
    statusBar()->setSizeGripEnabled(false);

    // ---------- connections ----------
    m_searchDebounce.setSingleShot(true);
    connect(&m_searchDebounce, &QTimer::timeout, this, &MainWindow::doSearch);
    connect(m_table, &PeriodicTableWidget::elementSelected, this, &MainWindow::onElementSelected);
    connect(btn, &QPushButton::clicked, this, &MainWindow::onSearch);
    connect(m_search, &QLineEdit::returnPressed, this, &MainWindow::onSearch);
    connect(m_search, &QLineEdit::textChanged, this, &MainWindow::onSearch);

    // 性能优先：不启用 DWM Acrylic（每帧整窗模糊合成是卡顿主因），
    // 背景由 GlassHost 自绘并缓存。如需原生材质可改调 GlassStyle::enableMica。
#ifdef Q_OS_WIN
    // GlassStyle::enableMica((quintptr)winId(), true);
#endif

    // ---------- 无边框窗口 ----------
    // 隐藏原生标题栏，保留系统菜单（任务栏/右键菜单行为正常）。
    // Windows 上用 DWM 扩展 1px 可缩放边框：既有自绘标题栏，也保留边缘缩放与贴靠。
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
#ifdef Q_OS_WIN
    HWND hwnd = (HWND)winId();
    // 去掉原生标题栏样式位，只保留可缩放边框
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    style &= ~WS_CAPTION;
    style |= WS_THICKFRAME;
    SetWindowLongPtr(hwnd, GWL_STYLE, style);
    // DWM 扩展 1px 边框：保留阴影/贴靠与边缘缩放
    MARGINS margins = {0, 0, 1, 0};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
#endif

    // default select Hydrogen
    onElementSelected(1);
}

void MainWindow::onMaximizeClicked() {
    if (isMaximized()) showNormal();
    else showMaximized();
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    if (m_titleBar) m_titleBar->setMaximized(isMaximized());
}

#ifdef Q_OS_WIN
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    Q_UNUSED(eventType)
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message != WM_NCHITTEST || isMaximized()) return false;

    const int border = 8; // 可拖拽缩放的边缘宽度
    const POINT pt = msg->pt;
    RECT wr;
    GetWindowRect((HWND)winId(), &wr);
    const int x = (int)(pt.x - wr.left);
    const int y = (int)(pt.y - wr.top);
    const int w = (int)(wr.right - wr.left);
    const int h = (int)(wr.bottom - wr.top);

    bool left = x < border, right = x > w - border;
    bool top = y < border, bottom = y > h - border;

    LRESULT hit = HTCLIENT;
    if (left && top) hit = HTTOPLEFT;
    else if (right && top) hit = HTTOPRIGHT;
    else if (left && bottom) hit = HTBOTTOMLEFT;
    else if (right && bottom) hit = HTBOTTOMRIGHT;
    else if (left) hit = HTLEFT;
    else if (right) hit = HTRIGHT;
    else if (top) hit = HTTOP;
    else if (bottom) hit = HTBOTTOM;

    if (hit != HTCLIENT) { *result = hit; return true; }
    return false;
}
#endif

quint32 MainWindow::currentMask() const { return m_catMask; }

void MainWindow::demoHover(int z) {
    if (m_table) m_table->demoHover(z);
}

void MainWindow::onElementSelected(int z) {
    if (const ElementData *e = ElementDatabase::byAtomicNumber(z)) {
        m_detail->setElement(*e);
        if (m_status)
            m_status->setText(QString("已选 %1 (%2)  ·  Z=%3  ·  %4")
                              .arg(e->nameCn, e->symbol, QString::number(e->atomicNumber), e->category));
    }
}

void MainWindow::onSearch() {
    // textChanged 每键一次都全量扫描 + 重建缓存；加 120ms 防抖
    m_searchDebounce.start(120);
}

void MainWindow::doSearch() {
    const QString q = m_search->text().trimmed().toLower();
    // 搜索在当前选中的分类掩码范围内进行
    const quint32 idx = ElementDatabase::searchFirstMask(q, m_catMask);
    if (idx < ElementDatabase::elementCount()) {
        const ElementData &e = ElementDatabase::elementAt(idx);
        onElementSelected(e.atomicNumber);
    }
}

void MainWindow::onCatChipToggled(int id, bool checked) {
    if (updatingChips) return;
    updatingChips = true;

    if (id == 99) {
        // "全部"：勾上则全选所有分类；取消则清空所有分类
        const quint32 newMask = checked ? ElementDatabase::kAllCategories : 0u;
        m_catMask = newMask;
        for (auto *b : m_catChips->buttons()) {
            const int bid = m_catChips->id(b);
            if (bid == 99) continue;
            b->setChecked(checked);
        }
    } else {
        // 单个分类：按位切换
        const quint32 bit = quint32(1) << id;
        if (checked) m_catMask |= bit;
        else m_catMask &= ~bit;

        // 无分类选中时，视为"全部"以给出合理视图
        if (m_catMask == 0) m_catMask = ElementDatabase::kAllCategories;

        // 同步各分类 chip 与"全部"的勾选状态
        for (auto *b : m_catChips->buttons()) {
            const int bid = m_catChips->id(b);
            if (bid == 99) {
                b->setChecked(m_catMask == ElementDatabase::kAllCategories);
            } else {
                b->setChecked((m_catMask & (quint32(1) << bid)) != 0);
            }
        }
    }

    updatingChips = false;

    // 应用到周期表（变暗未选分类）
    m_table->setCategoryFilter(m_catMask);

    // 状态栏显示当前筛选信息
    if (m_status) {
        if (m_catMask == ElementDatabase::kAllCategories) {
            m_status->setText("元素总数 118  ·  点击元素查看详情");
        } else {
            int total = 0;
            QStringList names;
            for (quint32 c = 0; c < ElementDatabase::categoryCount(); c++) {
                if (m_catMask & (quint32(1) << c)) {
                    total += (int)ElementDatabase::countInCategory(c);
                    names << ElementDatabase::categoryName(c);
                }
            }
            m_status->setText(QString("已选分类: %1  ·  %2 个元素")
                              .arg(names.join("、"))
                              .arg(total));
        }
    }

    // 分类变化后，用当前搜索词重新定位
    doSearch();
}