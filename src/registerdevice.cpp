#include "registerdevice.h"

#include <QThread>

RegisterDevice::RegisterDevice(int registerFrequency, QObject *parent) :
    QObject(parent),
    m_registerFrequency(registerFrequency)
{
    m_thread = new QThread();
    moveToThread(m_thread);
    m_thread->start();

    QMetaObject::invokeMethod(this, "createTimer", Qt::BlockingQueuedConnection);
}

RegisterDevice::~RegisterDevice()
{
    QMetaObject::invokeMethod(this, "stopTimer", Qt::BlockingQueuedConnection);
}

void RegisterDevice::startTimer()
{
    QTimer *reqTimerPtr = m_registerTimer;

    if(reqTimerPtr){
        QMetaObject::invokeMethod(reqTimerPtr, "start", Qt::BlockingQueuedConnection,Q_ARG(int, m_registerFrequency));
    }
}

void RegisterDevice::createTimer()
{
    m_registerTimer = new QTimer(this);
    connect(m_registerTimer, &QTimer::timeout, this, &RegisterDevice::signalSendRegistrateDevice);
}

void RegisterDevice::stopTimer()
{
    if(m_registerTimer != nullptr){
        m_registerTimer->stop();
    }
}
