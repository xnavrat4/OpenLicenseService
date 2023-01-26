#include "servicecreator.h"

#include <QCoreApplication>
#include <iostream>
#include <QDebug>
#include <QNetworkRequest>
#include <QUrl>

#if defined Q_OS_LINUX
#include <unistd.h>
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName( QString("ols") );
    QCoreApplication::setOrganizationName(QString("OLorg") );
#if defined Q_OS_LINUX
    //check for sudo privilages
    if (getuid()){
        qDebug() << "Run this with sudo privileges! Exiting...";
        return 1;
    }else {
        qDebug() << "Sudo privileges obtained\n";
    }
#endif
    ServiceCreator creator;
    return a.exec();
}

