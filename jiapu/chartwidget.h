#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QVector>
#include <QColor>

/**
 * 自绘柱状图（QPainter 实现，无第三方图表库依赖）
 * 配色遵循统一的浅色数据可视化规范：底 #fcfcfb / 主墨 #0b0b0b / 辅墨 #52514e /
 * 网格 #e1e0d9 / 轴线 #c3c2b7
 */
class BarChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BarChartWidget(QWidget* parent = nullptr);

    // 设置图表数据（labels 与 values 等长；barColor 为柱体颜色）
    void setData(const QString& title, const QStringList& labels,
                 const QVector<double>& values, const QColor& barColor);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_title;
    QStringList m_labels;
    QVector<double> m_values;
    QColor m_barColor;
    double m_maxValue = 0;
};

/**
 * 自绘横向条形图（用于支系人数 Top-K 排名）
 */
class HBarChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HBarChartWidget(QWidget* parent = nullptr);

    void setData(const QString& title, const QStringList& labels,
                 const QVector<double>& values);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString m_title;
    QStringList m_labels;
    QVector<double> m_values;
    double m_maxValue = 0;
};

#endif // CHARTWIDGET_H
