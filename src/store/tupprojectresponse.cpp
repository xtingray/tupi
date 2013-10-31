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
 *   the Free Software Foundation; either version 2 of the License, or     *
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

#include "tupprojectresponse.h"
#include "tdebug.h"

// This class returns information about data structure of a Tupi project 
// It is used from signals in the TupProject class to send data to anothers classes

class TupProjectResponse::Private
{
    public:
        Private(int part, int action) : part(part), action(action), isExternal(false) {}

        int part;
        int action;
        TupProjectRequestArgument arg;
        QByteArray data;
        Mode mode;

        bool isExternal;
};

TupProjectResponse::TupProjectResponse(int part, int action) : k(new Private(part, action))
{
}

TupProjectResponse::~TupProjectResponse()
{
    delete k;
}

int TupProjectResponse::part() const
{
    return k->part;
}

int TupProjectResponse::action() const
{
    if (k->mode == Undo) {

        switch (k->action) {
                case TupProjectRequest::Add:
                     {
                        return TupProjectRequest::Remove;
                     }
                break;
                case TupProjectRequest::InsertSymbolIntoFrame:
                     {
                        return TupProjectRequest::RemoveSymbolFromFrame;
                     }
                break;
                case TupProjectRequest::EditNodes:
                     {
                     }
                break;
                case TupProjectRequest::View:
                     {
                     }
                break;
                case TupProjectRequest::Select:
                     {
                     }
                break;
                case TupProjectRequest::Transform:
                     {
                     }
                break;
                case TupProjectRequest::SetTween:
                     {
                     }
                break;
                case TupProjectRequest::Lock:
                     {
                     }
                break;
                case TupProjectRequest::Ungroup:
                     {
                        return TupProjectRequest::Group;
                     }
                break;
                case TupProjectRequest::Rename:
                     {
                     }
                break;
                case TupProjectRequest::Move:
                     {
                     }
                break;
                case TupProjectRequest::Convert:
                     {
                     }
                break;
                case TupProjectRequest::Remove:
                     {
                        return TupProjectRequest::Add;
                     }
                break;
                case TupProjectRequest::Group:
                     {
                        return TupProjectRequest::Ungroup;
                     }
                break;
                case TupProjectRequest::Paste:
                     {
                     }
                break;
                default:
                     {
                        #ifdef K_DEBUG
                               tError() << "TupProjectResponse::action() : Fatal Error: Unhandled action -> " << k->action;
                        #endif
                     }
                break;
        }
    }

    return k->action;
}

int TupProjectResponse::originalAction() const
{
    return k->action;
}

void TupProjectResponse::setMode(Mode mode)
{
    k->mode = mode;
}

void TupProjectResponse::setExternal(bool e)
{
    k->isExternal = e;
}

bool TupProjectResponse::external() const
{
    return k->isExternal;
}

TupProjectResponse::Mode TupProjectResponse::mode() const
{
    return k->mode;
}

void TupProjectResponse::setArg(const QString &value)
{
    k->arg = value;
}

void TupProjectResponse::setData(const QByteArray &data)
{
    k->data = data;
}

TupProjectRequestArgument TupProjectResponse::arg() const
{
    return k->arg;
}

QByteArray TupProjectResponse::data() const
{
    return k->data;
}

// SCENE

TupSceneResponse::TupSceneResponse(int part, int action) : TupProjectResponse(part, action), m_sceneIndex(-1)
{
}

TupSceneResponse::~TupSceneResponse()
{
}

int TupSceneResponse::sceneIndex() const
{
    return m_sceneIndex;
}

void TupSceneResponse::setSceneIndex(int index)
{
    m_sceneIndex = index;
}

void TupSceneResponse::setState(const QString &state)
{
    m_state = state;
}

QString TupSceneResponse::state() const
{
    return m_state;
}

void TupSceneResponse::setScenes(Scenes scenes) 
{
    m_scenes = scenes;
}

// LAYER

TupLayerResponse::TupLayerResponse(int part, int action) : TupSceneResponse(part, action), m_layerIndex(-1)
{
}

TupLayerResponse::~TupLayerResponse()
{
}

int TupLayerResponse::layerIndex() const
{
    return m_layerIndex;
}

void TupLayerResponse::setLayerIndex(int index)
{
    m_layerIndex = index;
}

// FRAME

TupFrameResponse::TupFrameResponse(int part, int action) : TupLayerResponse(part, action), m_frameIndex(-1)
{
}

TupFrameResponse::~TupFrameResponse()
{
}

int TupFrameResponse::frameIndex() const
{
    return m_frameIndex;
}

void TupFrameResponse::setFrameIndex(int index)
{
    m_frameIndex = index;
}

bool TupFrameResponse::frameIsEmpty()
{
    return empty;
}

void TupFrameResponse::setFrameState(bool state)
{
    empty = state;
}

// ITEM

TupItemResponse::TupItemResponse(int part, int action) : TupFrameResponse(part, action), m_itemIndex(-1)
{
}

TupItemResponse::~TupItemResponse()
{
}

int TupItemResponse::itemIndex() const
{
    return m_itemIndex;
}

void TupItemResponse::setItemIndex(int index)
{
    m_itemIndex = index;
}

TupLibraryObject::Type TupItemResponse::itemType() const
{
    return m_itemType;
}

void TupItemResponse::setItemType(TupLibraryObject::Type type)
{
    m_itemType = type;
}

QPointF TupItemResponse::position()
{
    return QPointF(m_x, m_y);
}

void TupItemResponse::setPosX(double coord)
{
    m_x = coord;
}

void TupItemResponse::setPosY(double coord)
{
    m_y = coord;
}

TupProject::Mode TupItemResponse::spaceMode()
{
    return m_mode;
}

void TupItemResponse::setSpaceMode(TupProject::Mode mode)
{
    m_mode = mode;
}

bool TupItemResponse::frameIsEmpty()
{
    return empty;
}

void TupItemResponse::setFrameState(bool state)
{
    empty = state;
}

TupLibraryResponse::TupLibraryResponse(int part, int action) : TupFrameResponse(part, action)
{
}

TupLibraryResponse::~TupLibraryResponse()
{
}

void TupLibraryResponse::setSymbolType(TupLibraryObject::Type symtype)
{
    m_symbolType = symtype;
}

TupLibraryObject::Type TupLibraryResponse::symbolType() const
{
    return m_symbolType;
}

void TupLibraryResponse::setParent(const QString top)
{
    parentNode = top;
}

QString TupLibraryResponse::parent() const
{
    return parentNode;
}

TupProject::Mode TupLibraryResponse::spaceMode()
{
    return m_mode;
}

void TupLibraryResponse::setSpaceMode(TupProject::Mode mode)
{
    m_mode = mode;
}

bool TupLibraryResponse::frameIsEmpty()
{
    return empty;
}

void TupLibraryResponse::setFrameState(bool state)
{
    empty = state;
}

TupProjectResponseFactory::TupProjectResponseFactory()
{
}

TupProjectResponseFactory::~TupProjectResponseFactory()
{
}

TupProjectResponse *TupProjectResponseFactory::create(int part, int action)
{
    switch (part) {
            case TupProjectRequest::Scene:
             {
                 return new TupSceneResponse(part, action);
             }
            break;
            case TupProjectRequest::Layer:
             {
                 return new TupLayerResponse(part, action);
             }
            break;
            case TupProjectRequest::Frame:
             {
                 return new TupFrameResponse(part, action);
             }
            break;
            case TupProjectRequest::Item:
             {
                 return new TupItemResponse(part, action);
             }
            break;
            case TupProjectRequest::Library:
             {
                 return new TupLibraryResponse(part, action);
             }
            break;
            default:
             {
                tFatal() << "TupProjectResponseFactory::create() - Unknown/Unhandled element: " << part;
             }
            break;
    }
    
    return new TupProjectResponse(part, action);
}
