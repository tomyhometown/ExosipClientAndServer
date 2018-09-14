#include "handlesipevents.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sstream>
#include <QDebug>
#include <QSettings>
#include <QFile>
#include <QCoreApplication>
#include <QDateTime>

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
    , m_pRemoteSdp(NULL)
    , m_nExpires(0)
{
    m_pStrLocalIP = new char[50];
}

HandleSipEvents::~HandleSipEvents()
{
    eXosip_quit(m_pContextSip);
}


bool HandleSipEvents::Init()
{
    if(!InitSip())
    {
        qDebug() << "HandleSipEvents::Init--init sip error.";
        return false;
    }

    return true;
}

void HandleSipEvents::run()
{
    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")  << ": Enter HandleSipEvents::run() ..." << endl;
    int nRet = -1;
    std::ostringstream strContent;
    strContent << "<?xml version=\"1.0\" encoding=\"gb2312\"?>\r\n";
    strContent << "<Control>\n";
    strContent << "    <CmdType>DeviceControl</CmdType>\n";
    strContent << "    <SN>" << 1 << "</SN>\n";
    strContent << "    <DeviceID>34020000001320000001</DeviceID>\n";
    strContent << "    <DragZoomIn>\n";
    strContent << "    </DragZoomIn>\n";
    strContent << "</Control>\n";
    std::string msg = strContent.str();
    osip_message_t *pMsg = nullptr;
    for(int i=0; i<2; i++)
    {
        qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << ": i = " << i;
        eXosip_lock(m_pContextSip);
        nRet = eXosip_message_build_request(m_pContextSip, &pMsg, "MESSAGE",
                                            "sip:192.168.1.241", "sip:192.168.1.145", nullptr);
        nRet = osip_message_set_body(pMsg, msg.c_str(), msg.length());
        nRet = osip_message_set_content_type(pMsg, "Application/MANSCDP+xml");
        int nTransactionID = eXosip_message_send_request(m_pContextSip, pMsg);
        eXosip_unlock(m_pContextSip);
        qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
                 << ": tid =" << nTransactionID << "     Ret =" << nRet << endl;
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
