#ifndef WIDGET_H
#define WIDGET_H

#include <QSystemTrayIcon>
#include <QTcpSocket>
#include <QWidget>
#include <QSettings>
#include <QMenu>
#include <QTextDocumentFragment>
#include <QNetworkInterface>
#include <QScrollBar>
#include <QDateTime>
#include <QCheckBox>
#include <QCoreApplication>
#include <QTextBrowser>
#include <QGridLayout>
#include "parametre.h"
#include "serveur.h"
#include "client.h"
#include "client/chatBot/changeuserroom.h"
#include "client/chatBot/useraction.h"
#include "client/chatBot/chatbotinteraction.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    parametre parametres;
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    QList<QMap<QString,QString>> saveMessage;
    QList<QString> listeClient;
    client *clients;
    cesar *encryptioncesar;
    QString levelOfSecure;
    QSettings* settings;
    Ui::Widget *ui;
    bool condenser;
    bool lastMessageIsText;
    QSystemTrayIcon* sticon;
    QLabel *text;
    QLabel *lastText;
    QMenu *stmenu;
    QTcpSocket *socket;
    serveur *server;
    QString m_path;
    QList<QString> listeOfpPath;
    QString returnpseudo();
    QString generatemesage(QString message, QString pseudo);
    QString generatedate();
    void addmessage(QString message);
    void startTrayIcon();
    void changetransparency(Qt::ApplicationState state);
    void condesed();
    void autoconnect();
    void processechatbot(QString command);
    void on_pseudo_editingFinished();
    void connectClient();
    void openfile();
    void changeUserRole(QList<QString>usrRole);
    void changeUsersaloon(const QString username, const QString room);
    void remove( QLayout* layout );
private slots:
    //asseseur
    void errorServer(QString title, QString msg);
    void displayMessagelist(QString message);
    void displayMessagelistNoFormat(const QString message, const QString psedo);
    void displayFileOnMessageList(const QString comment, const QString NameOfFile);
    void displayEmbed(const QString name, const QString information,const QList<QList<QString >>liste);
    void deletClient(QString nameOfClient);
    void newuser(QString name);
    void recap();
    void executeCmd(const QString cmd);
    void mentionButonCliked();
    //les auto generer
    void on_parametrebutton_clicked();
    void on_pieceJointe_clicked();
    void on_sentbutton_clicked();
    void on_mesage_cursorPositionChanged(int arg1, int arg2);
};
#endif // WIDGET_H
