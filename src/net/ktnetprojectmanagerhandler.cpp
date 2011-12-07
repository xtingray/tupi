/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "ktnetprojectmanagerhandler.h"
#include "ktnetprojectmanagerparams.h"
#include "ktprojectresponse.h"
#include "tdebug.h"
#include "tosd.h"

#include "ktprojectcommand.h"
#include "ktcommandexecutor.h"
#include "ktnetsocket.h"
#include "ktprojectrequest.h"
#include "ktnewprojectpackage.h"
#include "ktconnectpackage.h"
#include "ktsavenetproject.h"
#include "ktopenpackage.h"
#include "ktchatpackage.h"
#include "ktnotificationparser.h"
#include "ktprojectlistparser.h"
#include "ktprojectparser.h"
#include "ktrequestparser.h"
#include "ktackparser.h"
#include "ktcommunicationparser.h"
#include "ktrequestbuilder.h"
#include "ktproject.h"
#include "ktlistprojectdialog.h"
#include "ktchat.h"
#include "ktnotice.h"

#include <QTemporaryFile>
#include <QTabWidget>
#include <QDesktopWidget>
#include <QMessageBox>

struct KTNetProjectManagerHandler::Private
{
    KTNetProjectManagerParams *params;
    KTNetSocket *socket;
    QString projectName;
    QString author;
    KTProject *project;
    
    QString sign;
    bool ownPackage;
    bool doAction;
    
    QTabWidget *communicationModule;
    KTChat *chat;
    KTNotice *notices;

    bool projectIsOpen; 
    bool dialogIsOpen;
    KTListProjectDialog *dialog;
};

KTNetProjectManagerHandler::KTNetProjectManagerHandler(QObject *parent) : KTAbstractProjectHandler(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->socket = new KTNetSocket(this);
    connect(k->socket, SIGNAL(disconnected()), this, SLOT(connectionLost()));

    k->project = 0;
    k->params = 0;
    k->ownPackage = false;
    k->doAction = true;
    k->projectIsOpen = false;
    k->dialogIsOpen = false;
    
    k->communicationModule = new QTabWidget;
    k->communicationModule->setWindowTitle(tr("Communications"));
    k->communicationModule->setWindowIcon(QPixmap(THEME_DIR + "icons/chat.png"));

    k->chat = new KTChat;
    k->communicationModule->addTab(k->chat, tr("Chat"));
    
    connect(k->chat, SIGNAL(requestSendMessage(const QString&)), this, SLOT(sendChatMessage(const QString&)));
    
    k->notices = new KTNotice;
    k->communicationModule->addTab(k->notices, tr("Notices"));
    
    // connect(k->notices, SIGNAL(requestSendMessage(const QString&)), this, SLOT(sendNoticeMessage(const QString&)));
}

KTNetProjectManagerHandler::~KTNetProjectManagerHandler()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    k->chat->close();
    delete k;
}

void KTNetProjectManagerHandler::handleProjectRequest(const KTProjectRequest* request)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("net");
    #endif

    // This comes from the project before the command execution
    // SQA: Save a copy of the events or queued packages and resend to the GUI when the "Ok" package 
    // comes from the server 

    if (k->socket->state() == QAbstractSocket::ConnectedState) {
        #ifdef K_DEBUG
               tDebug("net") << "KTNetProjectManagerHandler::handleProjectRequest() - SENDING PACKAGE: " << request->xml();
        #endif

        if (request->isValid()) {
            emit sendCommand(request, true);
            k->socket->send(request->xml());
        } else {
            #ifdef K_DEBUG
                   tFatal() << "KTNetProjectManagerHandler::handleProjectRequest() - INVALID REQUEST! ID: " << request->id();
            #endif
        }
    }
}

bool KTNetProjectManagerHandler::commandExecuted(KTProjectResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("net");
    #endif

    if (response->mode() == KTProjectResponse::Do) {
        k->doAction = true;
        return true;
    } 

    KTProjectRequest request = KTRequestBuilder::fromResponse(response);
    k->doAction = false;

    if (response->mode() != KTProjectResponse::Undo && response->mode() != KTProjectResponse::Redo) {
        handleProjectRequest(&request);
    } else { 
        if (k->socket->state() == QAbstractSocket::ConnectedState) {
            if (request.isValid())
                k->socket->send(request.xml());
        }
    }

    return true;
}

bool KTNetProjectManagerHandler::saveProject(const QString &fileName, KTProject *project)
{
    KTSaveNetProject saver(k->params->server(), k->params->port());
    
    return saver.save(fileName, project);
}

bool KTNetProjectManagerHandler::loadProject(const QString &fileName, KTProject *project)
{
    // SQA: Check why this two variables are required as parameters here
    Q_UNUSED(fileName);
    Q_UNUSED(project);

    /*
    if (k->socket->state() != QAbstractSocket::ConnectedState)
        return false;
    
    return loadProjectFromServer(k->params->projectName());
    */

    return true;
}

void KTNetProjectManagerHandler::loadProjectFromServer(const QString &projectID)
{
    KTOpenPackage package(projectID);
    k->socket->send(package);
}

void KTNetProjectManagerHandler::initialize(KTProjectManagerParams *params)
{
    KTNetProjectManagerParams *netparams = dynamic_cast<KTNetProjectManagerParams*>(params);

    if (! netparams) 
        return;
    
    k->params = netparams;
    
    #ifdef K_DEBUG
           tDebug("net") << "KTNetProjectManagerHandler::initialize() - Connecting to " << netparams->server() << ":" << netparams->port();
    #endif    

    k->socket->connectToHost(k->params->server(), k->params->port());
    bool connected = k->socket->waitForConnected(1000);

    if (connected) {
        KTConnectPackage connectPackage(k->params->server(), k->params->login(), k->params->password());
        k->socket->send(connectPackage);
    } else {
        TOsd::self()->display(tr("Error"), tr("Unable to connect to server"), TOsd::Error);
    }
}

bool KTNetProjectManagerHandler::setupNewProject(KTProjectManagerParams *params)
{
    KTNetProjectManagerParams *netparams = dynamic_cast<KTNetProjectManagerParams*>(params);
    
    if (! netparams) 
        return false;
    
    #ifdef K_DEBUG
           SHOW_VAR(netparams->projectName());
    #endif    

    k->projectName = netparams->projectName();
    k->author = netparams->author();
   
    /* 
    if (! k->socket->isOpen()) {
        bool connected = initialize(params);
        if (!connected) 
            return false;
    }
    */

    QString dimension = QString::number(netparams->dimension().width()) + "," + QString::number(netparams->dimension().height()); 

    KTNewProjectPackage newProjectPackage(netparams->projectName(), netparams->author(), netparams->description(),
                                          netparams->bgColor().name(), dimension, QString::number(netparams->fps()));
    k->socket->send(newProjectPackage);
    
    return true;
}

bool KTNetProjectManagerHandler::closeProject()
{
    k->projectIsOpen = false;

    closeConnection();

    return KTAbstractProjectHandler::closeProject();
}

void KTNetProjectManagerHandler::emitRequest(KTProjectRequest *request, bool toStack)
{
    emit sendCommand(request, toStack);
}

void KTNetProjectManagerHandler::handlePackage(const QString &root, const QString &package)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("net");
    #endif

    #ifdef K_DEBUG
           tDebug() << "KTNetProjectManagerHandler::handlePackage() - PKG:";
           tWarning() << package;
    #endif

    if (root == "user_denied") {
        closeConnection();
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Fatal Error"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(tr("User \"%1\" is disabled.\nPlease, contact the animation server admin to get access.").arg(k->params->login()));
        msgBox.exec();
    } else if (root == "project_request") {
               KTRequestParser parser;

               if (parser.parse(package)) {
                   if (parser.sign() == k->sign)
                       k->ownPackage = true;
                   else
                       k->ownPackage = false;

                   if (k->ownPackage && !k->doAction) {
                       if (parser.response()->part() == KTProjectRequest::Item) {
                           tFatal() << "KTNetProjectManagerHandler::handlePackage() - Executing local package!";
                           KTItemResponse *response = static_cast<KTItemResponse *>(parser.response());
                           KTProjectRequest request = KTRequestBuilder::createFrameRequest(response->sceneIndex(), 
                                                      response->layerIndex(), response->frameIndex(), KTProjectRequest::Select);
                           request.setExternal(!k->ownPackage);
                           emit sendLocalCommand(&request);
                       }
                       return;
                   } else {
                       tFatal() << "KTNetProjectManagerHandler::handlePackage() - Executing remote package!";
                       KTProjectRequest request = KTRequestBuilder::fromResponse(parser.response());
                       request.setExternal(!k->ownPackage);
                       emitRequest(&request, k->doAction && k->ownPackage);
                   }

               } else { // TODO: show error 
                   #ifdef K_DEBUG
                          tError() << "KTNetProjectManagerHandler::handlePackage() - Error parsing net request";
                   #endif
               }
    } else if (root == "server_ack") {
               // Checking the package
               KTAckParser parser;
               if (parser.parse(package)) {
                   k->sign = parser.sign();
                   // TOsd::self()->display(tr("Information"), tr("Login successful!")); 
                   // TOsd::self()->display(tr("Information"), parser.motd());
                   emit authenticationSuccessful(); 
               }
    } else if (root == "server_project") {
               KTProjectParser parser;
               if (parser.parse(package)) {
                   QTemporaryFile file;
                   if (file.open()) {
                       file.write(parser.data());
                       file.flush();
            
                       if (k->project) {
                           KTSaveProject *loader = new KTSaveProject;
                           loader->load(file.fileName(), k->project);
                           k->projectIsOpen = true;
                           emit openNewArea(k->project->projectName());
                           delete loader;
                       } else {
                           #ifdef K_DEBUG
                                  tError() << "KTNetProjectManagerHandler::handlePackage() - Error: Can't open project";
                           #endif
                       }
                   }
               }
    } else if (root == "server_projectlist") {
               KTProjectListParser parser;
               if (parser.parse(package)) {
                   if (parser.listSize() > 0) {
                       k->dialog = new KTListProjectDialog(k->params->server());
                       QDesktopWidget desktop;
                       k->dialog->show();
                       k->dialog->move((int) (desktop.screenGeometry().width() - k->dialog->width())/2,
                                       (int) (desktop.screenGeometry().height() - k->dialog->height())/2);
                       k->dialogIsOpen = true;

                       foreach (KTProjectListParser::ProjectInfo info, parser.projectsInfo())
                                k->dialog->addProject(info.file, info.name, info.author, info.description, info.date);

                       if (k->dialog->exec() == QDialog::Accepted && !k->dialog->projectID().isEmpty()) {
                           #ifdef K_DEBUG
                                  tDebug() << "KTNetProjectManagerHandler::handlePackage() - opening project " << k->dialog->projectID();
                           #endif
                           k->dialogIsOpen = false;
                           loadProjectFromServer(k->dialog->projectID());
                       } else {
                           k->dialogIsOpen = false;
                           closeConnection();
                       }
                   } else {
                       TOsd::self()->display(tr("Information"), tr("No available projects from server"), TOsd::Warning);
                       closeConnection();
                   }
               }
    } else if (root == "communication_notification") {
               KTNotificationParser parser;
               if (parser.parse(package)) {
                   int code = parser.notification().code;

                   if (code == 380)
                       emit savingSuccessful();

                   TOsd::Level level = TOsd::Level(parser.notification().level);
                   QString title = "Information";
                   if (level == TOsd::Warning) {
                       title = tr("Warning");
                   } else if (level == TOsd::Error) {
                              title = tr("Error");
                   }

                   TOsd::self()->display(title, parser.notification().message, level);
               }
    } else if (root == "communication_chat") {
               KTCommunicationParser parser;
               if (parser.parse(package)) {
                   k->chat->addMessage(parser.login(), parser.message());
               }
    } else if (root == "communication_notice") {
               KTCommunicationParser parser;
               if (parser.parse(package)) {
                   QString message = parser.message();
                   TOsd::self()->display(tr("Notice"), message);
                   k->notices->addMessage(message);
               } 
    } else if (root == "communication_wall") {
               KTCommunicationParser parser;
               if (parser.parse(package)) {
                   QString message = QObject::tr("Wall From") + ": "+ parser.login() + "\n" + parser.message();
                   TOsd::self()->display(tr("Information"), message);
               }
    } else {
      #ifdef K_DEBUG
             tError("net") << "KTNetProjectManagerHandler::handlePackage() - Error: Unknown package: " << root;
      #endif
    }
}

bool KTNetProjectManagerHandler::isValid() const
{
    return k->socket->state() == QAbstractSocket::ConnectedState;
}

void KTNetProjectManagerHandler::sendPackage(const QDomDocument &doc)
{
    k->socket->send(doc);
}

QTabWidget *KTNetProjectManagerHandler::communicationWidget()
{
    return k->communicationModule;
}

void KTNetProjectManagerHandler::setProject(KTProject *project)
{
    k->project = project;
}

void KTNetProjectManagerHandler::sendChatMessage(const QString & message)
{
    KTChatPackage package(message);
    sendPackage(package);
}

void KTNetProjectManagerHandler::connectionLost()
{
    #ifdef K_DEBUG
           tWarning() << "KTNetProjectManagerHandler::connectionLost() - The socket has been closed";
    #endif

    if (k->dialogIsOpen) {
        if (k->dialog) {
            if (k->dialog->isVisible())
                k->dialog->close();
        }
        emit connectionHasBeenLost();
    } else if (k->projectIsOpen) {
               emit connectionHasBeenLost();
    }
}

void KTNetProjectManagerHandler::closeConnection()
{
    if (k->socket->isOpen())
        k->socket->close();
}
