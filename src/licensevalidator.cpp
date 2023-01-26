#include "licensevalidator.h"

LicenseValidator::LicenseValidator(QObject *parent)
    : QObject{parent}
{
    m_thread = new QThread();
    moveToThread(m_thread);
    m_thread->start();
}

LicenseValidator::LicenseValidator(QDateTime lastServerTime, int heartBeat, const QList<License> &licenses, const QList<Violation> &violations, QObject *parent) :
    QObject{parent},
    m_lastTimeConnectedToServer(lastServerTime),
    m_heartbeatFrequency(heartBeat),
    m_availibleLicenseList(licenses),
    m_violations(violations)
{
}

void LicenseValidator::validateLicense(const License &license, const QString &appName)
{
    if (!m_violations.isEmpty()){
        QString violationType = m_violations.first().getViolationTypeString();
        emit signalLicenseValidated(false, violationType, QString(), license.getSerialNumber(), appName);
        return;
    }
    auto lastValidDay = m_lastTimeConnectedToServer.addSecs(m_heartbeatFrequency * 3600);

    if (QDateTime::currentDateTimeUtc().secsTo(lastValidDay) < 0 ){
        emit signalLicenseValidated(false, "Offline duration used up", QString(), license.getSerialNumber(), appName);
        return;
    }

    if (license.getProductName() != appName){
        emit signalLicenseValidated(false, "Product name mismatch!", QString(), license.getSerialNumber(), appName);
        return;
    }

    if (license.getRevoked()){
        emit signalLicenseValidated(false, "License is revoked", QString(), license.getSerialNumber(), appName);
        return;
    }

    if (license.getValidFrom().date().daysTo(QDateTime::currentDateTimeUtc().date()) < 0){
        emit signalLicenseValidated(false, "License is not yet valid", QString(), license.getSerialNumber(), appName);
        return;
    }

    if (license.getValidTo().date().daysTo(QDateTime::currentDateTimeUtc().date()) > 0){
        emit signalLicenseValidated(false, "License is no longer valid", QString(), license.getSerialNumber(), appName);
        return;
    }

    emit signalLicenseValidated(true, "License is valid!", license.getParameters(), license.getSerialNumber(), appName);
}

void LicenseValidator::serverTimeReceived(const QString &time)
{
    m_lastTimeConnectedToServer = QDateTime::fromString(time, Qt::ISODate);
}

void LicenseValidator::newViolationReceived(Violation newViolation)
{
    m_violations.append(newViolation);
}

void LicenseValidator::violationsReceived(QList<Violation> violations)
{
    m_violations = violations;
}

void LicenseValidator::heartBeatReceived(int heartBeat)
{
    m_heartbeatFrequency = heartBeat;
}

void LicenseValidator::licensesReceived(const QList<License> &validLicenses)
{
    m_availibleLicenseList = validLicenses;
}

void LicenseValidator::validateLicenseRequestTimedOut(const QString &appName, const QString &keyToValidate)
{
    for (const auto &license : m_availibleLicenseList){
        if (license.getSerialNumber() == keyToValidate){
            validateLicense(license, appName);
            return;
        }
    }
    //if no license is found -> signal invalid
    emit signalLicenseValidated(false, "No license with given key was found", QString(), keyToValidate, appName);
}

void LicenseValidator::validateLicenseRequest(License license, const QString &appName)
{
    validateLicense(license, appName);
}
