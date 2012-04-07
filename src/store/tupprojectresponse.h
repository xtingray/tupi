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

#ifndef TupPROJECTRESPONSE_H
#define TupPROJECTRESPONSE_H

#include "tupglobal_store.h"
#include "tupprojectrequest.h"
#include "tupscene.h"
#include "tuplibraryobject.h"

/**
 * Response to request (aka TupProjectRequest)
 * @author David Cuadrado
*/

class STORE_EXPORT TupProjectResponse
{
    public:
        enum Mode
             {
               None = 0x00,
               Do,
               Redo,
               Undo
             };

        TupProjectResponse(int part, int action);
        virtual ~TupProjectResponse();

        int part() const;
        int action() const;

        int originalAction() const;

        void setArg(const QString &value);
        void setData(const QByteArray &data);
        void setMode(Mode mode);

        void setExternal(bool e);
        bool external() const;

        Mode mode() const;

        TupProjectRequestArgument arg() const;
        QByteArray data() const;

    private:
        class Private;
        Private *const k;
};

class TupSceneResponse : public TupProjectResponse
{
    public:
        TupSceneResponse(int part, int action);
        ~TupSceneResponse();
        int sceneIndex() const;
        void setSceneIndex(int index);

        void setState(const QString &state);
        QString state() const;
        void setScenes(Scenes scenes);

    private:
        int m_sceneIndex;
        QString m_state;
        Scenes m_scenes;
};

class TupLayerResponse : public TupSceneResponse
{
    public:
        TupLayerResponse(int part, int action);
        ~TupLayerResponse();
        int layerIndex() const;
        void setLayerIndex(int index);

    private:
        int m_layerIndex;
};

class TupFrameResponse : public TupLayerResponse
{
    public:
        TupFrameResponse(int part, int action);
        ~TupFrameResponse();
        int frameIndex() const;
        void setFrameIndex(int index);
        bool frameIsEmpty();
        void setFrameState(bool state);

    private:
        int m_frameIndex;
        bool empty;
};

class TupItemResponse : public TupFrameResponse
{
    public:
        TupItemResponse(int part, int action);
        ~TupItemResponse();
        int itemIndex() const;
        void setItemIndex(int index);
        TupLibraryObject::Type itemType() const;
        void setItemType(TupLibraryObject::Type type);
        QPointF position();
        void setPosX(double coord);
        void setPosY(double coord);
        TupProject::Mode spaceMode();
        void setSpaceMode(TupProject::Mode mode);
        bool frameIsEmpty();
        void setFrameState(bool state);

    private:
        int m_itemIndex;
        TupLibraryObject::Type m_itemType;
        double m_x;
        double m_y;
        TupProject::Mode m_mode;
        bool empty;
};

class TupLibraryResponse : public TupFrameResponse
{
    public:
        TupLibraryResponse(int part, int action);
        ~TupLibraryResponse();
        void setSymbolType(TupLibraryObject::Type symtype);
        TupLibraryObject::Type symbolType() const;
        void setParent(const QString top);
        QString parent() const;
        TupProject::Mode spaceMode();
        void setSpaceMode(TupProject::Mode mode);
        bool frameIsEmpty();
        void setFrameState(bool state);

    private:
        TupLibraryObject::Type m_symbolType;
        QString parentNode;
        TupProject::Mode m_mode;
        bool empty;
};

class TupProjectResponseFactory
{
    private:
        TupProjectResponseFactory();

    public:
        ~TupProjectResponseFactory();
        static TupProjectResponse *create(int part, int action);
};

#endif
