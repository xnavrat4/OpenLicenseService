#ifndef LICENSESERVICE_H
#define LICENSESERVICE_H

#include "OpenLicenseCommunication/report.h"
#include "filemanager.h"
#include "licensevalidator.h"
#include "registerdevice.h"
#include "reporter.h"
#include "timemanager.h"

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

class LicenseService : public QObject
{
    Q_OBJECT

    HWIdentifiers* m_hwIdentifiers{nullptr};

    int m_deviceId;
    FileManager* m_fileManager{nullptr};

    RegisterDevice* m_registerDevice{nullptr};
    Reporter* m_reporter{nullptr};
    LicenseValidator* m_licenseValidator{nullptr};
    TimeManager* m_timeManager{nullptr};

    int m_reportFrequency{30000}; //30sec
    int m_registerFrequency{30000}; //30sec

public:
    LicenseService(QObject* parent = nullptr);
    ~LicenseService();

    void init();

    QString getPublicKey();
    QString getPrivateKey();

    int getDeviceId() const;

public slots:
    //registration
    void onRegistrationSuccessful(int deviceId, const QString& signature, const QString& serverTime);

    //reports
    void onReportReceived(const Report &report);
    void onReportTimedOut();

    //licenses
    void onLicenseReceived(const License &license, const QString &appName);
    void onLicenseTimedOut(const QString& appName, const QString& licenseKey);

    //to trigger actions
    void registerDevice();
    void onSendReport();
    void validateLicense(const QString& appName, const QString& licenseKey);

signals:
    //to send requests
    void signalRegisterService(QJsonObject hwIdentifiers);
    void signalSendReportRequest(int deviceId, QList<Violation> newViolations);

    //for license validator
    void signalReceivedLicense(License license, const QString& appName);
    void signalReceivedLicenses(QList<License> licenseList);
    void signalReceivedViolation(QList<Violation> violations);
    void signalRecievedHeartBeatFrequency(int frequency);
    void signalRecievedServerTime(const QString& serverTime);
    void signalNewViolation(Violation violation);
    void signalLicenseValidated(bool valid, const QString& message, const QString& parameters, const QString& licenseKey,  const QString& appName);

    //for ws server
    void signalKeys(const QString& publicKey, const QString& privateKey, const QString& publicKeySignature);

private:
    void generateRSAKeyPair();

    //init control classes
    void initLicenseValidator();
    void initReporter();
    void initTimeManager(const QString& lastServerTime, const QString& lastServiceTime);
};


#endif // LICENSESERVICE_H
