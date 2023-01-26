#include "processorinfo.h"

ProcessorInfo::ProcessorInfo()
{
}

ProcessorInfo::ProcessorInfo(const QString &type) : m_type(type)
{
}

QJsonObject ProcessorInfo::toJson()
{
    QJsonObject obj;
    obj.insert("type", m_type);
    obj.insert("processorId", m_serialNumber);
    return obj;
}

QString ProcessorInfo::getType() const
{
    return m_type;
}

QString ProcessorInfo::getSerialNumber() const
{
    return m_serialNumber;
}

void ProcessorInfo::setType(const QString &newType)
{
    m_type = newType;
}

void ProcessorInfo::setSerialNumber(const QString &newSerialNumber)
{
    m_serialNumber = newSerialNumber;
}
