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

#include "ktprojectmanager.h"

#include "ktproject.h"
#include "ktscene.h"
#include "ktlayer.h"
#include "ktframe.h"

#include "ktprojectrequest.h"
#include "ktprojectcommand.h"
#include "ktcommandexecutor.h"

#include "ktprojectmanagerparams.h"
#include "ktabstractprojectmanagerhandler.h"

#include "ktprojectresponse.h"
#include "ktrequestbuilder.h"
#include "ktrequestparser.h"

#include "talgorithm.h"
#include "tdebug.h"

#include <QUndoStack>
#include <QDir>

// This class handles the current animation project 

class KTProjectManager::Private
{
    public:
        Private() : handler(0), params(0)
        {
        }
        ~Private()
        {
           delete handler;
           delete undoStack;
           delete commandExecutor;
           delete params;
        }

    public:
        KTProject *project;
        bool isModified;
        int sceneIndex;
        int layerIndex;
        int frameIndex;
        KTAbstractProjectHandler *handler;
        QUndoStack *undoStack;
        KTCommandExecutor *commandExecutor;
        KTProjectManagerParams *params;
        QString copyFrame;
        bool isNetworked;
};

KTProjectManager::KTProjectManager(QObject *parent) : QObject(parent), k(new Private())
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->isModified = false;
    k->handler = 0;

    k->project = new KTProject(this);
    k->undoStack = new QUndoStack(this);
    k->commandExecutor = new KTCommandExecutor(k->project);

    connect(k->commandExecutor, SIGNAL(responsed(KTProjectResponse*)), this, SLOT(emitResponse(KTProjectResponse *)));
    connect(k->project, SIGNAL(responsed(KTProjectResponse*)), this, SIGNAL(responsed(KTProjectResponse *)));
}

KTProjectManager::~KTProjectManager()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    delete k;
}

void KTProjectManager::setParams(KTProjectManagerParams *params)
{
    if (k->params) 
        delete k->params;

    k->params = params;

    tFatal() << "KTProjectManager::setParams() - Initializing project manager handler";
    k->handler->initialize(k->params);
}

KTProjectManagerParams *KTProjectManager::params() const
{
    return k->params;
}

void KTProjectManager::setHandler(KTAbstractProjectHandler *handler, bool isNetworked)
{
    if (k->handler) {
        disconnect(k->handler, SIGNAL(sendCommand(const KTProjectRequest *, bool)), 
                   this, SLOT(createCommand(const KTProjectRequest *, bool)));
        disconnect(k->handler, SIGNAL(sendLocalCommand(const KTProjectRequest *)), 
                   this, SLOT(handleLocalRequest(const KTProjectRequest *)));
        delete k->handler;
        k->handler = 0;
    }

    k->handler = handler;
    k->handler->setParent(this);
    k->handler->setProject(k->project);
    k->isNetworked = isNetworked;

    connect(k->handler, SIGNAL(sendCommand(const KTProjectRequest *, bool)), this, SLOT(createCommand(const KTProjectRequest *, bool)));
    connect(k->handler, SIGNAL(sendLocalCommand(const KTProjectRequest *)), this, SLOT(handleLocalRequest(const KTProjectRequest *)));
}

KTAbstractProjectHandler *KTProjectManager::handler() const
{
    return k->handler;
}

void KTProjectManager::setupNewProject()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!k->handler || !k->params) {
        qDebug("ERROR: HANDLER!");
        return;
    }

    closeProject();

    k->project->setProjectName(k->params->projectName());
    k->project->setAuthor(k->params->author());
    k->project->setBgColor(k->params->bgColor());
    k->project->setDimension(k->params->dimension());
    k->project->setFPS(k->params->fps());

    if (! k->handler->setupNewProject(k->params)) {
        tDebug() << "KTProjectManager::setupNewProject() - Project settings error";
        return;
    }

    // k->project->setOpen(true);
    // setupProjectDir();

    if (!k->isNetworked) {
        KTProjectRequest request = KTRequestBuilder::createSceneRequest(0, KTProjectRequest::Add, tr("Scene %1").arg(1));
        handleProjectRequest(&request);

        request = KTRequestBuilder::createLayerRequest(0, 0, KTProjectRequest::Add, tr("Layer %1").arg(1));
        handleProjectRequest(&request);

        request = KTRequestBuilder::createFrameRequest(0, 0, 0, KTProjectRequest::Add, tr("Frame %1").arg(1));
        handleProjectRequest(&request);
    }
}

void KTProjectManager::closeProject()
{
    if (!k->handler)
        return;

    if (k->project->isOpen()) {
        if (! k->handler->closeProject()) {
            tError() << "KTProjectManager::closeProject() - Error while closing the project";
            return;
        }

        k->project->clear();
    }

    k->project->setOpen(false);
    k->isModified = false;
    k->undoStack->clear();
}

bool KTProjectManager::saveProject(const QString &fileName)
{
    bool result = k->handler->saveProject(fileName, k->project);
    k->isModified = !result;

    return result;
}

bool KTProjectManager::loadProject(const QString &fileName)
{
    if (! k->handler) {
        #ifdef K_DEBUG
               tFatal() << "KTProjectManager::loadProject() - NO HANDLER!";
        #endif
        return false;
    }

    bool ok = k->handler->loadProject(fileName, k->project);

    if (ok) {
        k->project->setOpen(true);
        k->isModified = false;
    }

    return ok;
}

/**
 * Returns true if project is open
 */
bool KTProjectManager::isOpen() const
{
    return k->project->isOpen();
}

bool KTProjectManager::isModified() const
{
    return k->isModified;
}

void KTProjectManager::undoModified()
{
    k->isModified = false;
}

bool KTProjectManager::isValid() const
{
    if (!k->handler) 
        return false;

    return k->handler->isValid();
}

/*
void KTProjectManager::setupProjectDir()
{
    QString name = (k->project->projectName().isEmpty() ? TAlgorithm::randomString(6) : k->project->projectName());
    QString dataDir = CACHE_DIR + "/" + name;
    QDir projectDir = dataDir;

    if (!projectDir.exists()) {
        if (projectDir.mkpath(projectDir.absolutePath())) {
            QStringList dirs;
            dirs << "audio" << "video" << "images" << "svg";
            foreach (QString dir, dirs)
                     projectDir.mkdir(dir);
        }
    }
}
*/

/**
 * This function is called when some event is triggered by the project
 * It must be re-implemented if you want to deal with the event in another way, i.ex: send it through the net.
 * By default, it sends the event through the signal commandExecuted
 * @param event 
 */
void KTProjectManager::handleProjectRequest(const KTProjectRequest *request)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
           tWarning() << "Package: ";
           tWarning() << request->xml();
    #endif

    // TODO: the handler must advise when to build the command
    
    if (k->handler)
        k->handler->handleProjectRequest(request);
    else
        qDebug("KTProjectManager::handleProjectRequest() - Error: No handler available");
}

void KTProjectManager::handleLocalRequest(const KTProjectRequest *request)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
           tWarning() << "Package: ";
           tWarning() << request->xml();
    #endif

    KTRequestParser parser;

    if (parser.parse(request->xml())) {
        if (KTFrameResponse *response = static_cast<KTFrameResponse *>(parser.response())) {
            k->sceneIndex = response->sceneIndex();
            k->layerIndex = response->layerIndex();
            k->frameIndex = response->frameIndex();

            if (response->action() == KTProjectRequest::Copy) {
                KTScene *scene = k->project->scene(k->sceneIndex);
                if (scene) {
                    KTLayer *layer = scene->layer(k->layerIndex);
                    if (layer) {
                        KTFrame *frame = layer->frame(k->frameIndex);
                        if (frame) {
                            QDomDocument doc;
                            doc.appendChild(frame->toXml(doc));
                            k->copyFrame = doc.toString(0);
                            response->setArg(k->copyFrame);
                        }
                    }
                }
            } else if (response->action() == KTProjectRequest::Paste) {
                       response->setArg(k->copyFrame);

                       // SQA: Bad code. Remove
                       // handleProjectRequest(& KTRequestBuilder::fromResponse(response));

                       KTProjectRequest request = KTRequestBuilder::fromResponse(response);
                       handleProjectRequest(&request);
                       return;
            }
        }

        parser.response()->setExternal(request->isExternal());

        emit responsed(parser.response());
    }
}

/**
 * This function creates a command to execute an action, i.e. add a frame. 
 * The command has the information necessary to undo its effect.
 * Usually this command must be added in the commands stack.
 * The command created is not deleted by this class, this task depends on the user.
 * @param event 
 * @return 
 */
void KTProjectManager::createCommand(const KTProjectRequest *request, bool addToStack)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (request->isValid()) {
        KTProjectCommand *command = new KTProjectCommand(k->commandExecutor, request);

        if (addToStack)
            k->undoStack->push(command);
         else 
            command->redo();
    } else {
        #ifdef K_DEBUG
               tWarning() << "KTProjectManager::createCommand() - Invalid request";
        #endif
    }
}

KTProject *KTProjectManager::project() const
{
    return k->project;
}

QUndoStack *KTProjectManager::undoHistory() const
{
    return k->undoStack;
}

void KTProjectManager::emitResponse(KTProjectResponse *response)
{
    #ifdef K_DEBUG
           T_FUNCINFO << response->action();
    #endif

    if (response->action() != KTProjectRequest::Select)
        k->isModified = true;

    if (!k->handler) {
        emit responsed(response);
    } else if (k->handler->commandExecuted(response)) {
        emit responsed(response);
    }
}

void KTProjectManager::setOpen(bool isOpen)
{
    k->project->setOpen(isOpen);
}
