#include "sipservice.h"

#include <unistd.h>
#include <QDebug>
#include <QCoreApplication>

#include "handlesipevents.h"

SipService::SipService()
    : m_pHandleSip(NULL)
{

}

SipService::~SipService()
{

}

bool SipService::InitSipService()
{
    int nRet = -1;

    m_pHandleSip = new HandleSipEvents;

    nRet = m_pHandleSip->Init();
    if(!nRet)
    {
        qDebug() << "SipService::InitSipService--Init sip context failed!!!";
        return false;
    }
    m_pHandleSip->start();
    return true;
}

