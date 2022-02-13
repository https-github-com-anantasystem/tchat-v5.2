#include "externalserver.h"
#include "ui_externalserver.h"

externalServer::externalServer(int *valueProgress, QString Adresse, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::externalServer)
{
    ui->setupUi(this);
    ui->ip->setText(Adresse);
    ui->progressBar->setValue(*valueProgress);
}

externalServer::~externalServer()
{
    delete ui;
}

void externalServer::changeProgress(const int newValue){
    ui->progressBar->setValue(newValue);
}
