#ifndef LICENSEVALIDATOR_H
#define LICENSEVALIDATOR_H

#include "OpenLicenseCommunication/license.h"
#include "OpenLicenseCommunication/violation.h"
#include <QObject>
#include <QThread>
#include <QTimer>

class LicenseValidator : public QObject
{
    Q_OBJECT

    QThread *m_thread{nullptr};

    QDateTime m_lastTimeConnectedToServer;
    int m_heartbeatFrequency{0};

    QList<License> m_availibleLicenseList;
    QList<Violation> m_violations;


public:
    explicit LicenseValidator(QObject *parent = nullptr);
    explicit LicenseValidator(QDateTime lastServerTime, int heartBeat, const QList<License>& licenses, const QList<Violation>& violations, QObject *parent = nullptr);

private:
    //the most important method - this one does all the heavy lifting
    void validateLicense(const License& license, const QString& appName);

public slots:
    // slots to get data
    void licensesReceived(const QList<License>& validLicenses);
    void newViolationReceived(Violation newViolation);
    void violationsReceived(QList<Violation> violations);
    void serverTimeReceived(const QString& time);
    void heartBeatReceived(int heartBeat);

    // license validate requests
    void validateLicenseRequest(License license, const QString& appName);
    void validateLicenseRequestTimedOut(const QString& appName, const QString& keyToValidate);

signals:
    void signalLicenseValidated(bool valid, const QString& message, const QString& parameters, const QString& licenseKey, const QString& appName);
};

#endif // LICENSEVALIDATOR_H
