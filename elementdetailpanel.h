#ifndef ELEMENTDETAILPANEL_H
#define ELEMENTDETAILPANEL_H
#include <QWidget>
#include "elementdata.h"

class QLabel;
class QGridLayout;

// 详情面板顶部的元素主视觉卡片（大符号 + 名称 + 分类色）
class ElementHeroCard : public QWidget {
    Q_OBJECT
public:
    explicit ElementHeroCard(QWidget *parent = nullptr);
    void setElement(const ElementData &e);
    QSize sizeHint() const override { return QSize(300, 130); }
    QSize minimumSizeHint() const override { return QSize(220, 130); }
protected:
    void paintEvent(QPaintEvent *e) override;
private:
    ElementData m_e;
    bool m_has = false;
};

class ElementDetailPanel : public QWidget {
    Q_OBJECT
public:
    explicit ElementDetailPanel(QWidget *parent=nullptr);
    void setElement(const ElementData &e);
private:
    QLabel *m_title = nullptr;
    QLabel *m_subtitle = nullptr;
    ElementHeroCard *m_hero = nullptr;
    QGridLayout *m_grid = nullptr;
    void clearGrid();
};

#endif