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

#ifndef TUPTIMELINE_H
#define TUPTIMELINE_H

#include "tglobal.h"
#include "tupmodulewidgetbase.h"
#include "ttabwidget.h"
#include "tupprojectrequest.h"
#include "tuplibraryobject.h"
#include "tuptimelinetable.h"
#include "tupprojectactionbar.h"
#include "tuprequestbuilder.h"
#include "tupproject.h"
#include "tuplayer.h"
#include "tuplibrary.h"

#include <QStackedWidget>
#include <QList>
#include <QHeaderView>

/**
 * @author David Cuadrado
**/

class TUPI_EXPORT TupTimeLine : public TupModuleWidgetBase
{
    Q_OBJECT

    public:
        TupTimeLine(TupProject *project, QWidget *parent = 0);
        ~TupTimeLine();
        void closeAllScenes();
        
    private:
        TupTimeLineTable *framesTable(int sceneIndex);
        
    protected:
        void sceneResponse(TupSceneResponse *response);
        void layerResponse(TupLayerResponse *response);
        void frameResponse(TupFrameResponse *response);
        void libraryResponse(TupLibraryResponse *response);
        
    public slots:
        void insertScene(int sceneIndex, const QString &name);
        void removeScene(int sceneIndex);
        void emitRequestChangeFrame(int sceneIndex, int layerIndex, int frameIndex);

    private slots:
        void requestCommand(int action);
        bool requestFrameAction(int action, int framePos = -1, int layerPos = -1, int scenePos = -1, const QVariant &arg = QVariant());
        bool requestLayerAction(int action, int layerPos = -1, int scenePos = -1, const QVariant &arg = QVariant());
        bool requestSceneAction(int action, int scenePos = -1, const QVariant &arg = QVariant());
        void selectFrame(int indexLayer, int indexFrame);
        void requestLayerMove(int oldIndex, int newIndex);

        void requestLayerVisibilityAction(int layer, bool isVisible);
        void requestLayerRenameAction(int layer, const QString &name);
        void requestSceneSelection(int sceneIndex);
        
    private:
        struct Private;
        Private *const k;
};

#endif
