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

#ifndef KTEXPORTWIDGET_H
#define KTEXPORTWIDGET_H

#include "ktmodulewidgetbase.h"
#include "ktproject.h"
#include "ktexportpluginobject.h"
#include "kexportwizard.h"

#include <QListWidget>
#include <QHash>

class QButtonGroup;
class QLineEdit;
class SelectPlugin;
class SelectScenes;
class ExportTo;

/**
 * @author David Cuadrado
*/

class KTExportWidget : public KExportWizard
{
    Q_OBJECT

    public:
        KTExportWidget(const KTProject *project, QWidget *parent = 0);
        ~KTExportWidget();

    private slots:
        void setExporter(const QString &plugin);
	
    private:
        void loadPlugins();
		
    private:
        SelectPlugin *m_pluginSelectionPage;
        SelectScenes *m_scenesSelectionPage;
        ExportTo *m_exportToPage;
        ExportTo *m_exportImages;
        const KTProject *m_project;
        QHash<QString, KTExportInterface *> m_plugins;
        const QString tag;
};

#endif
