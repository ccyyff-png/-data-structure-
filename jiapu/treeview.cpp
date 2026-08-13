#include "treeview.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsPathItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <cmath>
#include <map>
#include <vector>

// ---------------- 布局常量 ----------------
namespace {
constexpr double NODE_W   = 120.0;   // 节点卡片宽
constexpr double NODE_H   = 44.0;    // 节点卡片高
constexpr double COUPLE_GAP = 16.0;  // 夫妻卡片间距
constexpr double LEAF_GAP = 150.0;   // 相邻叶子槽位间距
constexpr double V_GAP    = 100.0;   // 行距（代际间距）
constexpr double MIN_SUBTREE_GAP = 24.0;  // 兄弟子树最小间距
constexpr double MARGIN   = 50.0;    // 场景四周留白

const QColor MALE_COLOR(0x2a, 0x78, 0xd6);      // 男：蓝
const QColor FEMALE_COLOR(0xe8, 0x7b, 0xa4);    // 女：粉
const QColor MALE_BORDER(0x10, 0x42, 0x81);
const QColor FEMALE_BORDER(0xc7, 0x4d, 0x85);
const QColor EDGE_COLOR(0x94, 0xa3, 0xb8);      // 连线灰
const QColor SELECT_COLOR(0xed, 0xa1, 0x00);    // 选中橙
const QColor HIGHLIGHT_COLOR(0xeb, 0x68, 0x34); // 搜索命中橙红
const QColor BG_COLOR(0xfc, 0xfc, 0xfb);        // 场景底色
}

// ---------------- 节点卡片 ----------------

class PersonNodeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    PersonNodeItem(const MemberInfo& m, QGraphicsItem* parent = nullptr)
        : QGraphicsObject(parent)
        , m_name(m.name)
        , m_male(m.male)
        , m_generation(m.generation)
    {
        setFlag(ItemIsSelectable, false);
        QString tip = QStringLiteral("%1（%2）\n第 %3 代")
                          .arg(m.name, m.male ? "男" : "女")
                          .arg(m.generation + 1);
        if (!m.spouse.isEmpty())
            tip += QStringLiteral("\n配偶：%1").arg(m.spouse);
        if (!m.father.isEmpty())
            tip += QStringLiteral("\n父亲：%1    母亲：%2").arg(m.father, m.mother);
        tip += QStringLiteral("\n子女：%1 人").arg(m.children.size());
        setToolTip(tip);
    }

    QRectF boundingRect() const override
    {
        return QRectF(-NODE_W / 2 - 6, -NODE_H / 2 - 6, NODE_W + 12, NODE_H + 12);
    }

    void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) override
    {
        const QRectF card(-NODE_W / 2, -NODE_H / 2, NODE_W, NODE_H);
        // 选中/搜索命中的外发光
        if (m_selected || m_searchHighlight) {
            p->setPen(Qt::NoPen);
            p->setBrush(QColor(m_selected ? SELECT_COLOR : HIGHLIGHT_COLOR));
            p->setOpacity(0.22);
            p->drawRoundedRect(card.adjusted(-6, -6, 6, 6), 12, 12);
            p->setOpacity(1.0);
        }
        // 卡片主体
        p->setPen(QPen(m_selected ? SELECT_COLOR
                       : (m_searchHighlight ? HIGHLIGHT_COLOR
                          : (m_male ? MALE_BORDER : FEMALE_BORDER)),
                       m_selected || m_searchHighlight ? 3.0 : 1.5));
        p->setBrush(m_male ? MALE_COLOR : FEMALE_COLOR);
        p->drawRoundedRect(card, 8, 8);
        // 姓名（白色，居中）
        QFont f = p->font();
        f.setPixelSize(14);
        f.setBold(true);
        p->setFont(f);
        p->setPen(Qt::white);
        p->drawText(card.adjusted(4, -4, -4, -8), Qt::AlignCenter, m_name);
        // 代际角标（右下角小字）
        QFont badge = p->font();
        badge.setPixelSize(9);
        badge.setBold(false);
        p->setFont(badge);
        p->setPen(QColor(255, 255, 255, 200));
        p->drawText(card.adjusted(0, 0, -4, -4), Qt::AlignRight | Qt::AlignBottom,
                    QStringLiteral("第%1代").arg(m_generation + 1));
    }

    QString name() const { return m_name; }

    void setSearchHighlight(bool on) { m_searchHighlight = on; update(); }
    void setSelectedVisual(bool on)   { m_selected = on; update(); }

signals:
    void clicked(const QString& name);
    void doubleClicked(const QString& name);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override
    {
        QGraphicsObject::mousePressEvent(event);
        if (event->button() == Qt::LeftButton)
            emit clicked(m_name);
    }
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override
    {
        QGraphicsObject::mouseDoubleClickEvent(event);
        if (event->button() == Qt::LeftButton)
            emit doubleClicked(m_name);
    }

private:
    QString m_name;
    bool m_male;
    int m_generation;
    bool m_searchHighlight = false;
    bool m_selected = false;
};

// ---------------- 场景与布局 ----------------

class TreeScene : public QGraphicsScene
{
    Q_OBJECT
public:
    // 家庭单元（男性成员 + 配偶 + 子单元）
    struct Unit
    {
        QString name;
        QString wife;                    // 配偶姓名（无配偶为空）
        std::vector<Unit*> children;
        double x = 0;                    // 夫妻组中心 X
        double minX = 0, maxX = 0;       // 子树包围盒
        int generation = 0;
        PersonNodeItem* husbandItem = nullptr;
        PersonNodeItem* wifeItem = nullptr;
    };

    explicit TreeScene(QObject* parent = nullptr) : QGraphicsScene(parent) {}

    void rebuild(const FamData& data)
    {
        clear();
        m_units.clear();
        m_husbandItems.clear();
        m_wifeItems.clear();
        m_slot = 0;

        if (data.members.empty())
            return;

        // 构建家庭单元森林（自根男性开始）
        std::map<QString, Unit*> unitOf;
        std::function<void(const QString&)> buildUnit = [&](const QString& name) {
            if (unitOf.count(name))
                return;
            auto* u = new Unit;
            u->name = name;
            u->generation = data.members.at(name).generation;
            const MemberInfo& m = data.members.at(name);
            u->wife = (m.spouse.isEmpty() || !data.members.count(m.spouse)) ? QString() : m.spouse;
            unitOf[name] = u;
            m_units.push_back(u);
            for (const QString& c : m.children) {
                buildUnit(c);
                u->children.push_back(unitOf[c]);
            }
        };
        buildUnit(data.rootName);

        // 布局：后序叶槽位 → 前序重叠修正
        std::function<void(Unit*)> layout = [&](Unit* u) {
            if (u->children.empty()) {
                u->x = m_slot++ * LEAF_GAP;
            } else {
                for (Unit* c : u->children)
                    layout(c);
                u->x = (u->children.front()->x + u->children.back()->x) / 2.0;
            }
            const double half = u->wife.isEmpty() ? NODE_W / 2 : NODE_W + COUPLE_GAP / 2;
            double minX = u->x - half, maxX = u->x + half;
            for (Unit* c : u->children) {
                minX = std::min(minX, c->minX);
                maxX = std::max(maxX, c->maxX);
            }
            u->minX = minX;
            u->maxX = maxX;
        };
        std::function<void(Unit*, double)> shift = [&](Unit* u, double dx) {
            u->x += dx;
            u->minX += dx;
            u->maxX += dx;
            for (Unit* c : u->children)
                shift(c, dx);
        };
        std::function<void(Unit*)> fixOverlaps = [&](Unit* u) {
            double rightmost = -1e18;
            for (Unit* c : u->children) {
                if (rightmost + MIN_SUBTREE_GAP > c->minX)
                    shift(c, rightmost + MIN_SUBTREE_GAP - c->minX);
                rightmost = std::max(rightmost, c->maxX);
                fixOverlaps(c);
            }
        };
        Unit* rootUnit = unitOf[data.rootName];
        layout(rootUnit);
        fixOverlaps(rootUnit);

        // 创建节点与连线
        std::function<void(Unit*)> place = [&](Unit* u) {
            const double y = MARGIN + u->generation * V_GAP;
            const MemberInfo& m = data.members.at(u->name);
            auto* hItem = new PersonNodeItem(m);
            hItem->setPos(u->x, y);
            addItem(hItem);
            m_husbandItems[u->name] = hItem;
            connect(hItem, &PersonNodeItem::clicked, this, &TreeScene::nodeClicked);
            connect(hItem, &PersonNodeItem::doubleClicked, this, &TreeScene::nodeDoubleClicked);
            u->husbandItem = hItem;

            if (!u->wife.isEmpty()) {
                const MemberInfo& w = data.members.at(u->wife);
                auto* wItem = new PersonNodeItem(w);
                wItem->setPos(u->x + NODE_W / 2 + COUPLE_GAP + NODE_W / 2, y);
                addItem(wItem);
                m_wifeItems[u->wife] = wItem;
                connect(wItem, &PersonNodeItem::clicked, this, &TreeScene::nodeClicked);
                connect(wItem, &PersonNodeItem::doubleClicked, this, &TreeScene::nodeDoubleClicked);
                u->wifeItem = wItem;
                // 婚姻连线（夫卡右缘 —— 妻卡左缘，中点小圆）
                auto* marriage = new QGraphicsLineItem(
                    QLineF(NODE_W / 2, 0, NODE_W / 2 + COUPLE_GAP, 0));
                marriage->setParentItem(hItem);
                marriage->setPen(QPen(EDGE_COLOR, 1.5));
                marriage->setZValue(-1);
                auto* ring = addEllipse(-2.5, -2.5, 5, 5, Qt::NoPen, QBrush(EDGE_COLOR));
                ring->setParentItem(hItem);
                ring->setPos(NODE_W / 2 + COUPLE_GAP / 2, 0);
            }

            if (!u->children.empty()) {
                // 亲子连线：父组底中点 → 竖直 → 水平母线 → 竖直 → 子卡顶
                const double coupleCenterX = u->wife.isEmpty() ? u->x : u->x + NODE_W / 2 + COUPLE_GAP / 2;
                const double parentBottom = y + NODE_H / 2;
                const double childTop = parentBottom + V_GAP - NODE_H / 2;
                const double railY = parentBottom + (childTop - parentBottom) / 2.0;
                QPainterPath path(QPointF(coupleCenterX, parentBottom));
                path.lineTo(coupleCenterX, railY);
                double firstX = 1e18, lastX = -1e18;
                for (Unit* c : u->children) {
                    const double cx = c->wife.isEmpty() ? c->x : c->x + NODE_W / 2 + COUPLE_GAP / 2;
                    firstX = std::min(firstX, cx);
                    lastX = std::max(lastX, cx);
                }
                path.lineTo(lastX, railY);
                for (Unit* c : u->children) {
                    const double cx = c->wife.isEmpty() ? c->x : c->x + NODE_W / 2 + COUPLE_GAP / 2;
                    QPainterPath branch(QPointF(cx, railY));
                    branch.lineTo(cx, childTop);
                    auto* edge = addPath(branch, QPen(EDGE_COLOR, 1.5));
                    edge->setZValue(-1);
                }
                // 水平母线从 firstX 画到 lastX 补全（先画竖直主线和各分支竖线）
                auto* rail = addLine(QLineF(firstX, railY, lastX, railY), QPen(EDGE_COLOR, 1.5));
                rail->setZValue(-1);
                auto* trunk = addPath(path, QPen(EDGE_COLOR, 1.5));
                trunk->setZValue(-1);
            }

            for (Unit* c : u->children)
                place(c);
        };
        place(rootUnit);

        // 背景与场景范围
        auto* bg = addRect(rootUnit->minX - MARGIN, 0, rootUnit->maxX - rootUnit->minX + 2 * MARGIN,
                           MARGIN + (data.maxGeneration + 1) * V_GAP + MARGIN,
                           QPen(Qt::NoPen), QBrush(BG_COLOR));
        bg->setZValue(-2);
        setSceneRect(bg->rect());
    }

    void setNodeHighlight(const QString& name, bool on)
    {
        if (auto it = m_husbandItems.find(name); it != m_husbandItems.end())
            it->second->setSearchHighlight(on);
        if (auto it = m_wifeItems.find(name); it != m_wifeItems.end())
            it->second->setSearchHighlight(on);
    }

    void clearHighlights()
    {
        for (auto& [name, item] : m_husbandItems)
            item->setSearchHighlight(false);
        for (auto& [name, item] : m_wifeItems)
            item->setSearchHighlight(false);
    }

    // 高亮所有包含关键字的节点，返回命中名单（按代际+姓名字典序）
    QStringList highlightMatches(const QString& keyword)
    {
        clearHighlights();
        QStringList hits;
        if (keyword.isEmpty())
            return hits;
        for (auto& [name, item] : m_husbandItems) {
            if (name.contains(keyword)) {
                item->setSearchHighlight(true);
                hits << name;
            }
        }
        for (auto& [name, item] : m_wifeItems) {
            if (name.contains(keyword)) {
                item->setSearchHighlight(true);
                hits << name;
            }
        }
        std::sort(hits.begin(), hits.end());
        return hits;
    }

    void setSelected(const QString& name)
    {
        if (auto it = m_husbandItems.find(m_selectedName); it != m_husbandItems.end())
            it->second->setSelectedVisual(false);
        if (auto it = m_wifeItems.find(m_selectedName); it != m_wifeItems.end())
            it->second->setSelectedVisual(false);
        m_selectedName = name;
        if (auto it = m_husbandItems.find(name); it != m_husbandItems.end())
            it->second->setSelectedVisual(true);
        if (auto it = m_wifeItems.find(name); it != m_wifeItems.end())
            it->second->setSelectedVisual(true);
    }

    QPointF nodePos(const QString& name) const
    {
        if (auto it = m_husbandItems.find(name); it != m_husbandItems.end())
            return it->second->pos();
        if (auto it = m_wifeItems.find(name); it != m_wifeItems.end())
            return it->second->pos();
        return {};
    }

signals:
    void memberClicked(const QString& name);
    void memberDoubleClicked(const QString& name);

private slots:
    void nodeClicked(const QString& name) { emit memberClicked(name); }
    void nodeDoubleClicked(const QString& name) { emit memberDoubleClicked(name); }

private:
    std::vector<Unit*> m_units;
    std::map<QString, PersonNodeItem*> m_husbandItems;
    std::map<QString, PersonNodeItem*> m_wifeItems;
    int m_slot = 0;
    QString m_selectedName;
};

// ---------------- 视图 ----------------

TreeView::TreeView(QWidget* parent)
    : QGraphicsView(parent)
    , m_scene(new TreeScene(this))
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::NoDrag);
    setFrameShape(QFrame::NoFrame);
    setBackgroundBrush(BG_COLOR);
    connect(m_scene, &TreeScene::memberClicked, this, &TreeView::memberSelected);
    connect(m_scene, &TreeScene::memberDoubleClicked, this, &TreeView::memberDoubleClicked);
}

void TreeView::setData(const FamData* data)
{
    if (!data)
        return;
    m_scene->rebuild(*data);
    m_selectedName.clear();
    fitInView(m_scene->sceneRect().adjusted(-30, -30, 30, 30), Qt::KeepAspectRatio);
}

void TreeView::highlightMatches(const QString& keyword)
{
    const QStringList hits = m_scene->highlightMatches(keyword);
    if (!hits.isEmpty())
        centerOnNode(hits.first());
}

void TreeView::clearHighlights()
{
    m_scene->clearHighlights();
}

void TreeView::centerOnNode(const QString& name)
{
    const QPointF pos = m_scene->nodePos(name);
    if (!pos.isNull())
        centerOn(pos);
}

void TreeView::showEvent(QShowEvent* event)
{
    QGraphicsView::showEvent(event);
    // 构造时视口尺寸尚未确定，首次显示时按真实视口重新适配整棵树
    if (m_firstShow && m_scene && m_scene->sceneRect().width() > 0) {
        m_firstShow = false;
        fitInView(m_scene->sceneRect().adjusted(-30, -30, 30, 30), Qt::KeepAspectRatio);
    }
}

void TreeView::wheelEvent(QWheelEvent* event)
{
    const double factor = event->angleDelta().y() > 0 ? 1.2 : 1.0 / 1.2;
    const double current = transform().m11();
    const double next = current * factor;
    if (next < 0.1 || next > 4.0)
        return;
    scale(factor, factor);
}

void TreeView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_pressPos = event->position().toPoint();
        m_panStart = QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value());
        m_panning = false;
    }
    QGraphicsView::mousePressEvent(event);
}

void TreeView::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        const QPoint delta = m_pressPos - event->position().toPoint();
        if (!m_panning && (std::abs(delta.x()) > 4 || std::abs(delta.y()) > 4))
            m_panning = true;
        if (m_panning) {
            horizontalScrollBar()->setValue(m_panStart.x() + delta.x());
            verticalScrollBar()->setValue(m_panStart.y() + delta.y());
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void TreeView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !m_panning) {
        // 点击（未拖动）：选中节点
        QGraphicsItem* it = itemAt(event->position().toPoint());
        auto* node = qgraphicsitem_cast<PersonNodeItem*>(it);
        if (node) {
            const QString name = node->name();
            m_selectedName = name;
            m_scene->setSelected(name);
            emit memberSelected(name);
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void TreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QGraphicsItem* it = itemAt(event->position().toPoint());
    if (auto* node = qgraphicsitem_cast<PersonNodeItem*>(it)) {
        const QString name = node->name();
        centerOnNode(name);
        emit memberDoubleClicked(name);
    }
    QGraphicsView::mouseDoubleClickEvent(event);
}

#include "treeview.moc"
