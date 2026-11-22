#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTimer>
#include "periodictablewidget.h"
#include "elementdetailpanel.h"
#include "titlebar.h"

class QLineEdit;
class QComboBox;
class QButtonGroup;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
    void demoHover(int z); // 开发自检：模拟悬停
private slots:
    void onElementSelected(int z);
    void onSearch();
    void doSearch();
    void onCatChipToggled(int id, bool checked);
    void onMaximizeClicked();
private:
    TitleBar *m_titleBar = nullptr;
    bool m_wasMaximized = false;
protected:
    void changeEvent(QEvent *e) override;
#ifdef Q_OS_WIN
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#endif
private:
    PeriodicTableWidget *m_table = nullptr;
    ElementDetailPanel *m_detail = nullptr;
    QLineEdit *m_search = nullptr;
    QButtonGroup *m_catChips = nullptr;
    QLabel *m_status = nullptr;
    QTimer m_searchDebounce;   // 搜索防抖，避免每键一次都重建缓存
    // 多选分类状态：bit c = 分类 c 选中；id 99 代表"全部"
    quint32 m_catMask = ElementDatabase::kAllCategories;
    bool updatingChips = false; // 防止程序化设置 checked 时递归
    quint32 currentMask() const;
};

#endif