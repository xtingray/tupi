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

#include "ktprojectresponse.h"
#include "tdebug.h"

// This class returns information about data structure of a Tupi project 
// It is used from signals in the KTProject class to send data to anothers classes

class KTProjectResponse::Private
{
    public:
        Private(int part, int action) : part(part), action(action), isExternal(false) {}

        int part;
        int action;
        KTProjectRequestArgument arg;
        QByteArray data;
        Mode mode;

        bool isExternal;
};

KTProjectResponse::KTProjectResponse(int part, int action) : k(new Private(part, action))
{
}

KTProjectResponse::~KTProjectResponse()
{
    delete k;
}

int KTProjectResponse::part() const
{
    return k->part;
}

int KTProjectResponse::action() const
{
    if (k->mode == Undo) {

        switch (k->action) {
                case KTProjectRequest::Add:
                     {
                        return KTProjectRequest::Remove;
                     }
                break;
                case KTProjectRequest::AddSymbolToProject:
                     {
                        return KTProjectRequest::RemoveSymbolFromProject;
                     }
                break;
                case KTProjectRequest::EditNodes:
                     {
                     }
                break;
                case KTProjectRequest::View:
                     {
                     }
                break;
                case KTProjectRequest::Select:
                     {
                     }
                break;
                case KTProjectRequest::Transform:
                     {
                     }
                break;
                case KTProjectRequest::SetTween:
                     {
                     }
                break;
                case KTProjectRequest::Lock:
                     {
                     }
                break;
                case KTProjectRequest::Ungroup:
                     {
                        return KTProjectRequest::Group;
                     }
                break;
                case KTProjectRequest::Rename:
                     {
                     }
                break;
                case KTProjectRequest::Move:
                     {
                     }
                break;
                case KTProjectRequest::Convert:
                     {
                     }
                break;
                case KTProjectRequest::Remove:
                     {
                        return KTProjectRequest::Add;
                     }
                break;
                case KTProjectRequest::Group:
                     {
                        return KTProjectRequest::Ungroup;
                     }
                break;
                case KTProjectRequest::Paste:
                     {
                     }
                break;
                default:
                     {
                        tFatal() << "KTProjectResponse::action() : Unhandled action -> " << k->action;
                     }
                break;
        }
    }

    return k->action;
}

int KTProjectResponse::originalAction() const
{
    return k->action;
}

void KTProjectResponse::setMode(Mode mode)
{
    k->mode = mode;
}

void KTProjectResponse::setExternal(bool e)
{
    k->isExternal = e;
}

bool KTProjectResponse::external() const
{
    return k->isExternal;
}

KTProjectResponse::Mode KTProjectResponse::mode() const
{
    return k->mode;
}

void KTProjectResponse::setArg(const QString &value)
{
    k->arg = value;
}

void KTProjectResponse::setData(const QByteArray &data)
{
    k->data = data;
}

KTProjectRequestArgument KTProjectResponse::arg() const
{
    return k->arg;
}

QByteArray KTProjectResponse::data() const
{
    return k->data;
}

// SCENE

KTSceneResponse::KTSceneResponse(int part, int action) : KTProjectResponse(part, action), m_sceneIndex(-1)
{
}

KTSceneResponse::~KTSceneResponse()
{
}

int KTSceneResponse::sceneIndex() const
{
    return m_sceneIndex;
}

void KTSceneResponse::setSceneIndex(int index)
{
    m_sceneIndex = index;
}

void KTSceneResponse::setState(const QString &state)
{
    m_state = state;
}

QString KTSceneResponse::state() const
{
    return m_state;
}

void KTSceneResponse::setScenes(Scenes scenes) 
{
    m_scenes = scenes;
}

// LAYER

KTLayerResponse::KTLayerResponse(int part, int action) : KTSceneResponse(part, action), m_layerIndex(-1)
{
}

KTLayerResponse::~KTLayerResponse()
{
}

int KTLayerResponse::layerIndex() const
{
    return m_layerIndex;
}

void KTLayerResponse::setLayerIndex(int index)
{
    m_layerIndex = index;
}

// FRAME

KTFrameResponse::KTFrameResponse(int part, int action) : KTLayerResponse(part, action), m_frameIndex(-1)
{
}

KTFrameResponse::~KTFrameResponse()
{
}

int KTFrameResponse::frameIndex() const
{
    return m_frameIndex;
}

void KTFrameResponse::setFrameIndex(int index)
{
    m_frameIndex = index;
}

// ITEM

KTItemResponse::KTItemResponse(int part, int action) : KTFrameResponse(part, action), m_itemIndex(-1)
{
}

KTItemResponse::~KTItemResponse()
{
}

int KTItemResponse::itemIndex() const
{
    return m_itemIndex;
}

void KTItemResponse::setItemIndex(int index)
{
    m_itemIndex = index;
}

KTLibraryObject::Type KTItemResponse::itemType() const
{
    return m_itemType;
}

void KTItemResponse::setItemType(KTLibraryObject::Type type)
{
    m_itemType = type;
}

QPointF KTItemResponse::position()
{
    return QPointF(m_x, m_y);
}

void KTItemResponse::setPosX(double coord)
{
    m_x = coord;
}

void KTItemResponse::setPosY(double coord)
{
    m_y = coord;
}

KTProject::Mode KTItemResponse::spaceMode()
{
    return m_mode;
}

void KTItemResponse::setSpaceMode(KTProject::Mode mode)
{
    m_mode = mode;
}

KTLibraryResponse::KTLibraryResponse(int part, int action) : KTFrameResponse(part, action)
{
}

KTLibraryResponse::~KTLibraryResponse()
{
}

void KTLibraryResponse::setSymbolType(KTLibraryObject::Type symtype)
{
    m_symbolType = symtype;
}

KTLibraryObject::Type KTLibraryResponse::symbolType() const
{
    return m_symbolType;
}

void KTLibraryResponse::setParent(const QString top)
{
    parentNode = top;
}

QString KTLibraryResponse::parent() const
{
    return parentNode;
}

KTProjectResponseFactory::KTProjectResponseFactory()
{
}

KTProjectResponseFactory::~KTProjectResponseFactory()
{
}

KTProjectResponse *KTProjectResponseFactory::create(int part, int action)
{
    switch (part) {
            case KTProjectRequest::Scene:
             {
                 return new KTSceneResponse(part, action);
             }
            break;
            case KTProjectRequest::Layer:
             {
                 return new KTLayerResponse(part, action);
             }
            break;
            case KTProjectRequest::Frame:
             {
                 return new KTFrameResponse(part, action);
             }
            break;
            case KTProjectRequest::Item:
             {
                 return new KTItemResponse(part, action);
             }
            break;
            case KTProjectRequest::Library:
             {
                 return new KTLibraryResponse(part, action);
             }
            break;
            default:
             {
                qFatal("Unknown PART"); // TODO: REMOVE ME
             }
            break;
    }
    
    return new KTProjectResponse(part, action);
}
