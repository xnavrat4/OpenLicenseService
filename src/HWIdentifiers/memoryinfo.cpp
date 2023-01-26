#include "memoryinfo.h"

MemoryInfo::MemoryInfo() : m_size(0)
{
}

QJsonObject MemoryInfo::toJson()
{
    QJsonObject obj;
    obj.insert("partNumber", m_partNumber);
    obj.insert("serialNumber", m_serialNumber);
    obj.insert("size", m_size);
    return obj;
}

QString MemoryInfo::getPartNumber() const
{
    return m_partNumber;
}

QString MemoryInfo::getSerialNumber() const
{
    return m_serialNumber;
}

int MemoryInfo::getSize() const
{
    return m_size;
}

void MemoryInfo::setPartNumber(const QString &newPartNumber)
{
    m_partNumber = newPartNumber;
}

void MemoryInfo::setSerialNumber(const QString &newSerialNumber)
{
    m_serialNumber = newSerialNumber;

}

void MemoryInfo::setSize(int newSize)
{
    m_size = newSize;
}

bool MemoryInfo::isNull() const {
    return m_serialNumber.isNull() && m_partNumber.isNull() && !m_size;
}

