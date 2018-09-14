#ifndef SIPSERVICE_H
#define SIPSERVICE_H

#include <QObject>

class HandleSipEvents;

class SipService : public QObject
{
    Q_OBJECT

public:
    SipService();
    ~SipService();

    bool InitSipService();

private:
    HandleSipEvents *m_pHandleSip;

};

#endif // SIPSERVICE_H
