#include "phasorpolarview.h"

PhasorPolarView::PhasorPolarView(QWidget* parent)
    : AbstractPhasorView(new QPolarChart(), parent) {

    auto chart = qobject_cast<QPolarChart*>(this->chart());

    /// create axes

    auto axisAngular = new QCategoryAxis(this);
    axisAngular->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
    axisAngular->setRange(0, 360);
    for (qint16 angleOnAxis = 0; angleOnAxis < 360; angleOnAxis += 30) {
        auto label = QString::asprintf("%d&deg;", toAngleActual(angleOnAxis));
        axisAngular->append(label, angleOnAxis);
    }
    chart->addAxis(axisAngular, QPolarChart::PolarOrientationAngular);

    auto axisRadial = new QValueAxis(this);
    axisRadial->setRange(0, 20);
    chart->addAxis(axisRadial, QPolarChart::PolarOrientationRadial);

    /// create series and add points

    for (auto phasor : Phasor::phasors) {
        auto series = new QLineSeries(this);
        chart->addSeries(series);

        series->setName(phasor->label);
        series->setPen(QPen(QBrush(phasor->color), 2.0));
        series->attachAxis(axisAngular);
        series->attachAxis(axisRadial);

        series->append(0.0, 0.0);

        connect(
            phasor,
            &Phasor::newValueAdded,
            this,
            [this, series](const Phasor::Value& v) {
                if (series->count() == 1) {
                    series->append(toPointF(v));
                } else {
                    series->replace(1, toPointF(v));
                }
            });

        addSeriesToControl(series, phasor->type);
    }
}

QPointF PhasorPolarView::toPointF(const Phasor::Value& value) const {
    return {toAngleOnAxisF(value.phaseAngle * 360.0), value.magnitude};
}

constexpr qint32 PhasorPolarView::normal(qint32 angle) {
    return ((angle % 360) + 360) % 360;
}

constexpr qreal PhasorPolarView::normalF(qreal angle) {
    angle = fmod(angle, 360.0);
    return angle + ((angle < 0.0) * 360.0);
}

constexpr qint32 PhasorPolarView::toAngleOnAxis(qint32 angleActual) {
    return normal(-angleActual - 90);
}

constexpr qreal PhasorPolarView::toAngleOnAxisF(qreal angleActual) {
    return normalF(-angleActual - 90);
}

constexpr qint32 PhasorPolarView::toAngleActual(qint32 angleOnAxis) {
    return normal(-angleOnAxis + 90);
}

constexpr qreal PhasorPolarView::toAngleActualF(qreal angleOnAxis) {
    return normalF(-angleOnAxis + 90.0);
}
