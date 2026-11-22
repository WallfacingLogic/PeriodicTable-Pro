#ifndef ELEMENTDATA_H
#define ELEMENTDATA_H

#include <QString>
#include <QColor>
#include <QVector>

struct ElementData {
    int atomicNumber = 0;
    QString symbol;
    QString name;        // English
    QString nameCn;      // 中文名
    QString category;    // 大类：碱金属 / 碱土金属 / 过渡金属 / 后过渡金属 / 类金属 / 非金属 / 卤素 / 稀有气体 / 镧系 / 锕系
    double atomicMass = 0.0;
    int period = 0;
    int group = 0;
    QString block;       // s/p/d/f
    QString electronConfig;
    double electronegativity = 0.0;
    double density = 0.0;     // g/cm3
    double meltingPoint = 0.0; // K
    double boilingPoint = 0.0; // K
    QString discovery;
    QString phaseAtSTP;       // Solid/Liquid/Gas
    int valenceElectrons = 0;
    QColor color;             // 用于显示的渐变主色
    quint32 categoryIndex = 0; // 0..9，初始化时按 category 计算

    bool isPlaceholder() const { return atomicNumber == 0; }
};

class ElementDatabase {
public:
    // 分类位掩码：bit c 表示分类 c 被选中；10 个分类全选 = 0x3FF
    static constexpr quint32 kAllCategories = 0x3FFu;

    static quint32 categoryCount();
    static const QString &categoryName(quint32 idx);
    static quint32 categoryIndex(const QString &cat);
    static quint32 elementCount();
    static const ElementData &elementAt(quint32 idx);
    static const ElementData *byAtomicNumber(int z);
    static const ElementData *bySymbol(const QString &sym);
    static quint32 indexOf(int z);
    static quint32 indexOfSymbol(const QString &sym);
    static quint32 indexOfName(const QString &n);
    static quint32 indexOfNameCn(const QString &n);
    static quint32 searchFirst(const QString &q, quint32 catIdx);
    static quint32 findNext(quint32 start, const QString &q, quint32 catIdx);
    // 基于位掩码的搜索：只匹配 mask 中选中的分类
    static quint32 searchFirstMask(const QString &q, quint32 catMask);
    static quint32 countInCategory(quint32 catIdx);
    static quint32 elementAtInCategory(quint32 catIdx, quint32 i);
};

#endif // ELEMENTDATA_H