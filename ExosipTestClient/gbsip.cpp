#include "gbsip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QDebug>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

GBSip::GBSip(eXosip_t *pContextSip, QObject *parent)
    : QThread(parent)
    , m_pContextSip(pContextSip)
    , m_bStop(false)
{

}

GBSip::~GBSip()
{
    clearSipEvents();
}

void GBSip::GetFirstSipEvent(eXosip_event_t* &pSipEvent)
{
    m_vecSipEventMutex.lock();
    pSipEvent = m_vecSipEvent.takeFirst();
    m_vecSipEventMutex.unlock();
}

bool GBSip::IsVecSipEventEmpty()
{
    return m_vecSipEvent.isEmpty();
}

void GBSip::clearSipEvents()
{
    m_vecSipEventMutex.lock();
    while(!m_vecSipEvent.isEmpty())
    {
        eXosip_event_free(m_vecSipEvent.takeFirst());
    }
    m_vecSipEventMutex.unlock();
}

void GBSip::stop()
{
    m_bStop = true;
}

void GBSip::run()
{
    qDebug() << "Enter GBSip::run() ...";

    while(!m_bStop)
    {
        msleep(1);
        eXosip_event_t *pSipEvent = NULL;
        pSipEvent = eXosip_event_wait(m_pContextSip, 0, 2);

        if(!pSipEvent)
        {
//            qDebug() << "No message received!";
            continue;
        }

        eXosip_lock(m_pContextSip);
        if(pSipEvent->type != EXOSIP_REGISTRATION_SUCCESS && pSipEvent->type != EXOSIP_REGISTRATION_FAILURE)
        {
            eXosip_default_action(m_pContextSip, pSipEvent);
            eXosip_automatic_action(m_pContextSip);
        }
        eXosip_unlock(m_pContextSip);

 //       qDebug() << "GBSip::eXosip event type: " << pSipEvent->type;
//        qDebug() << "GBSip::eXosip event tid:  " << pSipEvent->tid;

        m_vecSipEventMutex.lock();
        m_vecSipEvent.push_back(pSipEvent);
        m_vecSipEventMutex.unlock();
    }
    qDebug() << "Exit GBSip::run() ...";
}
