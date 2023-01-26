#include "servicecreator.h"

ServiceCreator::ServiceCreator(QObject *parent)
    : QObject{parent}
{
    m_service = new LicenseService();
    m_serverCommunicator = new ServerCommunicator(m_service->getDeviceId());
    m_wsServer = new WebSocketServer();

    QMetaObject::invokeMethod(m_wsServer, "createServer", Qt::BlockingQueuedConnection,Q_ARG(quint16, m_wsPortNumber));
    connectServiceAndWSServer();
    m_service->init();
    m_serverCommunicator->setPublicKey(m_service->getPublicKey());
    m_serverCommunicator->setPrivateKey(m_service->getPrivateKey());
    m_serverCommunicator->connectNetworkClient(QString::fromStdString(m_serverAddress), m_serverPortNumber);
    connectServiceAndServer();
}

void ServiceCreator::connectServiceAndServer()
{
    if (!m_service || !m_serverCommunicator){
        return;
    }

    connect(m_service, &LicenseService::signalRegisterService, m_serverCommunicator, &ServerCommunicator::sendRegisterService);
    connect(m_service, &LicenseService::signalSendReportRequest, m_serverCommunicator, &ServerCommunicator::sendReport);

    connect(m_serverCommunicator, &ServerCommunicator::signalLicenseValidated, m_service, &LicenseService::onLicenseReceived);
    connect(m_serverCommunicator, &ServerCommunicator::signalLicenseCheckTimeout, m_service, &LicenseService::onLicenseTimedOut);
    connect(m_serverCommunicator, &ServerCommunicator::signalRegistrationSuccesfull, m_service, &LicenseService::onRegistrationSuccessful);
    connect(m_serverCommunicator, &ServerCommunicator::signalReportReceived, m_service, &LicenseService::onReportReceived);
    connect(m_serverCommunicator, &ServerCommunicator::signalReportTimedOut, m_service, &LicenseService::onReportTimedOut);
}

void ServiceCreator::connectServiceAndWSServer()
{
    if (!m_service || !m_wsServer){
        return;
    }

    connect(m_wsServer, &WebSocketServer::signalValidateLicense, m_serverCommunicator, &ServerCommunicator::sendGetLicenseRequest);

    connect(m_service, &LicenseService::signalLicenseValidated, m_wsServer, &WebSocketServer::sendValidatedLicense);
    connect(m_service, &LicenseService::signalKeys, m_wsServer, &WebSocketServer::setKeys);

}
