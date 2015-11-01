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

#ifndef TUPEXPORTWIDGET_H
#define TUPEXPORTWIDGET_H

#include "tglobal.h"
#include "tupproject.h"
#include "tupexportwizard.h"
#include "tuppluginmanager.h"
#include "tosd.h"

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupExportWidget : public TupExportWizard
{
    Q_OBJECT

    public:
        enum OutputFormat { Animation = 0, ImagesArray, AnimatedImage };
        TupExportWidget(TupProject *project, QWidget *parent = 0, bool isLocal = true);
        ~TupExportWidget();
        QString videoTitle() const;
        QString videoTopics() const;
        QString videoDescription() const;
        QList<int> videoScenes() const;
        bool isComplete();

    private slots:
        void setExporter(const QString &plugin);
	
    private:
        void loadPlugins();
		
    private:
        struct Private;
        Private *const k;
};

#endif
