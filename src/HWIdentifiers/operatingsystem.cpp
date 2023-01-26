#include "operatingsystem.h"

OperatingSystem::OperatingSystem() :
    m_os(QSysInfo::prettyProductName()),
    m_machineId(QSysInfo::machineUniqueId())
{
}

QJsonObject OperatingSystem::toJson()
{
    QJsonObject osJson;
    osJson.insert("OSType", m_os);
    osJson.insert("MachineId", m_machineId);
    return osJson;
}

QString OperatingSystem::getOs() const
{
    return m_os;
}

QString OperatingSystem::getMachineId() const
{
    return m_machineId;
}

void OperatingSystem::setOs(const QString &newOs)
{
    m_os = newOs;
}

void OperatingSystem::setMachineId(const QString &newMachineId)
{
    m_machineId = newMachineId;
}
