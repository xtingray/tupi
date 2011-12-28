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

#ifndef KTSCENETABWIDGET_H
#define KTSCENETABWIDGET_H

#include "tglobal.h"
#include "ktexposuretable.h"
#include "tapplicationproperties.h"

#include <QTabWidget>
#include <QHash>
#include <QLabel>
#include <QFrame>

/**
 * @author Gustav Gonzalez 
*/

class K_GUI_EXPORT KTSceneTabWidget : public QFrame
{
    Q_OBJECT

    public:
        KTSceneTabWidget(QWidget *parent = 0);
        ~KTSceneTabWidget();
        QTabWidget* tabWidget();
        void addScene(int index, const QString &name, KTExposureTable *table = 0);
        KTExposureTable* getCurrentTable();
        KTExposureTable* getTable(int index);
        int currentIndex();
        int count();

    /*
    private:
        QHash<int, KTExposureTable *> tables;
        QTabWidget *tabber;
    */

    public slots:
        void removeAllTabs();

    private:
        struct Private;
        Private *const k;

};

#endif
