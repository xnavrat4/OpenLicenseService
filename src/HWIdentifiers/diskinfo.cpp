#include "diskinfo.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

DiskInfo::DiskInfo()
{
}

DiskInfo::DiskInfo(const QString &id, const QString &sn, const QString &size) :
    m_id(id),
    m_serialNumber(sn),
    m_size(size)
{
}

QJsonObject DiskInfo::toJson() const
{
    QJsonObject obj;
    obj.insert("diskId", m_id);
    obj.insert("serialNumber", m_serialNumber);
    obj.insert("size", m_size);
    return obj;
}

QString DiskInfo::getId() const
{
    return m_id;
}

QString DiskInfo::getSerialNumber() const
{
    return m_serialNumber;
}

QString DiskInfo::getSize() const
{
    return m_size;
}

void DiskInfo::setId(const QString &newName)
{
    m_id = newName;
}

void DiskInfo::setSerialNumber(const QString &newSerialNumber)
{
    m_serialNumber = newSerialNumber;
}

void DiskInfo::setSize(const QString &newSize)
{
    m_size = newSize;
}

