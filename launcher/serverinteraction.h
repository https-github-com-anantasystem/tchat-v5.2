#ifndef SERVERINTERACTION_H
#define SERVERINTERACTION_H

#include <QObject>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include "client.h"
#include "externalserver.h"

class serverInteraction : public QObject
{
    Q_OBJECT
public:
    serverInteraction();
    ~serverInteraction();
    void connectTo(const QString userName,const int port=2048, const QString ip="anantasystem.com");

private:
    int progresse;
    QString m_ip;
    QString m_psedo;
    int m_port;
    QString name;
    client* clients;
    externalServer* externalServ;
    cesar* encryptioncesar;
    void connect();
    void desconnect();
    void external(QMap<QString, QVariant> message);
    void createPacket(const QString message, const QString arg1="", const QString arg2="");
    void createServeur();
signals:
    void desconnected();
    void connected();
};

#endif // SERVERINTERACTION_H
