#ifndef MOTHERBOARDINFO_H
#define MOTHERBOARDINFO_H

#include <QJsonObject>
#include <QString>

class MotherboardInfo
{
    QString m_manufacturer;
    QString m_productName;
    QString m_serialNumber;

public:
    MotherboardInfo();
    MotherboardInfo(const QString &manufacturer, const QString &productName, const QString &serialNumber);

    QJsonObject toJson();

    //getters
    QString getManufacturer() const;
    QString getProductName() const;
    QString getSerialNumber() const;

    //setters
    void setManufacturer(const QString &newManufacturer);
    void setSerialNumber(const QString &newSerialNumber);
    void setProductName(const QString &newProductName);
};

#endif // MOTHERBOARDINFO_H
