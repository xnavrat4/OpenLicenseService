#ifndef REGISTERDEVICE_H
#define REGISTERDEVICE_H

#include <QJsonDocument>
#include <QNetworkReply>
#include <QObject>
#include <QThread>
#include <QTimer>

class RegisterDevice : public QObject
{
    Q_OBJECT

    QThread *m_thread{nullptr};

    QTimer *m_registerTimer{nullptr};
    int m_registerFrequency{30000};  //ms -> 30sec

public:
    RegisterDevice(int registerFrequency, QObject* parent = nullptr);
    ~RegisterDevice();

    void startTimer();

private slots:
    void createTimer();
    void stopTimer();

signals:
    void signalSendRegistrateDevice();
};

#endif // REGISTERDEVICE_H
