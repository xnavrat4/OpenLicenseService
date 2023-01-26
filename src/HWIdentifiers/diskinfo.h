#ifndef DISKINFO_H
#define DISKINFO_H

#include <QProcess>
#include <QString>

class DiskInfo
{
    QString m_id;
    QString m_serialNumber;
    QString m_size;

public:
    DiskInfo();
    DiskInfo(const QString& id, const QString& sn, const QString& size);

    QJsonObject toJson() const;

    //getters
    QString getId() const;
    QString getSerialNumber() const;
    QString getSize() const;

    //setters
    void setId(const QString &newName);
    void setSerialNumber(const QString &newSerialNumber);
    void setSize(const QString &newSize);
};

#endif // DISKINFO_H
