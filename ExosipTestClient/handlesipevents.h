#ifndef HANDLESIPEVENTS_H
#define HANDLESIPEVENTS_H

#include <eXosip2/eXosip.h>
#include <vector>
#include <QString>
#include <QThread>
#include <QVector>
#include <QMap>
#include <QVariant>
#include <QMutex>
#include <QTimer>

class GBSip;

class HandleSipEvents : public QThread
{
    Q_OBJECT

    enum {
        RECV_SERV_TIMEOUT = 60
    };

public:
    HandleSipEvents(QObject *parent = 0);
    ~HandleSipEvents();

    void AddSipEvent(eXosip_event_t *pSipEvent);

    bool Init();

protected:
    void run();

    bool InitSip();

    void OnMsgNewGet();

private:
    eXosip_t *m_pContextSip;
    eXosip_event_t *m_pSipEvent;
    sdp_message_t *m_pRemoteSdp;

    GBSip *m_pGBSipThread;

    int m_nExpires;
    char *m_pStrLocalIP;

};

#endif // HANDLESIPEVENTS_H
