#include "licenseservice.h"

#include <botan/aead.h>
#include <botan/rsa.h>
#include <botan/auto_rng.h>
#include <botan/secmem.h>
#include <botan/pubkey.h>
#include <botan/base64.h>
#include <botan/pkcs8.h>
#include <botan/rsa.h>
#include <botan/x509_key.h>

int LicenseService::getDeviceId() const
{
    return m_deviceId;
}

LicenseService::LicenseService(QObject *parent)
{
    m_hwIdentifiers = new HWIdentifiers();
    m_fileManager = new FileManager(m_hwIdentifiers);
    connect(this, &LicenseService::signalNewViolation, m_fileManager, &FileManager::saveViolation);

    m_fileManager->load();
    m_deviceId = m_fileManager->getId();
}

LicenseService::~LicenseService()
{
    if(m_registerDevice != nullptr){
        delete m_registerDevice;
        m_registerDevice = nullptr;
    }
    if(m_reporter != nullptr){
        delete m_reporter;
        m_reporter = nullptr;
    }
    if(m_licenseValidator != nullptr){
        delete m_licenseValidator;
        m_licenseValidator = nullptr;
    }
}

void LicenseService::initLicenseValidator()
{
    m_licenseValidator = new LicenseValidator(QDateTime::fromString(m_fileManager->getLastServerTime(), Qt::ISODate), m_fileManager->getHeartBeatFrequency(),
                                              m_fileManager->getLicenses(), m_fileManager->getViolations());

    connect(this, &LicenseService::signalReceivedLicenses, m_licenseValidator, &LicenseValidator::licensesReceived);
    connect(this, &LicenseService::signalReceivedLicense, m_licenseValidator, &LicenseValidator::validateLicenseRequest);
    connect(this, &LicenseService::signalNewViolation, m_licenseValidator, &LicenseValidator::newViolationReceived);
    connect(this, &LicenseService::signalReceivedViolation, m_licenseValidator, &LicenseValidator::violationsReceived);
    connect(this, &LicenseService::signalRecievedHeartBeatFrequency, m_licenseValidator, &LicenseValidator::heartBeatReceived);
    connect(this, &LicenseService::signalRecievedServerTime, m_licenseValidator, &LicenseValidator::serverTimeReceived);

    connect(m_licenseValidator, &LicenseValidator::signalLicenseValidated, this, &LicenseService::signalLicenseValidated);
}

void LicenseService::init()
{
    m_deviceId = m_fileManager->getId();
    if (m_deviceId <= 0){
        generateRSAKeyPair();
        m_registerDevice = new RegisterDevice(m_registerFrequency);
        m_registerDevice->startTimer();
        connect(m_registerDevice, &RegisterDevice::signalSendRegistrateDevice, this, &LicenseService::registerDevice);
    }
    else {
        initReporter();
        initLicenseValidator();
        initTimeManager(m_fileManager->getLastServerTime(), m_fileManager->getLastServiceTime());
    }
    emit signalKeys(m_fileManager->getServicePublicKey(), m_fileManager->getServicePrivateKey(), m_fileManager->getServicePublicKeySignature());
}

QString LicenseService::getPublicKey()
{
    if (m_fileManager){
        return m_fileManager->getServicePublicKey();
    }
    return QString();
}

QString LicenseService::getPrivateKey()
{
    if (m_fileManager){
        return m_fileManager->getServicePrivateKey();
    }
    return QString();
}

void LicenseService::initReporter()
{
    m_reporter = new Reporter(m_reportFrequency);
    m_reporter->startTimer();
    connect(m_reporter, &Reporter::signalSendReportRequest, this, &LicenseService::onSendReport);
}

void LicenseService::initTimeManager(const QString& lastServerTime, const QString& lastServiceTime)
{
    m_timeManager = new TimeManager();
    connect(m_timeManager, &TimeManager::signalSaveTimes, m_fileManager, &FileManager::onSystemTimeChanged);
    connect(m_timeManager, &TimeManager::signalNewViolation, this, &LicenseService::signalNewViolation);
    m_timeManager->init(lastServerTime, lastServiceTime);
}

void LicenseService::generateRSAKeyPair()
{
    Botan::AutoSeeded_RNG rng;
    Botan::RSA_PrivateKey keyPair(rng, static_cast<size_t>(4096));
    std::string privateKey = Botan::base64_encode(Botan::PKCS8::BER_encode(keyPair));
    m_fileManager->setServicePrivateKey(QString::fromStdString(privateKey));
    std::string publicKey = Botan::base64_encode(Botan::X509::BER_encode(keyPair));
    m_fileManager->setServicePublicKey(QString::fromStdString(publicKey));
    m_fileManager->saveEncrypted();
}

void LicenseService::onLicenseReceived(const License& license, const QString& appName)
{
    //m_fileManager->setLicenses(licenseList);
    //m_fileManager->saveEncrypted();
    emit signalReceivedLicense(license, appName);
}

void LicenseService::onLicenseTimedOut(const QString &appName, const QString &licenseKey)
{
    if (m_licenseValidator){
        m_licenseValidator->validateLicenseRequestTimedOut(appName, licenseKey);
    }
}

void LicenseService::onRegistrationSuccessful(int deviceId, const QString &signature, const QString &serverTime)
{
    m_fileManager->setId(deviceId);
    m_fileManager->setLastServerTime(serverTime);
    m_fileManager->setServicePublicKeySignature(signature);
    m_fileManager->saveEncrypted();
    m_deviceId = deviceId;

    if (m_registerDevice != nullptr){
        delete m_registerDevice;
        m_registerDevice = nullptr;

    }
    initReporter();
    initLicenseValidator();
    initTimeManager(serverTime, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    emit signalKeys(m_fileManager->getServicePublicKey(), m_fileManager->getServicePrivateKey(), m_fileManager->getServicePublicKeySignature());
}

void LicenseService::onReportReceived(const Report& report)
{
    m_fileManager->processReport(report);
    m_fileManager->saveEncrypted();

    m_timeManager->setServerTime(report.getServerTime());
    emit signalRecievedHeartBeatFrequency(report.getHeartbeatFrequency());
    emit signalRecievedServerTime(report.getServerTime());
    emit signalReceivedViolation(report.getViolationList());
    emit signalReceivedLicenses(report.getLicenseList());
}

void LicenseService::onReportTimedOut()
{
    m_timeManager->updateSystemTime();
}

void LicenseService::onSendReport()
{
    auto violations = m_fileManager->getNewViolations();
    emit signalSendReportRequest(m_deviceId, violations);
}

void LicenseService::validateLicense(const QString &appName, const QString &licenseKey)
{
    if (m_licenseValidator){

        m_licenseValidator->validateLicenseRequestTimedOut(appName, licenseKey);
    }
}

void LicenseService::registerDevice()
{
    emit signalRegisterService(m_hwIdentifiers->toJson());
}


