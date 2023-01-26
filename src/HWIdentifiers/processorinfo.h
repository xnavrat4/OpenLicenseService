#ifndef PROCESSORINFO_H
#define PROCESSORINFO_H

#include <QJsonObject>
#include <QString>



class ProcessorInfo
{
    QString m_type;
    QString m_serialNumber;

public:
    ProcessorInfo();
    ProcessorInfo(const QString &type);

    QJsonObject toJson();

    //getters
    QString getType() const;
    QString getSerialNumber() const;

    //setters
    void setType(const QString &newType);
    void setSerialNumber(const QString &newSerialNumber);
};

#endif // PROCESSORINFO_H
