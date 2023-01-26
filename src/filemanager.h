#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "HWIdentifiers/hwidentifiers.h"
#include "OpenLicenseCommunication/license.h"
#include "OpenLicenseCommunication/report.h"
#include "OpenLicenseCommunication/violation.h"

#include <QDateTime>
#include <QString>

class FileManager : public QObject
{
    Q_OBJECT

    QString m_servicePrivateKey;
    QString m_servicePublicKey;
    QString m_servicePublicKeySignature;

    QString m_lastServiceTime;
    QString m_lastServerTime;

    int m_id{0};
    int m_heartBeatFrequency{0};

    HWIdentifiers* m_identifiers{nullptr};

    QList<License> m_licenses;
    QList<Violation> m_violations;

public:
    FileManager(HWIdentifiers* identifiers);

    void load();
    void saveEncrypted();

    void processReport(const Report& report);

    //json methods
    QJsonObject toJson();
    void fromJson(QJsonObject json);

    //getters
    int getId() const;
    QList<License> getLicenses() const;
    QString getServicePublicKey() const;
    QString getServicePrivateKey() const;
    QString getLastServiceTime() const;
    QList<Violation> getViolations() const;
    QList<Violation> getNewViolations() const;
    QString getServicePublicKeySignature() const;
    QString getLastServerTime() const;
    int getHeartBeatFrequency() const;

    //setters
    void setId(int newId);
    void setLicenses(const QList<License> &newLicenses);
    void setServicePublicKey(const QString &newServicePublicKey);
    void setServicePrivateKey(const QString &newServicePrivateKey);
    void setViolations(const QList<Violation> &newViolations);
    void setLastServiceTime(const QString &newLastServiceTime);
    void setServicePublicKeySignature(const QString &newServicePublicKeySignature);
    void setLastServerTime(const QString &newLastServerTime);

private:
    //plain storage
    QString loadPlain();
    void savePlain(const QString& newIdentifiers);

    //encrypted storage
    QString loadEncrypted(const QString& encryptedString, const QString& identifiers);

public slots:
    //to save changed data
    void onSystemTimeChanged(const QString& newSystemTime);
    void saveViolation(Violation violation);

};

#endif // FILEMANAGER_H
