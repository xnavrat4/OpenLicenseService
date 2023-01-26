#include "filemanager.h"

#include <QSettings>
#include <stdio.h>
#include <iostream>
#include <exception>
#include <QJsonArray>
#include <QJsonDocument>
#include "OpenLicenseCommunication/fileencrypter.h"
#include "qjsondocument.h"


FileManager::FileManager(HWIdentifiers *identifiers) :
    m_identifiers(identifiers)
{
}

void FileManager::load()
{
    QSettings settings;
    settings.beginGroup("secret");
    QString encryptedData = settings.value("enc").toString();
    if (encryptedData.isEmpty()){
        return;
    }
    QString decryptedData = loadEncrypted(encryptedData, m_identifiers->staticToJson());

    QJsonDocument document = QJsonDocument::fromJson(decryptedData.toUtf8().data());
    if (!document.isEmpty() && document.isObject() && !document.isNull()){
        this->fromJson(document.object());
        return;
    }

    QString hwInfoBackup = loadPlain();
    if (hwInfoBackup.isEmpty()){
        return;
    }

    decryptedData = loadEncrypted(encryptedData, hwInfoBackup);
    document = QJsonDocument::fromJson(decryptedData.toUtf8().data());
    if (!document.isEmpty() && document.isObject() && !document.isNull()){
        this->fromJson(document.object());
        m_violations.append(Violation(Violation::ViolationType::HWViolation, hwInfoBackup, m_identifiers->staticToJson(), QDateTime::currentDateTimeUtc()));
        savePlain(m_identifiers->staticToJson());
        saveEncrypted();
    }
}

void FileManager::saveEncrypted()
{
    QJsonDocument jsonDoc(this->toJson());
    QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);
    auto encrypted = FileEncrypter::encrypt(data, m_identifiers->staticToJson());

    QSettings settings;
    settings.beginGroup("secret");
    settings.setValue("enc", QString::fromStdString(encrypted));
}

void FileManager::processReport(const Report &report)
{
    m_id = report.getDeviceId();
    m_lastServerTime = report.getServerTime();
    m_heartBeatFrequency = report.getHeartbeatFrequency();
    m_licenses = report.getLicenseList();
    m_violations = report.getViolationList();
}

QJsonObject FileManager::toJson()
{
    QJsonObject obj;
    obj.insert("id", m_id);
    obj.insert("privateKey", m_servicePrivateKey);
    obj.insert("publicKey", m_servicePublicKey);
    obj.insert("publicKeySignature", m_servicePublicKeySignature);
    obj.insert("lastServerTime", m_lastServerTime);
    obj.insert("lastServiceTime", m_lastServiceTime);
    obj.insert("heartbeatFrequency", m_heartBeatFrequency);

    QJsonArray licenseArray;
    for (auto &license : m_licenses){
        licenseArray.append(license.toJson());
    }
    obj.insert("licenses", licenseArray);

    QJsonArray violationsArray;
    for (auto &violation : m_violations){
        violationsArray.append(violation.toJson());
    }
    obj.insert("violation", violationsArray);

    return obj;
}

void FileManager::fromJson(QJsonObject json)
{
    m_id = json.value("id").toInt(-1);
    m_servicePrivateKey = json.value("privateKey").toString();
    m_servicePublicKey = json.value("publicKey").toString();
    m_servicePublicKeySignature = json.value("publicKeySignature").toString();
    m_lastServerTime = json.value("lastServerTime").toString();
    m_lastServiceTime = json.value("lastServiceTime").toString();
    m_heartBeatFrequency = json.value("heartbeatFrequency").toInt();

    QJsonArray licenseArray = json.value("licenses").toArray();
    for (const auto &obj : licenseArray){
        License l;
        l.fromJson(obj.toObject());
        m_licenses.append(l);
    }

    QJsonArray violationArray = json.value("violation").toArray();
    for (const auto &obj : violationArray){
        Violation v;
        v.fromJson(obj.toObject(), false);
        m_violations.append(v);
    }
}

QString FileManager::getServicePublicKey() const
{
    return m_servicePublicKey;
}

void FileManager::setServicePublicKey(const QString &newServicePublicKey)
{
    m_servicePublicKey = newServicePublicKey;
}

QString FileManager::getServicePrivateKey() const
{
    return m_servicePrivateKey;
}

int FileManager::getHeartBeatFrequency() const
{
    return m_heartBeatFrequency;
}

void FileManager::setServicePrivateKey(const QString &newServicePrivateKey)
{
    m_servicePrivateKey = newServicePrivateKey;
}

QList<Violation> FileManager::getViolations() const
{
    return m_violations;
}

QList<Violation> FileManager::getNewViolations() const
{
    QList<Violation> newViols;
    for (auto v : m_violations){
        if (!v.isFromServer()){
            newViols.append(v);
        }
    }
    return newViols;
}

void FileManager::setViolations(const QList<Violation> &newViolations)
{
    m_violations = newViolations;
}

QString FileManager::getLastServiceTime() const
{
    return m_lastServiceTime;
}

void FileManager::setLastServiceTime(const QString &newLastServiceTime)
{
    m_lastServiceTime = newLastServiceTime;
}

QString FileManager::getServicePublicKeySignature() const
{
    return m_servicePublicKeySignature;
}

void FileManager::setServicePublicKeySignature(const QString &newServicePublicKeySignature)
{
    m_servicePublicKeySignature = newServicePublicKeySignature;
}

QString FileManager::getLastServerTime() const
{
    return m_lastServerTime;
}

void FileManager::setLastServerTime(const QString &newLastServerTime)
{
    m_lastServerTime = newLastServerTime;
}


void FileManager::savePlain(const QString& newIdentifiers)
{
    QSettings settings;
    settings.beginGroup("HWGroup");
    settings.setValue("hwInfo", newIdentifiers);
}

QString FileManager::loadPlain()
{
    QSettings settings;
    settings.beginGroup("HWGroup");
    return settings.value("hwInfo").toString();
}


QString FileManager::loadEncrypted(const QString &encryptedString, const QString &identifiers)
{
    QString decryptedString = FileEncrypter::decrypt(encryptedString.toStdString(), identifiers);
    decryptedString.chop(decryptedString.size() - decryptedString.lastIndexOf("}") - 1);
    return decryptedString;
}

void FileManager::onSystemTimeChanged(const QString &newSystemTime)
{
    m_lastServiceTime = newSystemTime;
    saveEncrypted();
}

void FileManager::saveViolation(Violation violation)
{
    m_violations.append(violation);
    saveEncrypted();
}

int FileManager::getId() const
{
    return m_id;
}

void FileManager::setId(int newId)
{
    m_id = newId;
}

QList<License> FileManager::getLicenses() const
{
    return m_licenses;
}

void FileManager::setLicenses(const QList<License> &newLicenses)
{
    m_licenses = newLicenses;
}

