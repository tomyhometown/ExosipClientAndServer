#ifndef GBSIP_H
#define GBSIP_H

#include <eXosip2/eXosip.h>
#include <QThread>
#include <QVector>
#include <QMutex>

class GBSip : public QThread
{
    Q_OBJECT
public:
    GBSip(eXosip_t *pContextSip, QObject *parent = 0);
    ~GBSip();

    void GetFirstSipEvent(eXosip_event_t *&pSipEvent);
    bool IsVecSipEventEmpty();
    void clearSipEvents();
    void stop();

protected:
    void run();

private:
    bool m_bStop;
    eXosip_t *m_pContextSip;
    QVector<eXosip_event_t*> m_vecSipEvent;
    QMutex m_vecSipEventMutex;
};

#endif // GBSIP_H
