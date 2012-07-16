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

#include "tupnetprojectmanagerhandler.h"
#include "tupnetprojectmanagerparams.h"
#include "tupprojectresponse.h"
#include "tdebug.h"
#include "tosd.h"

#include "tupprojectcommand.h"
#include "tupcommandexecutor.h"
#include "tupnetsocket.h"
#include "tupprojectrequest.h"
#include "tupnewprojectpackage.h"
#include "tupconnectpackage.h"
#include "tupimageexportpackage.h"
#include "tupvideoexportpackage.h"
#include "tupstoryboardexportpackage.h"
#include "tupinetfilemanager.h"
#include "tupopenpackage.h"
#include "tupchatpackage.h"
#include "tupnotificationparser.h"
#include "tupprojectlistparser.h"
#include "tupprojectparser.h"
#include "tuprequestparser.h"
#include "tupackparser.h"
#include "tupcommunicationparser.h"
#include "tuprequestbuilder.h"
#include "tupproject.h"
#include "tuplistprojectdialog.h"
#include "tupchat.h"
#include "tupnotice.h"

#include <QTemporaryFile>
#include <QTabWidget>
#include <QDesktopWidget>
#include <QMessageBox>

struct TupNetProjectManagerHandler::Private
{
    TupNetProjectManagerParams *params;
    TupNetSocket *socket;
    QString projectName;
    QString username;
    TupProject *project;
    
    QString sign;
    bool ownPackage;
    bool doAction;
    
    QTabWidget *communicationModule;
    TupChat *chat;
    TupNotice *notices;

    bool projectIsOpen; 
    bool dialogIsOpen;
    TupListProjectDialog *dialog;
};

TupNetProjectManagerHandler::TupNetProjectManagerHandler(QObject *parent) : TupAbstractProjectHandler(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->socket = new TupNetSocket(this);
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

    k->chat = new TupChat;
    k->communicationModule->addTab(k->chat, tr("Chat"));
    
    connect(k->chat, SIGNAL(requestSendMessage(const QString&)), this, SLOT(sendChatMessage(const QString&)));
    
    k->notices = new TupNotice;
    k->communicationModule->addTab(k->notices, tr("Notices"));
    
    // connect(k->notices, SIGNAL(requestSendMessage(const QString&)), this, SLOT(sendNoticeMessage(const QString&)));
}

TupNetProjectManagerHandler::~TupNetProjectManagerHandler()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    k->chat->close();
    delete k;
}

void TupNetProjectManagerHandler::handleProjectRequest(const TupProjectRequest* request)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    // This comes from the project before the command execution
    // SQA: Save a copy of the events or queued packages and resend to the GUI when the "Ok" package 
    // comes from the server 

    if (k->socket->state() == QAbstractSocket::ConnectedState) {
        #ifdef K_DEBUG
               tWarning() << "TupNetProjectManagerHandler::handleProjectRequest() - SENDING PACKAGE: " << request->xml();
        #endif

        if (request->isValid()) {
            emit sendCommand(request, true);
            k->socket->send(request->xml());
        } else {
            #ifdef K_DEBUG
                   tFatal() << "TupNetProjectManagerHandler::handleProjectRequest() - INVALID REQUEST! ID: " << request->id();
            #endif
        }
    }
}

bool TupNetProjectManagerHandler::commandExecuted(TupProjectResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (response->mode() == TupProjectResponse::Do) {
        k->doAction = true;
        return true;
    } 

    TupProjectRequest request = TupRequestBuilder::fromResponse(response);
    k->doAction = false;

    if (response->mode() != TupProjectResponse::Undo && response->mode() != TupProjectResponse::Redo) {
        handleProjectRequest(&request);
    } else { 
        if (k->socket->state() == QAbstractSocket::ConnectedState) {
            if (request.isValid())
                k->socket->send(request.xml());
        }
    }

    return true;
}

bool TupNetProjectManagerHandler::saveProject(const QString &fileName, TupProject *project)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    Q_UNUSED(fileName);
    Q_UNUSED(project);

    /*
    TupiNetFileManager manager(k->params->server(), k->params->port());
    return manager.save(fileName, project);
    */

    return true;
}

bool TupNetProjectManagerHandler::loadProject(const QString &fileName, TupProject *project)
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

void TupNetProjectManagerHandler::loadProjectFromServer(const QString &projectID, const QString &owner)
{
    TupOpenPackage package(projectID, owner);
    k->socket->send(package);
}

void TupNetProjectManagerHandler::initialize(TupProjectManagerParams *params)
{
    TupNetProjectManagerParams *netparams = dynamic_cast<TupNetProjectManagerParams*>(params);

    if (! netparams) 
        return;
    
    k->params = netparams;
    
    #ifdef K_DEBUG
           tDebug("net") << "TupNetProjectManagerHandler::initialize() - Connecting to " << netparams->server() << ":" << netparams->port();
    #endif    

    k->socket->connectToHost(k->params->server(), k->params->port());
    bool connected = k->socket->waitForConnected(1000);

    if (connected) {
        TupConnectPackage connectPackage(k->params->server(), k->params->login(), k->params->password());
        k->socket->send(connectPackage);
        k->username = k->params->login();
    } else {
        TOsd::self()->display(tr("Error"), tr("Unable to connect to server"), TOsd::Error);
    }
}

bool TupNetProjectManagerHandler::setupNewProject(TupProjectManagerParams *params)
{
    TupNetProjectManagerParams *netparams = dynamic_cast<TupNetProjectManagerParams*>(params);
    
    if (! netparams) 
        return false;
    
    #ifdef K_DEBUG
           SHOW_VAR(netparams->projectName());
    #endif    

    k->projectName = netparams->projectName();
    // k->author = netparams->author();
   
    /* 
    if (! k->socket->isOpen()) {
        bool connected = initialize(params);
        if (!connected) 
            return false;
    }
    */

    QString dimension = QString::number(netparams->dimension().width()) + "," + QString::number(netparams->dimension().height()); 

    TupNewProjectPackage newProjectPackage(netparams->projectName(), netparams->author(), netparams->description(),
                                          netparams->bgColor().name(), dimension, QString::number(netparams->fps()));
    k->socket->send(newProjectPackage);
    
    return true;
}

bool TupNetProjectManagerHandler::closeProject()
{
    k->projectIsOpen = false;

    closeConnection();

    return TupAbstractProjectHandler::closeProject();
}

void TupNetProjectManagerHandler::emitRequest(TupProjectRequest *request, bool toStack)
{
    emit sendCommand(request, toStack);
}

void TupNetProjectManagerHandler::handlePackage(const QString &root, const QString &package)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("net");
    #endif

    #ifdef K_DEBUG
           tDebug() << "TupNetProjectManagerHandler::handlePackage() - PKG:";
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
               TupRequestParser parser;

               if (parser.parse(package)) {
                   if (parser.sign() == k->sign)
                       k->ownPackage = true;
                   else
                       k->ownPackage = false;

                   if (k->ownPackage && !k->doAction) {
                       if (parser.response()->part() == TupProjectRequest::Item) {
                           TupItemResponse *response = static_cast<TupItemResponse *>(parser.response());
                           TupProjectRequest request = TupRequestBuilder::createFrameRequest(response->sceneIndex(), 
                                                      response->layerIndex(), response->frameIndex(), TupProjectRequest::Select);
                           request.setExternal(!k->ownPackage);
                           emit sendLocalCommand(&request);
                       }
                       return;
                   } else {
                       TupProjectRequest request = TupRequestBuilder::fromResponse(parser.response());
                       request.setExternal(!k->ownPackage);
                       emitRequest(&request, k->doAction && k->ownPackage);
                   }

               } else { // TODO: show error 
                   #ifdef K_DEBUG
                          tError() << "TupNetProjectManagerHandler::handlePackage() - Error parsing net request";
                   #endif
               }
    } else if (root == "server_ack") {
               // Checking the package
               TupAckParser parser;
               if (parser.parse(package)) {
                   k->sign = parser.sign();
                   // TOsd::self()->display(tr("Information"), tr("Login successful!")); 
                   // TOsd::self()->display(tr("Information"), parser.motd());
                   emit authenticationSuccessful(); 
               }
    } else if (root == "server_project") {
               TupProjectParser parser;
               if (parser.parse(package)) {
                   QTemporaryFile file;
                   if (file.open()) {
                       file.write(parser.data());
                       file.flush();
            
                       if (k->project) {
                           TupiFileManager *manager = new TupiFileManager;
                           bool isOk = manager->load(file.fileName(), k->project);
                           if (isOk) {
                               k->projectIsOpen = true;
                               emit openNewArea(k->project->projectName(), parser.partners());
                           } else {
                               #ifdef K_DEBUG
                                      tError() << "TupNetProjectManagerHandler::handlePackage() - Error: Net project can't be opened";
                               #endif
                           }
                           delete manager;
                       } else {
                           #ifdef K_DEBUG
                                  tError() << "TupNetProjectManagerHandler::handlePackage() - Error: Can't open project";
                           #endif
                       }
                   }
               }
    } else if (root == "server_projectlist") {
               TupProjectListParser parser;
               if (parser.parse(package)) {
                   int works = parser.workSize();
                   int contributions = parser.contributionSize();
                   if ((works + contributions) > 0) {
                       k->dialog = new TupListProjectDialog(works, contributions, k->params->server());
                       QDesktopWidget desktop;
                       k->dialog->show();
                       k->dialog->move((int) (desktop.screenGeometry().width() - k->dialog->width())/2,
                                       (int) (desktop.screenGeometry().height() - k->dialog->height())/2);
                       k->dialogIsOpen = true;

                       foreach (TupProjectListParser::ProjectInfo info, parser.works())
                                k->dialog->addWork(info.file, info.name, info.description, info.date);

                       foreach (TupProjectListParser::ProjectInfo info, parser.contributions())
                                k->dialog->addContribution(info.file, info.name, info.author, info.description, info.date);

                       if (k->dialog->exec() == QDialog::Accepted && !k->dialog->projectID().isEmpty()) {
                           #ifdef K_DEBUG
                                  tDebug() << "TupNetProjectManagerHandler::handlePackage() - opening project " << k->dialog->projectID();
                           #endif
                           k->dialogIsOpen = false;
                           if (k->dialog->workIsMine())
                               loadProjectFromServer(k->dialog->projectID(), k->username);
                           else
                               loadProjectFromServer(k->dialog->projectID(), k->dialog->owner());
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
               TupNotificationParser parser;
               if (parser.parse(package)) {
                   int code = parser.notification().code;

                   switch(code) {
                          case 380:
                               emit savingSuccessful();
                          break;
                          case 100:
                          case 101:
                          case 102:
                          case 382:
                          case 383:
                          case 384:
                               emit postOperationDone();
                          break;
                   }

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
               TupCommunicationParser parser;
               if (parser.parse(package)) {
                   k->chat->addMessage(parser.login(), parser.message());
               }
    } else if (root == "communication_notice") {
               TupCommunicationParser parser;
               if (parser.parse(package)) {
                   QString login = parser.login();
                   int state = parser.state();

                   emit updateUsersList(login, state);

                   QString message = "<b>" + login + "</b>" + " has left the project"; 
                   if (state == 1)
                       message = "<b>" + login + "</b>" + " has joined the project";

                   TOsd::self()->display(tr("Notice"), message);
                   k->notices->addMessage(message);
               } 
    } else if (root == "communication_wall") {
               TupCommunicationParser parser;
               if (parser.parse(package)) {
                   QString message = QObject::tr("Wall From") + ": "+ parser.login() + "\n" + parser.message();
                   TOsd::self()->display(tr("Information"), message);
               }
    } else {
      #ifdef K_DEBUG
             tError("net") << "TupNetProjectManagerHandler::handlePackage() - Error: Unknown package: " << root;
      #endif
    }
}

bool TupNetProjectManagerHandler::isValid() const
{
    return k->socket->state() == QAbstractSocket::ConnectedState;
}

void TupNetProjectManagerHandler::sendPackage(const QDomDocument &doc)
{
    k->socket->send(doc);
}

QTabWidget *TupNetProjectManagerHandler::communicationWidget()
{
    return k->communicationModule;
}

void TupNetProjectManagerHandler::setProject(TupProject *project)
{
    k->project = project;
}

void TupNetProjectManagerHandler::sendChatMessage(const QString & message)
{
    TupChatPackage package(message);
    sendPackage(package);
}

void TupNetProjectManagerHandler::connectionLost()
{
    #ifdef K_DEBUG
           tWarning() << "TupNetProjectManagerHandler::connectionLost() - The socket has been closed";
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

void TupNetProjectManagerHandler::closeConnection()
{
    if (k->socket->isOpen())
        k->socket->close();
}

void TupNetProjectManagerHandler::sendExportImageRequestToServer(int frameIndex, int sceneIndex, 
                                                                const QString &title, const QString &topics, const QString &description)
{
    TupImageExportPackage package(frameIndex, sceneIndex, title, topics, description);
    sendPackage(package);
}

void TupNetProjectManagerHandler::sendVideoRequest(const QString &title, const QString &topics, const QString &description, int fps, const QList<int> sceneIndexes)
{
    TupVideoExportPackage package(title, topics, description, fps, sceneIndexes);
    sendPackage(package);
}

void TupNetProjectManagerHandler::sendStoryboardRequest(const QString &title, const QString &topics, const QString &description, const QList<int> sceneIndexes)
{
    TupStoryboardExportPackage package(title, topics, description, sceneIndexes);
    sendPackage(package);
}
