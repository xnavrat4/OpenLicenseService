#include "hwidentifiers.h"
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QTextStream>
#include "cmath"
#include <QNetworkInterface>
#include <stdio.h>
#include <QCryptographicHash>

HWIdentifiers::HWIdentifiers() :
    m_os(OperatingSystem()),
    m_disks(getDiskIdentifier()),
    m_memory(getMemoryIdentifier()),
    m_processor(getProcessorIdentifier()),
    m_motherboard(getMotherBoardIdentifier()),
    m_macAdresses(getMACAdresses())
{
}

void HWIdentifiers::printHWIdentifiers()
{
    qDebug()<< m_os.getOs() << m_os.getMachineId();
    qDebug()<< m_processor.getType() << m_processor.getSerialNumber();
    qDebug()<< m_motherboard.getSerialNumber() <<  m_motherboard.getManufacturer() << m_motherboard.getProductName();
    for (const auto &ram : m_memory){
        qDebug()<< ram.getSerialNumber() <<  ram.getPartNumber() << ram.getSize() << "GB";
    }
    for (const auto &disk : m_disks){
        qDebug()<< disk.getSerialNumber() <<  disk.getSize() << "GB";
    }
    for (const auto &macAdress : m_macAdresses){
        qDebug()<< macAdress;
    }
}

QString HWIdentifiers::calculateHash()
{
    auto ProcessorString = "Processor:" + m_processor.getType() + m_processor.getSerialNumber();
    QString RAMString = "RAMs:";
    QString OSString = "OS:" + m_os.getOs() + m_os.getMachineId();
    QString MBString = "MB:" + m_motherboard.getManufacturer() + m_motherboard.getProductName() + m_motherboard.getSerialNumber();
    for (const auto &ram :  m_memory){
        RAMString += (ram.getPartNumber() + ram.getSerialNumber() + QString::number(ram.getSize()));
    }
    QString diskString = "Disks";
    for (const auto &disk :  m_disks){
        diskString += (disk.getId() + disk.getSerialNumber());
    }
    QString MACString = "MACs";
    for (const auto &address :  m_macAdresses){
        MACString += address;
    }

    QString stringToHash = OSString + ProcessorString + MBString + diskString + RAMString + MACString;
    QByteArray h = QByteArray(stringToHash.toUtf8().toBase64());
    auto hash = QCryptographicHash::hash(h, QCryptographicHash::Sha512);

    return QString(hash).toUtf8();
}

QJsonObject HWIdentifiers::toJson()
{
    QJsonObject hwIdentifiers;
    hwIdentifiers.insert("operatingSystemDto", m_os.toJson());
    hwIdentifiers.insert("motherBoardDto", m_motherboard.toJson());
    hwIdentifiers.insert("processorDto", m_processor.toJson());
    QJsonArray memoryArray;
    for (auto part : m_memory){
        memoryArray.append(part.toJson());
    }
    hwIdentifiers.insert("ramModuleDto", memoryArray);

    QJsonArray addressArray;
    for (const auto &address : m_macAdresses){
        QJsonObject obj;
        obj.insert("address", address);
        addressArray.append(obj);
    }
    hwIdentifiers.insert("macAddressList", addressArray);

    QJsonArray diskArray;
    for (const auto &disk : m_disks){
        diskArray.append(disk.toJson());
    }
    hwIdentifiers.insert("diskList", diskArray);

    return hwIdentifiers;
}

QString HWIdentifiers::staticToJson()
{
    QJsonObject hwIdentifiers;
    hwIdentifiers.insert("machineID", m_os.getMachineId());
    hwIdentifiers.insert("motherBoard", m_motherboard.toJson());
    hwIdentifiers.insert("processor", m_processor.toJson());
    QJsonDocument doc = QJsonDocument(hwIdentifiers);
    return doc.toJson(QJsonDocument::Compact);
}

QList<MemoryInfo> HWIdentifiers::getMemoryIdentifier()
{
QList<MemoryInfo> ramList;
#if defined(Q_OS_WIN)
    QString wmicInfo = getWMICMemoryInfo("partnumber");
    QTextStream stream(&wmicInfo);
    QString line;
    QList<QString> pns;
    while (stream.readLineInto(&line) && !line.isEmpty()) {
        pns.append(line.trimmed());
    }

    wmicInfo = getWMICMemoryInfo("serialnumber");
    QTextStream stream1(&wmicInfo);
    QList<QString> serialNumbers;
    while (stream1.readLineInto(&line) && !line.isEmpty()) {
        serialNumbers.append(line.trimmed());
    }


    wmicInfo = getWMICMemoryInfo("capacity");
    QTextStream stream2(&wmicInfo);
    QList<QString> capacityList;
    while (stream2.readLineInto(&line) && !line.isEmpty()) {
        capacityList.append(line.trimmed());
    }

    for (int i=0; i < pns.size(); i++){
        MemoryInfo info;
        //ram size in bytes
        int64_t size = capacityList.at(i).toLongLong();
        //save it as GB
        info.setSize(size / std::pow(1024, 3));
        info.setPartNumber(pns.at(i));
        info.setSerialNumber(serialNumbers.at(i));
        ramList.append(info);
    }


    //procInfo.setSerialNumber(cleanWMICValues(SNInfo));
#elif defined(Q_OS_LINUX)
    QString output = getIdentifierInfo("dmidecode", {"-t", "memory"});
    auto parts = output.split("DMI type 17");
    parts.removeAt(0);

    for (const auto &part : parts){
        QString device = part.sliced(part.indexOf("Memory Device"));

        QTextStream stream(&device);
        QString line;
        MemoryInfo ramModule;

        while (stream.readLineInto(&line)) {
            if (line.contains("\tSize:")){
                QString size = line.mid(line.indexOf(":") + 1);
                ramModule.setSize(size.trimmed().split(" ").first().toInt());
            }
            if (line.contains("\tSerial Number:")){
                QString serialNumber = line.mid(line.indexOf(":") + 1);
                ramModule.setSerialNumber(serialNumber.trimmed());
            }
            if (line.contains("\tPart Number:")){
                QString partNumber = line.mid(line.indexOf(":") + 1);
                ramModule.setPartNumber(partNumber.trimmed());
            }
        }
        if (!ramModule.isNull() && ramModule.getSize() > 0){
            ramList.append(ramModule);

        }
    }
#endif
    return ramList;
}

QString HWIdentifiers::getIdentifierInfo(const QString &utilityName, QStringList arguments)
{
    auto obtainingProcess = new QProcess();
    obtainingProcess->start(utilityName, arguments);
    obtainingProcess->waitForFinished();
    return obtainingProcess->readAllStandardOutput();
}

QList<QString> HWIdentifiers::getMACAdresses()
{
    QList<QString> macAddresses;
    for(const auto &netInterface: QNetworkInterface::allInterfaces()){
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack)){
            macAddresses.append(netInterface.hardwareAddress());
        }
    }
    return macAddresses;
}

ProcessorInfo HWIdentifiers::getProcessorIdentifier()
{
    ProcessorInfo procInfo;
#if defined(Q_OS_WIN)
    QString cpuNameInfo = getIdentifierInfo("powershell", {"wmic cpu get name"});
    procInfo.setType(cleanWMICValues(cpuNameInfo));
    QString SNInfo = getIdentifierInfo("powershell", {"wmic cpu get processorId"});
    procInfo.setSerialNumber(cleanWMICValues(SNInfo));

#elif defined(Q_OS_LINUX)
    QString output = getIdentifierInfo("dmidecode", {"-t", "processor"});

    QTextStream stream(&output);
    QString line;

    while (stream.readLineInto(&line)) {
        if (line.contains("\tVersion:")){
            QString type = line.mid(line.indexOf(":") + 1);
            procInfo.setType(type.trimmed());
        }
        if (line.contains("\tID:")){
            QString ID = line.mid(line.indexOf(":") + 1);
            QStringList IDList = ID.trimmed().split(" ");
            std::reverse(IDList.begin(), IDList.end());
            procInfo.setSerialNumber(IDList.join(""));
        }
    }
#endif
    return procInfo;
}

QString HWIdentifiers::baseBoardInfoFromDMI()
{
    QString utilityInfo = getIdentifierInfo("dmidecode", {"-t", "baseboard"});
    //gets rid of the dmi utility information
    QString sliced = utilityInfo.sliced(utilityInfo.indexOf("Base Board Information"));
    sliced.truncate(sliced.indexOf("\n\n"));

    return sliced;
}

MotherboardInfo HWIdentifiers::getMotherBoardIdentifier()
{
    MotherboardInfo mb;
#if defined(Q_OS_WIN)
    QString manufacturerInfo = getIdentifierInfo("powershell", {"wmic baseboard get Manufacturer"});
    mb.setManufacturer(cleanWMICValues(manufacturerInfo));
    QString SNInfo = getIdentifierInfo("powershell", {"wmic baseboard get SerialNumber"});
    mb.setSerialNumber(cleanWMICValues(SNInfo));
    QString productName = getIdentifierInfo("powershell", {"wmic baseboard get Product"});
    mb.setProductName(cleanWMICValues(productName));
#elif defined(Q_OS_LINUX)
    QString boardInfo = baseBoardInfoFromDMI();

    QTextStream stream(&boardInfo);
    QString line;
    while (stream.readLineInto(&line)) {
        if (line.contains("\tManufacturer:")){
            QString manufacturer = line.mid(line.indexOf(":") + 1);
            mb.setManufacturer(manufacturer.trimmed());
        }
        if (line.contains("\tSerial Number")){
            QString sn = line.mid(line.indexOf(":") + 1);
            mb.setSerialNumber(sn.trimmed());
        }
        if (line.contains("\tProduct Name")){
            QString pn = line.mid(line.indexOf(":") + 1);
            mb.setProductName(pn.trimmed());
        }
    }
#endif
    return mb;
}

QList<DiskInfo> HWIdentifiers::getDiskIdentifier()
{
    QList<DiskInfo> diskInfoList;
#ifdef _WIN32 // unzip on Windows with 7z

    QString line;
    QString mediaTypes = getWMICDiskInfo("MediaType");
    QTextStream stream1(&mediaTypes);
    QList<int> fixedDisks;
    int i = 0;
    while (stream1.readLineInto(&line) && !line.isEmpty()) {
        if (line.contains("SSD")){
            fixedDisks.append(i);
        }
        i++;
    }

    QString serialNumbers = getWMICDiskInfo("SerialNumber");
    QTextStream stream2(&serialNumbers);
    QList<QString> serialNumberList;
    i = 0;
    while (stream2.readLineInto(&line) && !line.isEmpty()) {
        if (fixedDisks.contains(i)){
            serialNumberList.append(line.trimmed());
        }
        i++;
    }

    QString ids = getWMICDiskInfo("UniqueId");
    QTextStream stream3(&ids);
    QList<QString> idList;
    i = 0;
    while (stream3.readLineInto(&line) && !line.isEmpty()) {
        if (fixedDisks.contains(i)){
            idList.append(line.trimmed());
        }
        i++;
    }

    QString sizes = getWMICDiskInfo("Size");
    QTextStream stream4(&sizes);
    QList<QString> sizesList;
    i = 0;
    while (stream4.readLineInto(&line) && !line.isEmpty()) {
        if (fixedDisks.contains(i)){
            sizesList.append(line.trimmed());
        }
        i++;
    }

    for (int i=0; i < fixedDisks.size(); i++){
        DiskInfo disk;
        //ram size in bytes
        int64_t size = sizesList.at(i).toLongLong();
        //save it as GB
        disk.setSize(QString("%1G").arg(QString::number(size / std::pow(1024, 3))));
        disk.setSerialNumber(serialNumberList.at(i));
        disk.setId(idList.at(i));
        diskInfoList.append(disk);
    }
#else // unzip on Linux with preinstalled unzip tool
    auto obtainingProcess = new QProcess();

    QStringList list = QStringList ()<< "-J"  << "-o" << "name,serial,mountpoints,uuid,size";
   // obtainingProcess->setArguments(list);
    obtainingProcess->start("lsblk", list);
    obtainingProcess->waitForFinished();
    QString output(obtainingProcess->readAllStandardOutput());

    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    if (doc.isNull() || doc.isEmpty()){
        return diskInfoList;
    }
    QJsonObject obj = doc.object();
    QJsonArray devicesArray = obj.value("blockdevices").toArray();
    for (const auto &dev : devicesArray){
        QString serial = dev.toObject().value("serial").toString();
        QString size = dev.toObject().value("size").toString();
        QJsonArray children = dev.toObject().value("children").toArray();
        QString uuid;
        for (const auto &child : children){
            const QJsonArray mountpointsArray = child.toObject().value("mountpoints").toArray();
            for (const auto &mountPoint : mountpointsArray){
                if (mountPoint.toString() == "/"){
                    uuid = child.toObject().value("uuid").toString();
                }
            }
        }
        if (!uuid.isEmpty()){
            DiskInfo diskInfo = DiskInfo(uuid, serial,size);
            diskInfoList.append(diskInfo);
        }
    }
#endif
    return diskInfoList;
}


QString HWIdentifiers::cleanWMICValues(const QString &completeMessage)
{
    QString dirtyString = completeMessage.split("\n").at(1);
    dirtyString.truncate(dirtyString.lastIndexOf(" "));
    return dirtyString.trimmed();
}

QString HWIdentifiers::getWMICMemoryInfo(const QString& arg)
{
    QString partNumberInfo = getIdentifierInfo("powershell", {QString("wmic memorychip get %1").arg(arg)});
    QString data = partNumberInfo.mid(partNumberInfo.indexOf("\n") + 1, partNumberInfo.length());
    data.remove("\r");
    return data;
}

QString HWIdentifiers::getWMICDiskInfo(const QString& arg)
{
    QString partNumberInfo = getIdentifierInfo("powershell", {QString("Get-PhysicalDisk | Select-Object %1").arg(arg)});
    QString data = partNumberInfo.mid(partNumberInfo.indexOf("\n") + 1, partNumberInfo.length());
    data.remove("\r");
    return data;
}

