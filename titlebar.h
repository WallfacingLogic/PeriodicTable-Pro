#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QPoint>
#include <QPushButton>
#include <QEnterEvent>

// 自绘的窗口控制按钮（最小化 / 最大化·还原 / 关闭），矢量绘制，任意 DPI 清晰
class TitleButton : public QPushButton {
    Q_OBJECT
public:
    enum Kind { Minimize, Maximize, Close };
    explicit TitleButton(Kind kind, QWidget *parent = nullptr);
    void setRestorable(bool r);
protected:
    void paintEvent(QPaintEvent *) override;
    void enterEvent(QEnterEvent *) override;
    void leaveEvent(QEvent *) override;
private:
    Kind m_kind;
    bool m_restorable = false;
    bool m_hovered = false;
};

// 自定义标题栏：左侧图标+标题+副标题，右侧窗口按钮；空白处可拖动，双击最大化
class TitleBar : public QWidget {
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);
    void setMaximized(bool maximized);
signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();
protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
private:
    TitleButton *m_min = nullptr;
    TitleButton *m_max = nullptr;
    TitleButton *m_close = nullptr;
    QPoint m_dragPos;      // 用于无 native 支持时的回退拖动
    bool m_dragging = false;
};

#endif // TITLEBAR_H