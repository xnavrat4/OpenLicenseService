#ifndef OPERATINGSYSTEM_H
#define OPERATINGSYSTEM_H

#include <QJsonObject>
#include <QString>

class OperatingSystem
{
    QString m_os;
    QString m_machineId;

public:
    OperatingSystem();

    QJsonObject toJson();

    //getters
    QString getOs() const;
    QString getMachineId() const;

    //setters
    void setOs(const QString &newOs);
    void setMachineId(const QString &newMachineId);
};

#endif // OPERATINGSYSTEM_H
