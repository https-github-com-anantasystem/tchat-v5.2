#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include "connecttoclient.h"
#include "startserveur.h"

QT_BEGIN_NAMESPACE
namespace Ui { class launcher; }
QT_END_NAMESPACE

class launcher : public QMainWindow
{
    Q_OBJECT

public:
    startserveur startserver;
    connecttoclient connectclient;
    launcher(QWidget *parent = nullptr);
    ~launcher();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::launcher *ui;
    serverInteraction servinteract;
};
#endif // LAUNCHER_H
