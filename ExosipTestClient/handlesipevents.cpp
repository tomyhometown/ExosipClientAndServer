#include "handlesipevents.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QCoreApplication>
#include <QDateTime>

#include "gbsip.h"

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

using namespace std;

HandleSipEvents::HandleSipEvents(QObject *parent)
    : QThread(parent)
    , m_pContextSip(NULL)
    , m_pSipEvent(NULL)
    , m_pRemoteSdp(NULL)
    , m_nExpires(0)
    , m_pGBSipThread(NULL)
{
    m_pStrLocalIP = new char[50];
}

HandleSipEvents::~HandleSipEvents()
{
    if(m_pGBSipThread) m_pGBSipThread->stop();
    delete m_pGBSipThread;
    eXosip_quit(m_pContextSip);
}


bool HandleSipEvents::Init()
{
    if(!InitSip())
    {
        qDebug() << "HandleSipEvents::Init--init sip error.";
        return false;
    }

    m_pGBSipThread = new GBSip(m_pContextSip);
    m_pGBSipThread->start();

    return true;
}

void HandleSipEvents::run()
{
    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")  << ": Enter HandleSipEvents::run() ..." << endl;

    while(1)
    {
        if(m_pGBSipThread && m_pGBSipThread->IsVecSipEventEmpty())
        {
            usleep(1500);
            continue;
        }

        m_pGBSipThread->GetFirstSipEvent(m_pSipEvent);//    m_pNvr = FactoryNVR::getInstance()->createNVR();
//        qDebug() << "HandleSipEvents::eXosip event type: " << m_pSipEvent->type;
        qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << ": HandleSipEvents::eXosip event tid:  " << m_pSipEvent->tid;
        switch (m_pSipEvent->type) {
        case EXOSIP_MESSAGE_NEW://23,新的消息到来
            OnMsgNewGet();
            break;
        default:
            break;
        }
    }
    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << ": Exit HandleSipEvents::run() ...";
}

bool HandleSipEvents::InitSip()
{
    int nRet = -1;

    m_pContextSip = eXosip_malloc();
    nRet = eXosip_init(m_pContextSip);
    if(nRet != OSIP_SUCCESS)
    {
        qDebug() << "eXosip init failed!";
        return false;
    }
    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << ": eXosip init succeed!";

    eXosip_set_user_agent(m_pContextSip, "Embedded Net DVR/NVR/DVS");
    nRet = eXosip_listen_addr(m_pContextSip, IPPROTO_UDP, NULL, 5060, AF_INET, 0);
    if(nRet != OSIP_SUCCESS)
    {
        eXosip_quit(m_pContextSip);
        qDebug() << "Transport layer init failed!";
        return false;
    }

    nRet = eXosip_guess_localip (m_pContextSip, AF_INET, m_pStrLocalIP,  50);
    if(nRet != OSIP_SUCCESS)
    {
        eXosip_quit(m_pContextSip);
        qDebug() << "Guess local ip failed!";
        return false;
    }

    return true;
}

void HandleSipEvents::OnMsgNewGet()
{
    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") <<  ": EXOSIP_MESSAGE_NEW" << endl;
    if(MSG_IS_MESSAGE(m_pSipEvent->request))//如果接受到的消息类型是MESSAGE
    {
        osip_body_t *reqBody;
        osip_message_t *pAnsMsg = NULL;

        if(osip_message_get_body(m_pSipEvent->request, 0, &reqBody) != OSIP_UNDEFINED_ERROR)
        {
//            qDebug() << "Get msg body is: " << reqBody->body;

            //按照规则，需要回复OK信息
            eXosip_lock(m_pContextSip);

            int nRet = eXosip_message_build_answer(m_pContextSip, m_pSipEvent->tid, 200, &pAnsMsg);
            if(nRet != OSIP_SUCCESS)
            {
                qDebug() << "This new msg is invalid! Cann't response!";
                nRet = eXosip_message_send_answer(m_pContextSip, m_pSipEvent->tid, 400, NULL);
                eXosip_unlock(m_pContextSip);
            }
            else
            {
                nRet = eXosip_message_send_answer(m_pContextSip, m_pSipEvent->tid, 200, pAnsMsg);
                eXosip_unlock(m_pContextSip);

 //               qDebug() << "m_pSipEvent->request content-type: " << osip_message_get_content_type(m_pSipEvent->request)->subtype;
                QStringList msgBody = QString(reqBody->body).split("\r\n", QString::SkipEmptyParts);
                QMap<QString, QString> mapAttr;
                QString key, val;
                for(int i=2; i<msgBody.count()-1; i++)
                {
                    while(msgBody[i].size()>0 && msgBody[i].at(0).isSpace())
                    {
                        msgBody[i].remove(0, 1);
                    }
                    key = msgBody[i].mid(1, msgBody[i].indexOf(">")-msgBody[i].indexOf("<")-1);
                    val = msgBody[i].mid(msgBody[i].indexOf(">")+1, msgBody[i].indexOf("</")-msgBody[i].indexOf(">")-1);
                    mapAttr[key] = val;
                }

//                StreamThread* pThread = m_pThreadFac->createStreamThread(
//                            mapAttr["CmdType"].toStdString(), mapAttr, m_pContextSip, m_sipInfo);
            }
        }
    }
    eXosip_event_free(m_pSipEvent);
    m_pSipEvent = NULL;
}

