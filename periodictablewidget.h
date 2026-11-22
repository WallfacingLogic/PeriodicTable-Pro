#ifndef PERIODICTABLEWIDGET_H
#define PERIODICTABLEWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPixmap>
#include <QTimer>
#include "elementdata.h"

class PeriodicTableWidget : public QWidget {
    Q_OBJECT
public:
    explicit PeriodicTableWidget(QWidget *parent = nullptr);
    void setElements(const QVector<ElementData> &els);
    void setCategoryFilter(quint32 catMask);
    void demoHover(int z);
    QSize sizeHint() const override;
signals:
    void elementSelected(int atomicNumber);
protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
private:
    struct Cell {
        QRect rect;
        int z = 0;
        quint32 elementIndex = 0;
        bool placeholder = false;
        QString placeholderText;
    };
    QVector<Cell> m_cells;
    QVector<int> m_hitGrid;       // 18x10 O(1) 命中查表
    int m_side = 40;
    int m_startX = 0;
    int m_startY = 0;
    int m_hoverZ = 0;
    int m_selectedZ = 0;
    quint32 m_catMask = ElementDatabase::kAllCategories;

    // 两张严格 1:1 的静态缓存：
    //  m_bgPix   — 背景渐变/光晕
    //  m_tablePix— 标签 + 所有格子 + 图例（不含悬停）
    // 悬停态永不写入缓存；每次刷新都从缓存按脏区 1:1 blit 覆盖旧痕迹，
    // 从数学上杜绝"缩放拉伸"与"抗锯齿残影累积"。
    QPixmap m_bgPix;
    QSize m_bgLogical;
    bool m_bgDirty = true;
    QPixmap m_tablePix;
    QSize m_tableLogical;
    bool m_tableDirty = true;
    // 最近一帧完整合成画面（背景+表格）。缩放期间直接把它拉伸 blit，
    // 保留玻璃/圆角/抗锯齿全部质感，且每帧只有一次 GPU 纹理缩放，极快。
    QPixmap m_framePix;
    void ensureBackground();
    void ensureTableCache();
    void ensureFrame();

    // 缩放期间走快速路径：跳过文字/抗锯齿与背景光晕重建，
    // 缩放停止后延迟精绘，实现丝滑无延迟拖动。
    QTimer m_resizeTimer;
    bool m_resizing = false;
    void beginResizing();

    // 悬停锚定区域（脏矩形用；浮卡锚定到格子，不跟随鼠标）
    QRect m_lastHoverRect;
    QRect m_lastCardRect;

    void rebuildLayout();
    void drawCell(QPainter &p, const Cell &cell, bool hover, bool sel) const;
    void drawLegend(QPainter &p) const;
    void drawAxisLabels(QPainter &p) const;
    int hitTest(const QPoint &p) const;
    QRect hoverCellRect(int z) const;
    QRect hoverCardRect(const QRect &cellRect) const;
    void paintHoverCard(QPainter &p, const QRect &r) const;
    void refreshHover(int z);

    static constexpr int MAX_CELL = 76; // 格子边长上限，保证 18 列始终完整可见
};

#endif