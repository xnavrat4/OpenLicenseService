#ifndef REPORTER_H
#define REPORTER_H

#include <QObject>
#include <QThread>
#include <QTimer>

class Reporter : public QObject
{
    Q_OBJECT

    QThread *m_thread{nullptr};

    QTimer *m_registerTimer{nullptr};
    int m_timerLenght{30000}; //ms -> 30sec

public:
    Reporter(int reportFrequency, QObject* parent = nullptr);
    void startTimer();

signals:
    void signalSendReportRequest();

private slots:
    void createTimer();
    void stopTimer();
};

#endif // REPORTER_H
