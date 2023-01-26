#include "timemanager.h"
#include <cstdlib>

TimeManager::TimeManager(QObject *parent) :
    QObject{parent}
{
    m_timer = new QElapsedTimer();
    m_timer->start();

    m_thread = new QThread();
    moveToThread(m_thread);
    m_thread->start();

}

void TimeManager::init(const QString &lastServerTime, const QString &lastServiceTime)
{
    m_serverTime = QDateTime::fromString(lastServerTime, Qt::ISODate);

        m_systemTime = QDateTime::fromString(lastServiceTime, Qt::ISODate);

        auto newSystemTime = QDateTime::currentDateTimeUtc();
        if (m_systemTime.secsTo(newSystemTime) < 0){
            //new server time can never be smaller then new time
            Violation v(Violation::SystemTimeViolation, m_serverTime.toString(Qt::ISODate), newSystemTime.toString(Qt::ISODate), QDateTime::currentDateTimeUtc());
            emit signalNewViolation(v);
            m_systemTime = newSystemTime;
        }

}

QDateTime TimeManager::getServerTime() const
{
    return m_serverTime;
}

void TimeManager::setServerTime(const QString &time)
{
    auto newServerTime = QDateTime::fromString(time, Qt::ISODate);
    if (m_serverTime.secsTo(newServerTime) < 0){
        //new server time can never be smaller then new time
        Violation v(Violation::ServerTimeViolation, m_serverTime.toString(Qt::ISODate), newServerTime.toString(Qt::ISODate), QDateTime::fromString(time, Qt::ISODate));
        emit signalNewViolation(v);
    }
    m_serverTime = newServerTime;

    updateSystemTime();
}

QDateTime TimeManager::getSystemTime() const
{
    return m_systemTime;
}

void TimeManager::updateSystemTime()
{
    auto newSystemTime = QDateTime::currentDateTimeUtc();
    auto oldSystemTime = m_systemTime;
    int64_t elapsed = m_timer->elapsed();
    m_timer->restart();
    oldSystemTime = oldSystemTime.addMSecs(elapsed);
    if (oldSystemTime.secsTo(newSystemTime) < 0){
        //new server time can never be smaller then new time
        Violation v(Violation::SystemTimeViolation, oldSystemTime.toString(Qt::ISODate), newSystemTime.toString(Qt::ISODate), QDateTime::currentDateTimeUtc());
        emit signalNewViolation(v);
    }
    m_systemTime = newSystemTime;
    emit signalSaveTimes(m_systemTime.toString(Qt::ISODate));
}
