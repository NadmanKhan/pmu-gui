#include "phasor.h"

qreal Phasor::m_maxAmplitude = 250.0;
qsizetype Phasor::m_capacity = 20;
qint64 Phasor::m_interval = 50;
Phasor::ModeOfOperation Phasor::m_mode = Phasor::Random;
QTimer Phasor::m_timer = QTimer();
qint64 Phasor::m_currTime = 0;

QMetaObject::Connection Phasor::m_connection;

std::array<Phasor* const, 7> Phasor::phasors = {
    new Phasor("IA", "IA", Qt::red),
    new Phasor("IB", "IB", Qt::green),
    new Phasor("IC", "IC", Qt::blue),
    new Phasor("IN", "IN", Qt::yellow),
    new Phasor("VA", "VA", Qt::darkRed),
    new Phasor("VB", "VB", Qt::darkGreen),
    new Phasor("VC", "VC", Qt::darkBlue)};

void Phasor::setup(
    qreal maxAmplitude,
    qsizetype capacity,
    qint64 interval,
    Phasor::ModeOfOperation mode) {

    m_maxAmplitude = maxAmplitude;
    m_capacity = capacity;
    m_interval = interval;
    m_mode = mode;
    for (auto& phasor : phasors) {
        phasor->clear();
    }
    disconnect(m_connection);
    m_connection = connect(
        &m_timer,
        &QTimer::timeout,
        [] {
            m_currTime += m_interval;
            for (auto& phasor : phasors) {
                auto magnitude = random(m_maxAmplitude * -1.0, m_maxAmplitude);
                auto phaseAngle = random(0, 1);
                phasor->add({phaseAngle, magnitude, m_currTime});
            }
        });
    m_timer.start(interval);
}

Phasor::Phasor(QString name, QString label, QColor color, QString unit)
    : name(name), label(label), color(color), unit(unit) {}

qreal Phasor::random(qreal low, qreal high) {
    return low + (QRandomGenerator::global()->generateDouble() * (high - low));
}

qreal Phasor::maxAmplitude() {
    return m_maxAmplitude;
}

qint64 Phasor::interval() {
    return m_interval;
}

Phasor::ModeOfOperation Phasor::mode() {
    return m_mode;
}

qsizetype Phasor::capacity() {
    return m_capacity;
}

void Phasor::add(const Value& value) {
    m_phaseAngles.push_back(value.phaseAngle);
    m_magnitudes.push_back(value.magnitude);
    m_timestamps.push_back(value.timeStamp);
    while ((qsizetype)m_phaseAngles.size() > m_capacity) {
        m_phaseAngles.pop_front();
        m_magnitudes.pop_front();
        m_timestamps.pop_back();
    }
    emit newValueAdded(value);
}

void Phasor::clear() {
    m_phaseAngles.clear();
    m_magnitudes.clear();
    m_timestamps.clear();
}

std::deque<Phasor::Value> Phasor::values() const {
    std::deque<Phasor::Value> list;
    for (int i = 0; i < (int)m_phaseAngles.size(); ++i) {
        list.push_back({m_phaseAngles[i], m_magnitudes[i], m_timestamps[i]});
    }
    return list;
}

Phasor::Value_SOA Phasor::values_SOA() const {
    std::deque<qreal> phaseAngles(m_phaseAngles.begin(), m_phaseAngles.end());
    std::deque<qreal> magnitudes(m_magnitudes.begin(), m_magnitudes.end());
    std::deque<qint64> timestamps(m_timestamps.begin(), m_timestamps.end());
    return {phaseAngles, magnitudes, timestamps};
}
