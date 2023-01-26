#ifndef MEMORYINFO_H
#define MEMORYINFO_H

#include <QJsonObject>
#include <QString>



class MemoryInfo
{
    QString m_partNumber;
    QString m_serialNumber;
    int m_size;

public:
    MemoryInfo();

    QJsonObject toJson();
    bool isNull() const;

    //getters
    QString getSerialNumber() const;
    QString getPartNumber() const;
    int getSize() const;

    //setters
    void setSerialNumber(const QString &newSerialNumber);
    void setPartNumber(const QString &newPartNumber);
    void setSize(int newSize);
};

#endif // MEMORYINFO_H
