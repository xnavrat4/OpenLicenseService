#include "motherboardinfo.h"

MotherboardInfo::MotherboardInfo()
{
}

MotherboardInfo::MotherboardInfo(const QString &manufacturer, const QString &productName, const QString &serialNumber) :
    m_manufacturer(manufacturer),
    m_productName(productName),
    m_serialNumber(serialNumber)
{
}

QJsonObject MotherboardInfo::toJson()
{
    QJsonObject mbJson;
    mbJson.insert("SerialNumber", m_serialNumber);
    mbJson.insert("ProductName", m_productName);
    mbJson.insert("Manufacturer", m_manufacturer);
    return mbJson;
}

QString MotherboardInfo::getManufacturer() const
{
    return m_manufacturer;
}

QString MotherboardInfo::getSerialNumber() const
{
    return m_serialNumber;
}

QString MotherboardInfo::getProductName() const
{
    return m_productName;
}

void MotherboardInfo::setManufacturer(const QString &newManufacturer)
{
    m_manufacturer = newManufacturer;
}

void MotherboardInfo::setSerialNumber(const QString &newSerialNumber)
{
    m_serialNumber = newSerialNumber;
}

void MotherboardInfo::setProductName(const QString &newProductName)
{
    m_productName = newProductName;
}
