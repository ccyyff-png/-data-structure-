#include "chartwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <algorithm>

// ---------------- 统一配色 ----------------
namespace {
const QColor SURFACE(0xfc, 0xfc, 0xfb);      // 底
const QColor INK(0x0b, 0x0b, 0x0b);          // 主墨色
const QColor INK_SECONDARY(0x52, 0x51, 0x4e); // 辅墨色
const QColor INK_MUTED(0x89, 0x87, 0x81);     // 弱墨色
const QColor GRID(0xe1, 0xe0, 0xd9);          // 网格线
const QColor AXIS(0xc3, 0xc2, 0xb7);          // 轴线
const QColor BAR_BLUE(0x2a, 0x78, 0xd6);
const QFont& baseFont()
{
    static const QFont f("Microsoft YaHei UI", 9);
    return f;
}
}

// ---------------- BarChartWidget ----------------

BarChartWidget::BarChartWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(200);
}

void BarChartWidget::setData(const QString& title, const QStringList& labels,
                             const QVector<double>& values, const QColor& barColor)
{
    m_title = title;
    m_labels = labels;
    m_values = values;
    m_barColor = barColor;
    m_maxValue = 0;
    for (double v : values)
        m_maxValue = std::max(m_maxValue, v);
    if (m_maxValue <= 0)
        m_maxValue = 1;
    update();
}

void BarChartWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF card = rect().adjusted(1, 1, -1, -1);
    // 底色与边框
    p.setPen(QPen(GRID, 1));
    p.setBrush(SURFACE);
    p.drawRoundedRect(card, 8, 8);

    // 标题
    p.setFont(QFont("Microsoft YaHei UI", 10, QFont::Bold));
    p.setPen(INK);
    p.drawText(QRectF(card.left() + 14, card.top() + 8, card.width() - 28, 22),
               Qt::AlignLeft | Qt::AlignVCenter, m_title);

    if (m_values.isEmpty())
        return;

    // 绘图区
    const QRectF plot(card.left() + 52, card.top() + 42,
                      card.width() - 52 - 14, card.height() - 42 - 34);
    // 网格与纵轴刻度（4 等分）
    p.setFont(baseFont());
    for (int i = 0; i <= 4; ++i) {
        const double y = plot.bottom() - plot.height() * i / 4.0;
        p.setPen(QPen(i == 4 ? AXIS : GRID, 1));
        p.drawLine(QPointF(plot.left(), y), QPointF(plot.right(), y));
        const double val = m_maxValue * i / 4.0;
        p.setPen(INK_SECONDARY);
        p.drawText(QRectF(card.left() + 6, y - 9, 40, 18),
                   Qt::AlignRight | Qt::AlignVCenter, QString::number(qRound(val)));
    }

    // 柱体
    const int n = m_values.size();
    const double slotW = plot.width() / n;
    const double barW = std::min(46.0, slotW * 0.55);
    for (int i = 0; i < n; ++i) {
        const double cx = plot.left() + slotW * (i + 0.5);
        const double h = plot.height() * m_values[i] / m_maxValue;
        const QRectF bar(cx - barW / 2, plot.bottom() - h, barW, h);
        if (bar.height() > 2) {
            QPainterPath path;
            path.addRoundedRect(bar, 3, 3);
            p.fillPath(path, m_barColor);
        }
        // 数值标签
        p.setFont(QFont("Microsoft YaHei UI", 9, QFont::Bold));
        p.setPen(INK_SECONDARY);
        p.drawText(QRectF(cx - slotW / 2, bar.top() - 20, slotW, 18),
                   Qt::AlignCenter, QString::number(qRound(m_values[i])));
        // X 轴标签（超长省略）
        p.setFont(baseFont());
        p.setPen(INK_SECONDARY);
        QString label = m_labels.value(i);
        const QFontMetrics fm(p.font());
        if (fm.horizontalAdvance(label) > slotW - 4)
            label = fm.elidedText(label, Qt::ElideRight, static_cast<int>(slotW - 4));
        p.drawText(QRectF(cx - slotW / 2, plot.bottom() + 6, slotW, 22),
                   Qt::AlignHCenter | Qt::AlignTop, label);
    }
}

// ---------------- HBarChartWidget ----------------

HBarChartWidget::HBarChartWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumHeight(200);
}

void HBarChartWidget::setData(const QString& title, const QStringList& labels,
                              const QVector<double>& values)
{
    m_title = title;
    m_labels = labels;
    m_values = values;
    m_maxValue = 0;
    for (double v : values)
        m_maxValue = std::max(m_maxValue, v);
    if (m_maxValue <= 0)
        m_maxValue = 1;
    update();
}

void HBarChartWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF card = rect().adjusted(1, 1, -1, -1);
    p.setPen(QPen(GRID, 1));
    p.setBrush(SURFACE);
    p.drawRoundedRect(card, 8, 8);

    p.setFont(QFont("Microsoft YaHei UI", 10, QFont::Bold));
    p.setPen(INK);
    p.drawText(QRectF(card.left() + 14, card.top() + 8, card.width() - 28, 22),
               Qt::AlignLeft | Qt::AlignVCenter, m_title);

    if (m_values.isEmpty())
        return;

    const QRectF plot(card.left() + 96, card.top() + 42,
                      card.width() - 96 - 14, card.height() - 42 - 14);
    const int n = m_values.size();
    const double rowH = plot.height() / n;
    const double barH = std::min(20.0, rowH * 0.55);

    p.setFont(baseFont());
    const QFontMetrics fm(p.font());
    for (int i = 0; i < n; ++i) {
        const double cy = plot.top() + rowH * (i + 0.5);
        // 名称（左列，超长省略）
        p.setPen(INK_SECONDARY);
        QString label = m_labels.value(i);
        if (fm.horizontalAdvance(label) > 84)
            label = fm.elidedText(label, Qt::ElideRight, 84);
        p.drawText(QRectF(card.left() + 8, cy - 10, 84, 20),
                   Qt::AlignRight | Qt::AlignVCenter, label);
        // 条形
        const double w = plot.width() * m_values[i] / m_maxValue;
        const QRectF bar(plot.left(), cy - barH / 2, std::max(w, 3.0), barH);
        QPainterPath path;
        path.addRoundedRect(bar, 4, 4);
        p.fillPath(path, BAR_BLUE);
        // 数值：条形足够宽时画在条内右端（白字），否则画在条外右侧
        const QString valueText = QString::number(qRound(m_values[i])) + " 人";
        p.setFont(QFont("Microsoft YaHei UI", 9, QFont::Bold));
        const QFontMetrics valueFm(p.font());
        const double textW = valueFm.horizontalAdvance(valueText);
        if (bar.width() > textW + 14) {
            p.setPen(Qt::white);
            p.drawText(QRectF(bar.right() - textW - 8, cy - 10, textW + 4, 20),
                       Qt::AlignRight | Qt::AlignVCenter, valueText);
        } else {
            p.setPen(INK_SECONDARY);
            p.drawText(QRectF(bar.right() + 6, cy - 10, textW + 4, 20),
                       Qt::AlignLeft | Qt::AlignVCenter, valueText);
        }
        p.setFont(baseFont());
    }
}
