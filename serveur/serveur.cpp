/*By Unel at 30/12/21
For Ananta Project*/
#include "serveur.h"

serveur::serveur()
{
    QCoreApplication::setOrganizationName("ananta system");
    QCoreApplication::setOrganizationDomain("https://anantasystem.com/");
    QCoreApplication::setApplicationName("tchat");
    QCoreApplication::setApplicationVersion("5.2");

    settings = new QSettings("settings.ini", QSettings::IniFormat);
    //encryptioncesar = new cesar(2);
    m_serveur = new QTcpServer(this);
    psedo=tr("non rensengner","dans le constructeur du client a letape du psedo. c'est le psedo par defaut");
    NbOfMessage=0;
    recoverallfile();
}
void serveur::emitlog(const QString log){emit serveur::log(log);}
void serveur::displayMessagelist(const QString newMessage, const QString psedo){ emit serveur::display(newMessage, psedo); }
void serveur::messageBox( QString title, QString msg){emit serveur::error(title,msg );}
int serveur::startserveur(const int port, const QString fileOfSave)
{
    m_FileOfSave = fileOfSave;
    m_serveur = new QTcpServer();
    if (!m_serveur->listen(QHostAddress::Any, port)) // Démarrage du serveur sur toutes les IP disponibles et sur le port 2048
    {// Si le serveur n'a pas été démarré correctement
        displayMessagelist(tr("le serveur n'a pas été lancée sur le port demandée"), tr("Serveur Bot"));
        if (!m_serveur->listen(QHostAddress::Any)) // Démarrage du serveur sur toutes les IP disponibles
        { // Si le serveur n'a pas été démarré correctement
            displayMessagelist(tr("Le serveur n'a pas pu être démarré. Raison : ") + m_serveur->errorString(),tr("Serveur Bot"));
            return 0;
        }else{
            displayMessagelist(tr("Le serveur a été démarré sur le port <strong>") + QString::number(m_serveur->serverPort()) + tr("</strong>.Des clients peuvent maintenant se connecter."), tr("Chat Bot"));
            QObject::connect(m_serveur, &QTcpServer::newConnection, this, &serveur::newconect);
        }
    }else{// Si le serveur a été démarré correctement
        displayMessagelist(tr("Le serveur a été démarré sur le port <strong>") + QString::number(m_serveur->serverPort()) + tr("</strong>.Des clients peuvent maintenant se connecter."), tr("Chat Bot"));
        QObject::connect(m_serveur, &QTcpServer::newConnection, this, &serveur::newconect);
    }
    encryptioncesar = new cesar(port);
    return m_serveur->serverPort();
}
//serveur
void serveur::sentmessagetoall(const QMap<QString, QVariant> &message)
{
    QByteArray pack;
    QDataStream out(&pack, QIODevice::WriteOnly);

    out << (int) 0; // On écrit 0 au début du paquet pour réserver la place pour écrire la taille
    out << message; // On ajoute le message à la suite
    out.device()->seek(0); // On se replace au début du paquet
    out << (int) (pack.size() - sizeof(int)); // On écrase le 0 qu'on avait réservé par la longueur du message
    //Sending
    for(int i = 0; i < clientsList.size(); i++)
    {
        clientsList[i]->getSocket()->write(pack);
    }
}
void serveur::sentmessageto(const QMap<QString, QVariant> &message, int NoUtilisateur)
{
    QByteArray pack;
    QDataStream out(&pack, QIODevice::WriteOnly);

    out << (int) 0; // On écrit 0 au début du paquet pour réserver la place pour écrire la taille
    out << message; // On ajoute le message à la suite
    out.device()->seek(0); // On se replace au début du paquet
    out << (int) (pack.size() - sizeof(int)); // On écrase le 0 qu'on avait réservé par la longueur du message
    //Sending
    clientsList[NoUtilisateur]->getSocket()->write(pack);
}
void serveur::sentmessagetoall(const QString type, QString message, QString pseudo){
    QMap<QString,QVariant> sendmap;
    sendmap["type"]=type;
    sendmap["message"]=encryptioncesar->chiffre(message);
    sendmap["pseudo"]=encryptioncesar->chiffre(pseudo);
    sendmap["time"]=QDateTime::currentDateTime();
    sentmessagetoall(sendmap);
}
void serveur::sentmessageto(const QString &message, const int NoUtilisateur, QString pseudo)
{
    if(psedo==""){
        pseudo = "serveur"+encryptioncesar->chiffre(psedo);
    }
    QMap<QString,QVariant> sendmap;
    sendmap["type"]="msg";
    sendmap["message"]=encryptioncesar->chiffre(message);
    sendmap["pseudo"]=encryptioncesar->chiffre(pseudo);
    sendmap["time"]=QDateTime::currentDateTime();
    sentmessageto(sendmap,NoUtilisateur);
}
void serveur::sentMessageToRole(const QString message, const int role, QString psedoOfSent){
    for(int i = 0; i<clientsList.size(); i++){
        if(clientsList[i]->getGrade()==role){
            sentmessageto(message,i,psedoOfSent);
        }
    }
}
void serveur::sendFileto(const QString path, const QString NameOfFile, const int NoUtilisateur){
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)){ return; }//on test louverture du ficher
    QByteArray ba = file.readAll();
    QMap<QString,QVariant> sendmap;//creation du descritif
    sendmap["type"]="attachmentFile";
    sendmap["nameOfFile"]=encryptioncesar->chiffre(NameOfFile);
    sendmap["version"]=QCoreApplication::applicationVersion();;
    sendmap["time"]=QDateTime::currentDateTime();
    sendmap["attachment"]=ba;
    sentmessageto(sendmap, NoUtilisateur);
}
void serveur::sentcomandto(const int usernaime, const QVariant &message, const QString arg, const QString arg2, const QString arg3)
{
    QMap<QString,QVariant> sendmap;
    sendmap["type"]="cmd";
    sendmap["message"]=message;
    sendmap["encrypt?"]=true;
    sendmap["arg"]=encryptioncesar->chiffre(arg);
    sendmap["arg2"]=encryptioncesar->chiffre(arg2);
    sendmap["arg3"]=encryptioncesar->chiffre(arg3);
    sendmap["pseudo"]="serveur"+encryptioncesar->chiffre(psedo);
    sendmap["time"]=QDateTime::currentDateTime();
    sentmessageto(sendmap,usernaime);

}
void serveur::sentcomandto(const int usernaime, const QString message, const QVariant arg)
{
    QMap<QString,QVariant> sendmap;
    sendmap["type"]="cmd";
    sendmap["encrypt?"]=false;
    sendmap["message"]=message;
    sendmap["arg"]=arg;
    sendmap["pseudo"]="serveur"+encryptioncesar->chiffre(psedo);
    sendmap["time"]=QDateTime::currentDateTime();
    sentmessageto(sendmap,usernaime);

}
void serveur::sentcommande(const QString commande,const QString arg,const QString arg2){
    QMap<QString,QVariant> sendmap;
    sendmap["type"]="cmd";
    sendmap["message"]=commande;
    sendmap["arg"]=encryptioncesar->chiffre(arg);
    sendmap["arg2"]=encryptioncesar->chiffre(arg2);
    sendmap["pseudo"]="serveur"+encryptioncesar->chiffre(psedo);
    sendmap["time"]=QDateTime::currentDateTime();
    sentmessagetoall(sendmap);
}
void serveur::newconect()
{
    utilisateur* newClient = new utilisateur(m_serveur->nextPendingConnection());
    if(clientsList.empty()){
        newClient->promote(2); //on le passe host
        newClient->changeRoom("talk");
    }
    clientsList.append(newClient);
    QObject::connect(clientsList.last()->getSocket(), &QTcpSocket::readyRead, this, &serveur::datareceived);
    QObject::connect(clientsList.last()->getSocket(), &QTcpSocket::disconnected, this ,&serveur::disconnectclients);
    emitlog(tr("un client s'est connécter mais ne s'est pas encors identifier.", "dans les log"));
    emit serveur::ActionOnUser(newClient,true);
}
void  serveur::outOfWating(int usernaime, const QString newpsedo)
{
    if(clientsList[usernaime]->getRoom()!="waiting"){// si  il est pas en sale d'atente{
        sentcommande("silentNameChange",clientsList[usernaime]->getpseudo(),newpsedo);//on dit qu'il s'est renomée
        emitlog(tr("un client vien de sidentifier : ", "dans les log")+ clientsList[usernaime]->getpseudo());
        srand (time(NULL));
        int random = rand() % 4 + 1;
        if(random == 1){
            sentmessagetoall("msg",newpsedo + tr(" est connecté."),tr("Tchat Bot"));
        }else if(random == 2){
             sentmessagetoall("msg",newpsedo + tr(" vient d'arriver dans le salon."),tr("Tchat Bot"));
        }else if(random == 3){
            sentmessagetoall("msg",newpsedo + tr(" vient de nous rejoindre."),tr("Tchat Bot"));
        }else if(random == 4){
            sentmessagetoall("msg",tr("Il ne nous manquait plus que ")+newpsedo+ tr(" heureusement il nous a rejoint."),tr("Tchat Bot"));
        }
        for(int i = 0; i < saveMessage.size()-1; i++)
        {
            sentmessageto(saveMessage[i],usernaime);
        }
        for(int i = 0; i < clientsList.size()-1; i++){
            sentcomandto(usernaime,"isconnected",clientsList[i]->getpseudo());
        }
        emit serveur::ActionOnUser(clientsList[usernaime],true);
    }else if (clientsList[usernaime]->getRoom()=="waiting"){
        emitlog(tr("un client vien d'etre mis en salle d'atente : ", "dans les log")+ clientsList[usernaime]->getpseudo());
        srand (time(NULL));
        int random = rand() % 3 + 1;
        if(random == 1){
            sentmessagetoall("msg",clientsList[usernaime]->getpseudo() + tr(" est passer en salle d'atente."),tr("Tchat Bot"));
        }else if(random == 2){
             sentmessagetoall("msg",clientsList[usernaime]->getpseudo() + tr(" vient de changer dans le salon."),tr("Tchat Bot"));
        }else if(random == 3){
            sentmessagetoall("msg",clientsList[usernaime]->getpseudo() + tr(" vient de nous quitée."),tr("Tchat Bot"));
        }
        clientsList[usernaime]->editpseudo(clientsList[usernaime]->getpseudo()+" ("+clientsList[usernaime]->getRoom()+")");//on le nome
        sentmessageto(tr("vous avez ete placée en sale d'atente...","lors d'une connexion"),usernaime,tr("tchat bot"));
        sentMessageToRole(clientsList[usernaime]->getpseudo()+tr(" vien de se connecter... il a été placée en salle d'atente taper /acept","lors d'une connexion"),1);
        sentMessageToRole(clientsList[usernaime]->getpseudo()+tr(" vien de se connecter... il a été placée en salle d'atente taper /acept","lors d'une connexion"),2);
        sentmessageto(tr("les administarteur est l'host on été prevenu","lors d'une connexion"),usernaime,tr("tchat bot"));
        emitlog(clientsList[usernaime]->getpseudo()+tr(" vien de se connecter... il a été placée en salle d'atente","lors d'une connexion"));
        sentcommande("silentNameChange",clientsList[usernaime]->getpseudo(),clientsList[usernaime]->getpseudo()+" ("+clientsList[usernaime]->getRoom()+")");//on dit qu'il s'est renomée
    }
}
void serveur::connect( QMap<QString, QVariant> &connectpack, int usernaime){
    QString username =encryptioncesar->deChiffre(connectpack["pseudo"].toString());

    for(int i = 0; i < clientsList.size(); i++)
    {
        if(clientsList[usernaime]->getpseudo().remove(" ")==""){
            sentcomandto(usernaime,"psedoNull");
            return;
        }
        if(clientsList[i]->getpseudo().remove(" ")=="" && i != usernaime){
            sentcomandto(usernaime,"pseudoalreadyuse");
            return;
        }
        if(clientsList[i]->getpseudo()==connectpack["pseudo"] && i != usernaime){//si c'est le meme on coupe et on envoie une erreur
            sentcomandto(usernaime,"pseudoalreadyuse");
            return;
        }else if(clientsList[i]->getpseudo().remove(" ").remove(".")==connectpack["arg"].toString().remove(" ").remove(".") && i != usernaime){//si c'est resembleaut on coupe et on envoie une erreur
            sentcomandto(usernaime,"pseudoresembling");
            return;
        }
    }

    clientsList[usernaime]->editversion(connectpack["version"].toString());
    if(clientsList[usernaime]->getRoom()!="waiting"){// si  il est pas en sale d'atente{
        clientsList[usernaime]->editpseudo(username);//on le nomme
        emitlog(tr("un client vien de sidentifier : ", "dans les log")+ clientsList[usernaime]->getpseudo());
        srand (time(NULL));
        int random = rand() % 4 + 1;
        if(random == 1){
            sentmessagetoall("msg",clientsList[usernaime]->getpseudo() + tr(" est connecté."),tr("Tchat Bot"));
        }else if(random == 2){
             sentmessagetoall("msg",clientsList[usernaime]->getpseudo() + tr(" vient d'arriver dans le salon."),tr("Tchat Bot"));
        }else if(random == 3){
            sentmessagetoall("msg",clientsList[usernaime]->getpseudo() + tr(" vient de nous rejoindre."),tr("Tchat Bot"));
        }else if(random == 4){
            sentmessagetoall("msg",tr("Il ne nous manquait plus que ")+clientsList[usernaime]->getpseudo()+ tr(" heureusement il nous a rejoint."),tr("Tchat Bot"));
        }
        for(int i = 0; i < saveMessage.size()-1; i++)
        {
            sentmessageto(saveMessage[i],usernaime);
        }
        for(int i = 0; i < clientsList.size()-1; i++){
            sentcomandto(usernaime,"isconnected",clientsList[i]->getpseudo());
        }
        sentmessagetoall(connectpack);
    }else{//il est en sale d'atente
        clientsList[usernaime]->editpseudo(username+" ("+clientsList[usernaime]->getRoom()+")");//on le nome
        sentmessageto(tr("vous avez ete placée en sale d'atente...","lors d'une connexion"),usernaime,tr("tchat bot"));
        sentMessageToRole(clientsList[usernaime]->getpseudo()+tr(" vien de se connecter... il a été placée en salle d'atente taper /acept","lors d'une connexion"),1);
        sentMessageToRole(clientsList[usernaime]->getpseudo()+tr(" vien de se connecter... il a été placée en salle d'atente taper /acept","lors d'une connexion"),2);
        sentmessageto(tr("les administarteur est l'host on été prevenu","lors d'une connexion"),usernaime,tr("tchat bot"));
        emitlog(clientsList[usernaime]->getpseudo()+tr(" vien de se connecter... il a été placée en salle d'atente","lors d'une connexion"));
        connectpack["pseudo"]=encryptioncesar->chiffre(encryptioncesar->deChiffre(connectpack["pseudo"].toString())+" ("+clientsList[usernaime]->getRoom()+")");
        sentmessagetoall(connectpack);
    }

}
void serveur::datareceived()
{
    int index = findIndex(qobject_cast<QTcpSocket*>(sender()));
      utilisateur* sendingClient = clientsList[index];
      QTcpSocket* socket = sendingClient->getSocket();

      if(socket == nullptr) {
        displayMessagelist(tr("erreur lors de la recherche du client qui a envoyé le paquet"),tr("serveur bot"));
        emitlog(tr("erreur lors de la recherche du client qui a envoyé le paquet", "dans les log"));
        return; //Error
      }
      QDataStream in(socket);
        while(1){
            if(sendingClient->getmessageSize() == 0) { //Try to catch
                if(socket->bytesAvailable() < static_cast<int>(sizeof(int))){
                    return;
                }
                int messageSize;
                in >> messageSize;
                sendingClient->setmessageSize(messageSize);
            }
            if(socket->bytesAvailable() < sendingClient->getmessageSize()){ //Part of the message missing
            return;
            }
            QMap<QString, QVariant>message;
            in >> message;
            sendingClient->setmessageSize(static_cast<int>(0));
        if(message["type"]=="cmd"){//une commende
            message["receviedTime"]=QDateTime::currentDateTime();
            message["arg"]=encryptioncesar->deChiffre(message["arg"].toString());
            message["arg2"]=encryptioncesar->deChiffre(message["arg2"].toString());
            message["pseudo"]=encryptioncesar->deChiffre(message["pseudo"].toString());
            processcomand(message,index);
        }else if(message["type"]=="msg"){
            if(clientsList[index]->getRoom()!="waiting"){// si il est pas en sale d'atente
                sentmessagetoall(message);
                if(settings->value("settings/SaveMessage").toBool()){
                    writetofile(message);
                }
            }else{//si il est en sale d'atente
                sentmessageto(tr("vous ne pouvez pas envoyer de message car vous ete en sale d'atente","lors de la reception de message si le client est en salle d'atente"),index,tr("Tchat Bot"));
            }
        }else if(message["type"]=="attachment"){
            sentmessagetoall(message);
            message["pseudo"]=encryptioncesar->deChiffre(message["pseudo"].toString());
            message["message"]=encryptioncesar->deChiffre(message["message"].toString());
            message["nameOfFile"]=encryptioncesar->deChiffre(message["nameOfFile"].toString());
            // Ask the user where he/she wants to save the file
            QDir dir;
            dir.mkpath("temp");//on crée le repertoir
            QFile file("temp/"+message["nameOfFile"].toString());

            if (!file.fileName().isEmpty()) {// Check that the path is valid
                file.open(QIODevice::WriteOnly);
                QByteArray ba = message["attachment"].toByteArray();//on crée le flux
               file.write(ba);
               qDebug()<<file;
               file.close();// Close the file
            }
        }else if(message["type"]=="connection"){
            connect(message, index);
        }else{
        emit serveur::noInternal(message, index);
        }
        sendingClient->setmessageSize(static_cast<int>(0));
    }
}

void serveur::disconnectclients()
{
    QTcpSocket* disconnectingClientSocket = qobject_cast<QTcpSocket*>(sender());
    int index = findIndex(disconnectingClientSocket);
    utilisateur* disconnectingClient = clientsList[index];//on recherche le cilent

    emit serveur::ActionOnUser(disconnectingClient, false);//on dit qu'il a été suprimée
    if(disconnectingClientSocket == nullptr) //Error
    {
        displayMessagelist(tr("Erreur fatal: les clients ne peuvent pas être supprimés. fermeture!"),tr("Serveur Bot"));
        messageBox(tr("Erreur fatal"),tr("Les clients ne peuvent pas être supprimés. fermeture!"));
        emitlog("/!\\"+tr("un client n'a pas pue etre suprimée", "dans les log")+"/!\\");
        qApp->quit();
        return;
    }
    emitlog(tr("un client vien d'etre suprimée : ", "dans les log")+disconnectingClient->getpseudo());
    sentcommande("disconnected",disconnectingClient->getpseudo());

    clientsList.removeOne(disconnectingClient);
    disconnectingClient->deleteLater();
}
int serveur::findIndex(QTcpSocket* socket)
{
    int index = 0;
    while(clientsList[index]->getSocket() != socket)
    {
        index++;
    }
    return index;
}
void serveur::writetofile(QMap<QString, QVariant> FluxFile)
{
    saveMessage.push_back(FluxFile);
    ++NbOfMessage;
    QFile file(m_FileOfSave);
    if (!file.open(QIODevice::WriteOnly)){
            displayMessagelist(tr("Il est impossible d'écrire dans le fichier."),tr("Tchat Bot"));
            return;
        }
    QDataStream out(&file);
    out <<saveMessage;
}
void serveur::recoverallfile()
{
    QFile fichier(m_FileOfSave);

   if(fichier.open(QIODevice::ReadOnly))
   {
       QByteArray paquet;
       paquet = fichier.readAll();// Lecture entier de notre fichier en octet
       QDataStream out(paquet);
       out>>saveMessage;

   }
   else
   {
       displayMessagelist(tr("Le fichier est inaxcessible"), tr("System Tchat Bot"));

   }
}
void serveur::processcomand(QMap<QString, QVariant> command, int noclient)
{
    if (command["message"].toString()=="change_psedo") {// changer psedo)
        //on verifie que l'utilisateur est pas en salle d'attente
        if(clientsList[noclient]->getRoom()=="waiting"){
            sentcomandto(noclient,"not_change_psedo_in_waiting");
            return;
        }
        for(int i = 0; i < clientsList.size(); i++)
        {
            if(clientsList[i]->getpseudo().remove(" ")=="" && i != noclient){
                sentcomandto(noclient,"pseudoalreadyuse");
                return;
            }
            if(clientsList[i]->getpseudo()==command["pseudo"] && i != noclient){//si c'est le meme on coupe et on envoie une erreur
                sentcomandto(noclient,"pseudoalreadyuse");
                return;
            }else if(clientsList[i]->getpseudo().remove(" ")==command["arg"].toString().remove(" ") && i != noclient){//si c'est resembleaut on coupe et on envoie une erreur
                sentcomandto(noclient,"pseudoresembling");
                return;
            }
            sentcommande("changePsedo",clientsList[noclient]->getpseudo(),command["arg"].toString());
            sentmessagetoall("msg",clientsList[noclient]->getpseudo()+" a changer son psedo en "+ command["arg"].toString(),"Tchat Bot");
            emitlog(clientsList[noclient]->getpseudo()+tr(" a changer son psedo en : ", "dans les log")+command["arg"].toString());
            clientsList[noclient]->editpseudo(command["arg"].toString());
        }
    }else if(command["message"].toString()=="file?") {
        sendFileto(command["arg"].toString(),command["nameOfFile"].toString(),noclient);
        emitlog(clientsList[noclient]->getpseudo()+tr("a demander le fichier : ", "dans les log")+command["nameOfFile"].toString());
    }else if (command["message"].toString()=="clearForAll"){
        if (clientsList[noclient]->getGrade()==1||clientsList[noclient]->getGrade()==2){
            sentcommande("clear");
            saveMessage.clear();
            emitlog(clientsList[noclient]->getpseudo()+tr(" a suprimée tout les message", "dans les log"));
        }else{
            sentmessageto(tr("vous n'avais pas le droit de faire cette commende : clear est soumis a un rôle admin ou host","lors de lexecution d'une commende"), noclient);
            emitlog(clientsList[noclient]->getpseudo()+tr(" a voulue suprimée tout les messagemais n'a pas pue !", "dans les log"));
        }
    }else if(command["message"].toString()=="changeUsrRole"){
        int clientname=-1;
        for(int i = 0; i<clientsList.size(); i++){
            if(clientsList[i]->getpseudo()==command["arg"]){
                clientname=i;
            }
        }if(clientsList[noclient]->getGrade()==0){
            sentmessageto(tr("vous n'avais pas le droit de faire cette commende : changeUsrRole est soumis a un rôle admin ou host","lors de lexecution d'une commende"), noclient);
            return;
        }else if(clientsList[noclient]->getGrade()<command["arg2"].toInt()){
            sentmessageto(tr("vous n'avais pas le droit de donée un grade plus élever que le votre","lors de lexecution d'une commenoutde"), noclient);
            return;
        }else if(command["arg2"].toInt()==2){
            sentmessageto(tr("il est imposible de donée le grade host","lors de lexecution d'une commende"), noclient);
            return;
        }else if(clientsList[clientname]->getGrade()==2){
             sentmessageto(tr("il est imposible changer le rôle d'un host","lors de lexecution d'une commende"), noclient);
        }else{
            clientsList[clientname]->promote(command["arg2"].toInt());
            sentmessagetoall("msg",clientsList[noclient]->getpseudo()+tr(" a changer le grade de ")+ clientsList[clientname]->getpseudo()+ tr(" en ")+clientsList[clientname]->getGradeString(),tr("Tchat Bot"));
            emitlog(clientsList[noclient]->getpseudo()+tr(" a changer le grade de ", "dans les log")+ clientsList[clientname]->getpseudo()+ tr(" en ", "dans les log")+clientsList[clientname]->getGradeString());
        }
    }else if(command["message"].toString()=="changeUsrRoom"){
        int clientname=-1;
        for(int i = 0; i<clientsList.size(); i++){
            if(clientsList[i]->getpseudo().remove(" ")==command["arg"].toString().remove(" ")){//on prend le n° du client
                clientname=i;
            }
        }
        if(clientname<0){
            return;
        }
        if(clientsList[noclient]->getGrade()==0){// si c'est un user
            sentmessageto(tr("vous n'avais pas le droit de faire cette commende : changeUsrRoom est soumis a un rôle admin ou host","lors de lexecution d'une commende"), noclient);
            return;
        }else if(clientsList[clientname]->getGrade()==2){//si on veut changer un host
            sentmessageto(tr("vous n'avais pas le droit de faire cette commende : changeUsrRoom ne peut etre fait sur un host","lors de lexecution d'une commende"), noclient);
            return;
        }else{
            sentcomandto(clientname,"clear");
            for(int i = 0; i < saveMessage.size()-1; i++)
            {
                sentmessageto(saveMessage[i],clientname);
            }
            sentmessageto(tr("vous avez été changer de salle par ")+clientsList[noclient]->getpseudo()+ tr(" vous éte maintenant en salle : ")+command["arg2"].toString(), clientname);
            emitlog(command["arg"].toString()+tr(" a été changer de sale par : ", "dans les log")+clientsList[noclient]->getpseudo()+tr("il est maintenant en salle :", "dans les log")+command["arg2"].toString());
            const QString name =clientsList[clientname]->getpseudo().remove(" ("+clientsList[clientname]->getRoom()+")");
            clientsList[clientname]->changeRoom(command["arg2"].toString());
            outOfWating(clientname,name);
            clientsList[clientname]->editpseudo(name);//on le renome
        }
    }else if(command["message"].toString()=="ping"){
        QTime actualTime = QTime::currentTime();
        const int time = command["time"].toDateTime().time().msecsTo(actualTime);
        sentcomandto(noclient,"pong",QString::number(time));
    }else if(command["message"].toString()=="info"){
        QTime actualTime = QTime::currentTime();
        const int ping =command["time"].toDateTime().time().msecsTo(command["receviedTime"].toDateTime().time());//te temps entre l'envoie est la reception
        const int internalPing = command["receviedTime"].toDateTime().time().msecsTo(actualTime);
        sentcomandto(noclient,"ReInfo",QString::number(ping),QString::number(internalPing),QString::number(clientsList.size()));
    }else if(command["message"].toString()=="debug"){
        if(clientsList[noclient]->getGrade()==0){
            sentmessageto(tr("vous n'avais pas le droit de faire cette commende : debug est soumis a un rôle admin ou host","lors de lexecution d'une commende"), noclient);
        }else{
            QList<QVariant> debug =recapEmbed();
            sentcomandto(noclient,"reDebug",debug);
        }
    }else{
        messageBox(tr("Erreur"), tr("Un paquet de commande a été reçu mais la commande est incomprise."));
}}
void serveur::recap(){
    const auto fichier = "raport.txt";
    QFile file(fichier);//on crée le fichier
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        messageBox(tr("erreur de permision","dans les ouverture de fichier"),tr("le fichier ne peut pas etre lu sans doute une erreur d'autorisation","dans les ouverture de fichier"));
        return;
    }
    QTextStream out(&file);
    out << tr("-----------------generate-by-Ananta-System-5.2-----------------","dans la generation de recapitulatif")<< Qt::endl;
    out << tr("Le serveur a été démarré sur le port ","dans la generation de recapitulatif")+"<strong>" + QString::number(m_serveur->serverPort()) + "</strong>."<< Qt::endl;
    out << tr("la derniere erreur a été : ","dans la generation de recapitulatif")<<m_serveur->errorString()<<tr(" avec le code d'erreur :","dans la generation de recapitulatif")<<m_serveur->serverError()<< Qt::endl;
    out << tr("il y a une connection en atente : ","dans la generation de recapitulatif")<<m_serveur->hasPendingConnections()<< Qt::endl;
    out << tr("le serveur ecoute les connexion : ","dans la generation de recapitulatif")<<m_serveur->isListening()<< Qt::endl;
    out << tr("le nombre max de connexion en atente est : ","dans la generation de recapitulatif")<<m_serveur->maxPendingConnections()<< Qt::endl;
    out << tr("l'adresse du serveur : ","dans la generation de recapitulatif")<<" scolpeID :"<<m_serveur->serverAddress().scopeId()<<" to string : "<<m_serveur->serverAddress().toString()<< Qt::endl;
    out << tr("vos ip sont ","dans la generation de recapitulatif")<< Qt::endl;
    {//on met des crocher pour deconstruire la liste a la fin
       QList<QHostAddress> listes;
       listes = QNetworkInterface::allAddresses();
       for(int i = 0; i< listes.size(); i++)
       {
       out << (listes.at(i).toString()) << Qt::endl;
       }
    }
    out << tr("-----------------generate-by-Ananta-System-5.2-----------------","dans la generation de recapitulatif")<< Qt::endl;
}
QList<QVariant> serveur::recapEmbed(){

    QList<QVariant>liste;//creation de la liste principale

    const QList<QVariant>one{tr("port du serveur","dans la generation de recapitulatif"),QString::number(m_serveur->serverPort())};
    liste.push_back(one);

    const QList<QVariant>four{tr("derniere erreur : ","dans la generation de recapitulatif"),m_serveur->errorString()};
    liste.push_back(four);

    if(m_serveur->hasPendingConnections()){
        const QList<QVariant>five{tr("il y a une connexion en attente : ","dans la generation de recapitulatif"),tr("oui")};
        liste.push_back(five);
    }else{
        const QList<QVariant>five{tr("il y a une connexion en attente : ","dans la generation de recapitulatif"),tr("non")};
        liste.push_back(five);
    }

    if(m_serveur->isListening()){
        const QList<QVariant>six{tr("le serveur ecoute les connexion : ","dans la generation de recapitulatif"),tr("oui")};
        liste.push_back(six);
    }else{
        const QList<QVariant>six{tr("le serveur ecoute les connexion : ","dans la generation de recapitulatif"),tr("non")};
        liste.push_back(six);
    }
    const QList<QVariant>two{tr("le nombre max de connexion en atente est : ","dans la generation de recapitulatif"),QString::number(m_serveur->maxPendingConnections())};
    liste.push_back(two);

    const QList<QVariant>seven{ tr("l'adresse du serveur : ","dans la generation de recapitulatif")," scolpeID :"+m_serveur->serverAddress().scopeId()+" to string : "+m_serveur->serverAddress().toString()};
    QVariant test = seven;
    liste.push_back(test);

return liste;
}
