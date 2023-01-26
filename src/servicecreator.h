#ifndef SERVICECREATOR_H
#define SERVICECREATOR_H

#include "OpenLicenseCommunication/websocketserver.h"
#include "licenseservice.h"
#include "OpenLicenseCommunication/servercommunicator.h"

#include <QObject>

class ServiceCreator : public QObject
{
    Q_OBJECT

    LicenseService* m_service;
    ServerCommunicator* m_serverCommunicator;
    WebSocketServer* m_wsServer;

    char* m_serverAddress = AY_OBFUSCATE("localhost");
    int m_serverPortNumber = 50102;
    int m_wsPortNumber = 15679;

public:
    explicit ServiceCreator(QObject *parent = nullptr);

private:
    void connectServiceAndServer();
    void connectServiceAndWSServer();
signals:

};

#endif // SERVICECREATOR_H
