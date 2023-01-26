#include "reporter.h"

Reporter::Reporter(int reportFrequency, QObject *parent) :
    QObject(parent),
    m_timerLenght(reportFrequency)
{
    m_thread = new QThread();
    moveToThread(m_thread);
    m_thread->start();

    QMetaObject::invokeMethod(this, "createTimer", Qt::BlockingQueuedConnection);
}

void Reporter::startTimer()
{
    QTimer *reqTimerPtr = m_registerTimer;


    if(reqTimerPtr){
        QMetaObject::invokeMethod(reqTimerPtr, "start", Qt::BlockingQueuedConnection,Q_ARG(int, m_timerLenght));
    }
}

void Reporter::createTimer()
{
    m_registerTimer = new QTimer(this);
    connect(m_registerTimer, &QTimer::timeout, this, &Reporter::signalSendReportRequest);
}

void Reporter::stopTimer()
{
    if(m_registerTimer != nullptr){
        m_registerTimer->stop();
    }
}
