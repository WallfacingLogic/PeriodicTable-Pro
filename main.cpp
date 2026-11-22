#include <QApplication>
#include <QIcon>
#include <QPixmap>
#include <QTimer>
#include <QStringList>
#include "mainwindow.h"

int main(int argc, char **argv) {
    QApplication a(argc, argv);
    a.setStyle("Fusion");
    a.setWindowIcon(QIcon(":/resources/PeriodicTable.png"));
    a.setApplicationName("PeriodicTable Pro");
    a.setApplicationVersion("1.0");

    MainWindow w;
    w.setWindowIcon(QIcon(":/resources/PeriodicTable.png"));
    const QStringList args = a.arguments();
    // 调试：--size WxH 指定窗口尺寸，验证不同尺寸下的布局
    {
        const int si = args.indexOf("--size");
        if (si >= 0 && si + 1 < args.size()) {
            const QString wh = args.at(si + 1);
            const int x = wh.indexOf('x');
            if (x > 0) {
                bool ok1 = false, ok2 = false;
                int ww = wh.left(x).toInt(&ok1), hh = wh.mid(x + 1).toInt(&ok2);
                if (ok1 && ok2) w.resize(ww, hh);
            }
        }
    }
    w.show();

    // 开发自检：--screenshot <path> [--hover Z] 渲染一帧后退出
    const int idx = args.indexOf("--screenshot");
    if (idx >= 0 && idx + 1 < args.size()) {
        const QString out = args.at(idx + 1);
        int hoverZ = 0;
        const int hidx = args.indexOf("--hover");
        if (hidx >= 0 && hidx + 1 < args.size()) hoverZ = args.at(hidx + 1).toInt();
        // --resize-to WxH：先渲染大窗，再缩到目标尺寸并二次渲染，验证动态缩放
        QString resizeTo;
        const int ridx = args.indexOf("--resize-to");
        if (ridx >= 0 && ridx + 1 < args.size()) resizeTo = args.at(ridx + 1);

        // --sweep：模拟鼠标连续滑过多个格子，验证悬停残影/白线
        bool sweep = args.contains("--sweep");
        bool midResize = args.contains("--mid-resize");

        QTimer::singleShot(700, &w, [&w, out, hoverZ, resizeTo, sweep, midResize]() {
            if (sweep) {
                // 依次悬停多个元素，模拟快速滑动（都触发脏矩形刷新）
                for (int z : {1, 3, 11, 19, 37, 55, 87, 6, 14, 32, 50, 82}) {
                    w.demoHover(z);
                    QApplication::processEvents();
                }
                w.grab().save(out);
                QCoreApplication::quit();
                return;
            }
            if (!resizeTo.isEmpty()) {
                const int x = resizeTo.indexOf('x');
                if (x > 0) {
                    bool ok1 = false, ok2 = false;
                    w.resize(resizeTo.left(x).toInt(&ok1), resizeTo.mid(x + 1).toInt(&ok2));
                }
                // --mid-resize：在精绘定时器(180ms)触发前抓取缩放中间帧
                if (midResize) {
                    QTimer::singleShot(50, &w, [&w, out]() {
                        w.grab().save(out);
                        QCoreApplication::quit();
                    });
                } else {
                // 缩窗后再等两帧，让 resizeEvent/paintEvent 稳定
                QTimer::singleShot(600, &w, [&w, out, hoverZ]() {
                    if (hoverZ > 0) w.demoHover(hoverZ);
                    w.grab().save(out);
                    QCoreApplication::quit();
                });
                }
            } else {
                if (hoverZ > 0) w.demoHover(hoverZ);
                w.grab().save(out);
                QCoreApplication::quit();
            }
        });
    }

    return a.exec();
}