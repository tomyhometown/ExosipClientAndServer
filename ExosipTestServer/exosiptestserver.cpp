#include <signal.h>
#include <QCoreApplication>
#include <QDebug>
#include <QMetaType>

#include "sipservice.h"
SipService *pSipService = NULL;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<QVariant>("QVariant&");

    pSipService = new SipService;

    if(!pSipService->InitSipService())
    {
        qDebug() << "main--Init sip service failed.";
        return -1;
    }

    return a.exec();
}
