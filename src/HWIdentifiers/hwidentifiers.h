#ifndef HWIDENTIFIERS_H
#define HWIDENTIFIERS_H

#include "diskinfo.h"
#include "memoryinfo.h"
#include "motherboardinfo.h"
#include "operatingsystem.h"
#include "processorinfo.h"

#include <QJsonObject>
#include <QList>

class HWIdentifiers
{
    OperatingSystem m_os;
    QList<DiskInfo> m_disks;
    QList<MemoryInfo> m_memory;
    ProcessorInfo m_processor;
    MotherboardInfo m_motherboard;
    QList<QString> m_macAdresses;

public:
    HWIdentifiers();

    void printHWIdentifiers();
    QString calculateHash();

    QJsonObject toJson();

    QString staticToJson();

private:
    //WMIC methods
    QString cleanWMICValues(const QString& completeMessage);
    QString getWMICMemoryInfo(const QString& arg);
    QString getWMICDiskInfo(const QString& arg);

    //DMIDECODE method
    QString baseBoardInfoFromDMI();

    //get identifiers
    QList<MemoryInfo> getMemoryIdentifier();
    ProcessorInfo getProcessorIdentifier();
    MotherboardInfo getMotherBoardIdentifier();
    QList<DiskInfo> getDiskIdentifier();
    QList<QString> getMACAdresses();
    QString getIdentifierInfo(const QString& utilityName, QStringList arguments);
};

#endif // HWIDENTIFIERS_H
