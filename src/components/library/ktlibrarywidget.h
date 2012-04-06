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

#ifndef KTLIBRARYWIDGET_H
#define KTLIBRARYWIDGET_H

#include "ktmodulewidgetbase.h"
#include "ktitempreview.h"
#include "timagebutton.h"
#include "ktgctable.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include <QDir>
#include <QMouseEvent>

class KTLibrary;

/**
 * @author David Cuadrado
*/

class KTLibraryWidget : public KTModuleWidgetBase
{
    Q_OBJECT

    public:
        KTLibraryWidget(QWidget *parent = 0);
        ~KTLibraryWidget();
        void resetGUI();
        void setLibrary(KTLibrary *library);
        void setNetworking(bool isNetworked);

    protected:
        virtual void libraryResponse(KTLibraryResponse *response);
        virtual void frameResponse(KTFrameResponse *response);

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
