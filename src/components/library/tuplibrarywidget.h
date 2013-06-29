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

#ifndef TUPLIBRARYWIDGET_H
#define TUPLIBRARYWIDGET_H

#include "tupmodulewidgetbase.h"
#include "tupitempreview.h"
#include "timagebutton.h"
#include "tupgctable.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include <QDir>
#include <QMouseEvent>

class TupLibrary;

/**
 * @author David Cuadrado
*/

class TupLibraryWidget : public TupModuleWidgetBase
{
    Q_OBJECT

    public:
        TupLibraryWidget(QWidget *parent = 0);
        ~TupLibraryWidget();
        void resetGUI();
        void setLibrary(TupLibrary *library);
        void setNetworking(bool isNetworked);

    protected:
        virtual void sceneResponse(TupSceneResponse *response);
        virtual void libraryResponse(TupLibraryResponse *response);
        virtual void frameResponse(TupFrameResponse *response);

    private slots:
        void addFolder();
        void previewItem(QTreeWidgetItem *);
        void insertObjectInWorkspace();
        void removeCurrentGraphic();
        void renameObject(QTreeWidgetItem* item);
        void importGraphicObject();
        void refreshItem(QTreeWidgetItem *item);
        void updateLibrary(QString node, QString target);
        void activeRefresh(QTreeWidgetItem *item);

    public slots:
        void importBitmap();
        void importSvg();
        void importBitmapArray();
        void importSvgArray();
        void importSound();

    signals:
        void requestCurrentGraphic();

    private:
        struct Private;
        Private *const k;
};

#endif
