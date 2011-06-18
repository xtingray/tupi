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

#ifndef KTPROJECTRESPONSE_H
#define KTPROJECTRESPONSE_H

#include "ktglobal_store.h"
#include "ktprojectrequest.h"
#include "ktscene.h"
#include "ktlibraryobject.h"

/**
 * Response to request (aka KTProjectRequest)
 * @author David Cuadrado
*/

class STORE_EXPORT KTProjectResponse
{
    public:
        enum Mode
             {
               None = 0x00,
               Do,
               Redo,
               Undo
             };

        KTProjectResponse(int part, int action);
        virtual ~KTProjectResponse();

        int part() const;
        int action() const;

        int originalAction() const;

        void setArg(const QString &value);
        void setData(const QByteArray &data);
        void setMode(Mode mode);

        void setExternal(bool e);
        bool external() const;

        Mode mode() const;

        KTProjectRequestArgument arg() const;
        QByteArray data() const;

    private:
        class Private;
        Private *const k;
};

class KTSceneResponse : public KTProjectResponse
{
    public:
        KTSceneResponse(int part, int action);
        ~KTSceneResponse();
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

class KTLayerResponse : public KTSceneResponse
{
    public:
        KTLayerResponse(int part, int action);
        ~KTLayerResponse();
        int layerIndex() const;
        void setLayerIndex(int index);

    private:
        int m_layerIndex;
};

class KTFrameResponse : public KTLayerResponse
{
    public:
        KTFrameResponse(int part, int action);
        ~KTFrameResponse();
        int frameIndex() const;
        void setFrameIndex(int index);

    private:
        int m_frameIndex;
};

class KTItemResponse : public KTFrameResponse
{
    public:
        KTItemResponse(int part, int action);
        ~KTItemResponse();
        int itemIndex() const;
        void setItemIndex(int index);
        KTLibraryObject::Type itemType() const;
        void setItemType(KTLibraryObject::Type type);
        QPointF position();
        void setPosX(double coord);
        void setPosY(double coord);
        KTProject::Mode spaceMode();
        void setSpaceMode(KTProject::Mode mode);

    private:
        int m_itemIndex;
        KTLibraryObject::Type m_itemType;
        double m_x;
        double m_y;
        KTProject::Mode m_mode;
};

class KTLibraryResponse : public KTFrameResponse
{
    public:
        KTLibraryResponse(int part, int action);
        ~KTLibraryResponse();
        void setSymbolType(KTLibraryObject::Type symtype);
        KTLibraryObject::Type symbolType() const;
        void setParent(const QString top);
        QString parent() const;
        KTProject::Mode spaceMode();
        void setSpaceMode(KTProject::Mode mode);

    private:
        KTLibraryObject::Type m_symbolType;
        QString parentNode;
        KTProject::Mode m_mode;
};

class KTProjectResponseFactory
{
    private:
        KTProjectResponseFactory();

    public:
        ~KTProjectResponseFactory();
        static KTProjectResponse *create(int part, int action);
};

#endif
