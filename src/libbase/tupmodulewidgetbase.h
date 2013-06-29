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

#ifndef TUPMODULEWIDGETBASE_H
#define TUPMODULEWIDGETBASE_H

#include "tosd.h"
#include "tupprojectresponse.h"
#include "tupabstractprojectresponsehandler.h"
#include "tupglobal.h"

#include <QLayout>
#include <QSizePolicy> 
#include <QObject>
#include <QEvent>
#include <QBoxLayout>
#include <QDockWidget>

/**
 * @author Jorge Cuadrado
*/

class TUPI_EXPORT TupModuleWidgetBase : public QWidget, public TupAbstractProjectResponseHandler
{
    Q_OBJECT
    
    public:
        TupModuleWidgetBase(QWidget *parent, const char *name = 0);
        ~TupModuleWidgetBase();
        virtual void addChild(QWidget *child, Qt::Alignment alignment = 0);
        void setFont(const QFont &);
        QBoxLayout *boxLayout();
        
    private:
        struct Private;
        Private *const k;
        
    protected:
        virtual bool event(QEvent *e);
        
    public slots:
        bool handleProjectResponse(TupProjectResponse *response);
        
    protected:
        virtual void enterEvent(QEvent *e);
        virtual void leaveEvent(QEvent *e);
        virtual void frameResponse(TupFrameResponse *frameRequest);
        virtual void layerResponse(TupLayerResponse *layerRequest);
        virtual void sceneResponse(TupSceneResponse *sceneRequest);
        virtual void projectResponse(TupProjectResponse *projectRequest);
        virtual void itemResponse(TupItemResponse *event);
        virtual void libraryResponse(TupLibraryResponse *libraryResponse);
        
    signals:
        void documentModified(bool);
        void sendToStatus(const QString &);
        void toggle();
        void activate(bool);
        
        void requestTriggered(const TupProjectRequest *event);
        void localRequestTriggered(const TupProjectRequest *event);
        void postPage(QWidget *widget);
};

#endif
