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
#include "ktnoticepackage.h"

#include "kterrorparser.h"
#include "ktprojectsparser.h"
#include "ktprojectparser.h"
#include "ktrequestparser.h"
#include "ktackparser.h"
#include "ktcomunicationparser.h"

#include "ktrequestbuilder.h"

#include "ktproject.h"

#include "ktlistprojectdialog.h"
#include "ktchat.h"
#include "ktnotice.h"

#include <QTemporaryFile>
#include <QTabWidget>

struct KTNetProjectManagerHandler::Private
{
    KTNetSocket *socket;
    QString projectName, author;
    
    KTNetProjectManagerParams *params;
    KTProject *project;
    
    QString sign;
    bool ownPackage;
    bool doAction;
    
    QTabWidget *comunicationModule;
    
    KTChat *chat;
    KTNotice *notices;
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
    
    k->comunicationModule = new QTabWidget;
    k->comunicationModule->setWindowTitle(tr("Communications"));
    k->comunicationModule->setWindowIcon(QPixmap(THEME_DIR + "icons/chat.png"));

    k->chat = new KTChat;
    k->comunicationModule->addTab(k->chat, tr("chat"));
    
    connect(k->chat, SIGNAL(requestSendMessage(const QString&)), this, SLOT(sendChatMessage(const QString&)));
    
    k->notices = new KTNotice;
    k->comunicationModule->addTab(k->notices, tr("notices"));
    
    connect(k->notices, SIGNAL(requestSendMessage(const QString&)), this, SLOT(sendNoticeMessage(const QString&)));

    tFatal() << "KTNetProjectManagerHandler::KTNetProjectManagerHandler() - Constructor is done!";
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
    // This comes from the project before the command execution
    
    // SQA: Save a copy of the events or queued packages and resend to the GUI when the "Ok" package 
    // comes from the server 

    if (k->socket->state() == QAbstractSocket::ConnectedState) {
        #ifdef K_DEBUG
               tDebug("net") << "KTNetProjectManagerHandler::handleProjectRequest() - SENDING PACKAGE: " << request->xml();
        #endif
        k->socket->send(request->xml());
    }
}

bool KTNetProjectManagerHandler::commandExecuted(KTProjectResponse *response)
{
    if (response->mode() == KTProjectResponse::Do) {
        k->doAction = true;
        return true;
    }
    
    KTProjectRequest request = KTRequestBuilder::fromResponse(response);
    handleProjectRequest(&request);
    
    k->doAction = false;
    
    return false;
}

bool KTNetProjectManagerHandler::saveProject(const QString &fileName, KTProject *project)
{
    KTSaveNetProject saver(k->params->server(), k->params->port());
    
    return saver.save(fileName, project);
}

bool KTNetProjectManagerHandler::loadProject(const QString &fileName, KTProject *project)
{
    if (k->socket->state() != QAbstractSocket::ConnectedState)
        return false;
    
    return loadProjectFromServer(k->params->projectName());
}

bool KTNetProjectManagerHandler::loadProjectFromServer(const QString &name)
{
    KTOpenPackage package(name);
    k->socket->send(package);

    return true;
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
        KTConnectPackage connectPackage(k->params->login(), k->params->password());
        k->socket->send(connectPackage);
    } else {
        tFatal() << "KTNetProjectManagerHandler::initialize() - Unable to connect to " << k->params->server() << ":" << k->params->port();
        TOsd::self()->display(tr("Error"), tr("Unable to connect to server"), TOsd::Error);
    }
}

bool KTNetProjectManagerHandler::setupNewProject(KTProjectManagerParams *params)
{
    tFatal() << "KTNetProjectManagerHandler::setupNewProject() - Tracing first connection to server!";

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
    
    KTNewProjectPackage newProjectPackage(netparams->projectName(), netparams->author(), netparams->description());
    k->socket->send(newProjectPackage);
    
    return true;
}

bool KTNetProjectManagerHandler::closeProject()
{
    if (k->socket->isOpen())
        k->socket->close();

    return KTAbstractProjectHandler::closeProject();
}

void KTNetProjectManagerHandler::emitRequest(KTProjectRequest *request, bool toStack)
{
    emit sendCommand(request, toStack);
}

void KTNetProjectManagerHandler::handlePackage(const QString &root ,const QString &package)
{
    #ifdef K_DEBUG
           T_FUNCINFOX("net");
    #endif

    if (root == "request") {
        KTRequestParser parser;
        if (parser.parse(package)) {

            if (parser.sign() == k->sign) 
                k->ownPackage = true;
            else 
                k->ownPackage = false;
            
            if (k->ownPackage && !k->doAction) {
                if (parser.response()->part() == KTProjectRequest::Item) {
                    KTItemResponse *response = static_cast<KTItemResponse *>(parser.response());
                    KTProjectRequest request = KTRequestBuilder::createFrameRequest(response->sceneIndex(), 
                                               response->layerIndex(), response->frameIndex(), KTProjectRequest::Select);
                    request.setExternal(!k->ownPackage);
                    emit sendLocalCommand(&request);
                }

                return;
            }
            
            KTProjectRequest request = KTRequestBuilder::fromResponse(parser.response());
            request.setExternal(!k->ownPackage);
            emitRequest(&request, k->doAction && k->ownPackage);

        } else { // TODO: show error 
            #ifdef K_DEBUG
                   tError() << "KTNetProjectManagerHandler::handlePackage() - Error parsing net request";
            #endif
        }
    } else if (root == "ack") {
               // Checking the package
               KTAckParser parser;
               if (parser.parse(package)) {
                   k->sign = parser.sign();
                   // TOsd::self()->display(tr("Information"), tr("Login successful!")); 
                   // TOsd::self()->display(tr("Information"), parser.motd());
                   emit createNewNetProject(); 
               }
    } else if (root == "notification") {
               KTErrorParser parser;
               if (parser.parse(package)) {
                   TOsd::Level level = TOsd::Level(parser.error().level);
                   QString title = "Information";
                   if (level == TOsd::Warning) {
                              title = tr("Warning");
                   } else if (level == TOsd::Error) {
                              title = tr("Error");
                   }
                   TOsd::self()->display(title, parser.error().message, level);
               }
    } else if (root == "project") {
               KTProjectParser parser;
               if (parser.parse(package)) {
                   QTemporaryFile file;
                   if (file.open()) {
                       file.write(parser.data());
                       file.flush();
            
                       if (k->project) {
                           KTSaveProject *loader = new KTSaveProject;
                           loader->load(file.fileName(), k->project);
                           tFatal() << "KTNetProjectManagerHandler::handlePackage() - Calling out for new project!";  
                           // emit openNewArea();
                           emit openNewArea(k->project->projectName());
                           delete loader;
                       } else {
                           tFatal() << "KTNetProjectManagerHandler::handlePackage() - No project. No call";
                       }
                   }
               }
    } else if (root == "projectlist") {
               KTProjectsParser parser;
               if (parser.parse(package)) {
                   KTListProjectDialog dialog;
                   foreach (KTProjectsParser::ProjectInfo info, parser.projectsInfo()) {
                            dialog.addProject(info.name, info.author, info.description);
                   }

                   if (dialog.exec () == QDialog::Accepted && !dialog.currentProject().isEmpty()) {
                       #ifdef K_DEBUG
                              tDebug() << "opening " << dialog.currentProject() << "project";
                       #endif
                       loadProjectFromServer(dialog.currentProject());
                   } else {
                       //SQA: Disconnect client
                   }
               }
    } else if (root == "chat") {
               KTComunicationParser parser;
               if (parser.parse(package)) {
                   k->chat->addMessage(parser.login(), parser.message());
               }
    } else if (root == "notice") {
               KTComunicationParser parser;
               if (parser.parse(package)) {
                   QString message = QObject::tr("Notice From") + ": "+ parser.login() + "\n" + parser.message();
                   TOsd::self()->display(tr("Notice"), message);
                   k->notices->addMessage(parser.login(), parser.message());
               }
    } else if (root == "wall") {
               KTComunicationParser parser;
               if (parser.parse(package)) {
                   QString message = QObject::tr("Wall From") + ": "+ parser.login() + "\n" + parser.message();
                   TOsd::self()->display(tr("Information"), message);
               }
    } else {
      #ifdef K_DEBUG
             tError("net") << "KTNetProjectManagerHandler::handlePackage() - Unknown package: " << root;
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

QTabWidget *KTNetProjectManagerHandler::comunicationWidget()
{
    return k->comunicationModule;
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

void KTNetProjectManagerHandler::sendNoticeMessage(const QString & message)
{
    KTNoticePackage package(message);
    sendPackage(package);
}

void KTNetProjectManagerHandler::connectionLost()
{
    tDebug() << "KTNetProjectManagerHandler::connectionLost() - The socket has been closed!";
}
