#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "OpenLicenseCommunication/violation.h"
#include <QDateTime>
#include <QObject>
#include <QTimer>
#include <QThread>

class TimeManager : public QObject
{
    Q_OBJECT

    QThread *m_thread{nullptr};

    QElapsedTimer *m_timer{nullptr};

    QDateTime m_systemTime;
    QDateTime m_serverTime;

public:
    explicit TimeManager(QObject *parent = nullptr);

    void init(const QString& lastServerTime, const QString& lastServiceTime);

    //to count the time
    void setServerTime(const QString &time);
    void updateSystemTime();

    //getters
    QDateTime getSystemTime() const;
    QDateTime getServerTime() const;

signals:
    void signalNewViolation(Violation v);
    void signalSaveTimes(QString systemTime);
};

#endif // TIMEMANAGER_H
