#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QGraphicsView>
#include "familytree.h"

class TreeScene;
class PersonNodeItem;

/**
 * 家谱树可视化视图（QGraphicsView 自绘，无第三方依赖）
 *
 * 布局算法（叶槽位法 + 重叠修正）：
 *  1. 后序遍历：无子女的成员依次占据叶子槽位（自左向右），
 *     有子女的成员取其子女子树 X 区间的中点；
 *  2. 前序遍历：右移与前一兄弟子树发生重叠的子树，直至间距满足阈值。
 * 夫妻同层并排（[夫]＝[妻]），子女挂下一代；纵坐标 = 代际 * 行距。
 *
 * 交互：滚轮缩放（锚定光标，0.1x~4x）、拖拽平移、单击选中（发出 memberSelected）、
 *       双击居中（发出 memberDoubleClicked）、搜索命中高亮 + 自动居中。
 */
class TreeView : public QGraphicsView
{
    Q_OBJECT
    friend class TestUi;   // 离屏回归测试需直接调用事件处理器

public:
    explicit TreeView(QWidget* parent = nullptr);

    // 依据家谱数据重建场景（数据变更后调用）
    void setData(const FamData* data);
    // 高亮所有包含关键字的节点，并居中到第一个命中
    void highlightMatches(const QString& keyword);
    void clearHighlights();
    // 将指定成员节点居中
    void centerOnNode(const QString& name);
    // 成员节点在视口中的坐标（测试/定位用）
    QPointF viewportPosOf(const QString& name) const;
    // 同行卡片重叠对列表（布局回归测试用，正常应为空）
    QStringList overlappingPairs() const;

signals:
    void memberSelected(const QString& name);
    void memberDoubleClicked(const QString& name);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    TreeScene* m_scene = nullptr;
    bool m_panning = false;          // 是否正在拖拽平移
    QPoint m_pressPos;               // 按下位置（视图坐标）
    QPoint m_panStart;               // 平移起始（滚动条位置）
    QString m_selectedName;          // 当前选中成员
    bool m_firstShow = true;         // 首次显示时重新适配视图
};

#endif // TREEVIEW_H
