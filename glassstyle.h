#ifndef GLASSSTYLE_H
#define GLASSSTYLE_H

#include <QString>
#include <QWidget>
#include <QPixmap>
#include <QTimer>

// 液态玻璃风格的背景容器：绘制深色渐变 + 光晕，供半透明玻璃面板叠加
class GlassHost : public QWidget {
    Q_OBJECT
public:
    explicit GlassHost(QWidget *parent = nullptr);
protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
private:
    QPixmap m_bgCache;   // 背景只渲染一次，尺寸变化时重建
    QTimer m_resizeTimer;
    bool m_resizing = false;
};

class GlassStyle {
public:
    // Windows 10/11 原生背景材质（Mica / Acrylic）。失败时静默忽略。
    static void enableAcrylic(quintptr winHandle, bool dark = true);
    static void enableMica(quintptr winHandle, bool dark = true);

    // 通用玻璃主题 QSS
    static QString themeStyleSheet();
};

#endif