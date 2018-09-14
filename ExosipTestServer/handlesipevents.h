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

class HandleSipEvents : public QThread
{
    Q_OBJECT

    enum {
        RECV_SERV_TIMEOUT = 60
    };

public:
    HandleSipEvents(QObject *parent = 0);
    ~HandleSipEvents();

    bool Init();

protected:
    void run();

    bool InitSip();

private:
    eXosip_t *m_pContextSip;
    sdp_message_t *m_pRemoteSdp;

    int m_nExpires;
    char *m_pStrLocalIP;

};

#endif // HANDLESIPEVENTS_H
