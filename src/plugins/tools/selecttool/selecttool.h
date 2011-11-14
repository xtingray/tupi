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

#ifndef SELECTTOOL_H
#define SELECTTOOL_H

#include "kttoolplugin.h"
#include "ktpathitem.h"
#include "ktproject.h"
#include "ktgraphicsscene.h"
#include "infopanel.h"

#include <QObject>

class KTItemResponse;
class NodeManager;

/**
 * @author Jorge Cuadrado
*/

class SelectTool : public KTToolPlugin
{
    Q_OBJECT
    
    public:
        SelectTool();
        virtual ~SelectTool();
        
        virtual void init(KTGraphicsScene *scene);
        virtual QStringList keys() const;
        virtual void press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene);
        virtual void keyPressEvent(QKeyEvent *event);
        
        virtual QMap<QString, TAction *>actions() const;
        
        int toolType() const;
        
        virtual QWidget *configurator();
        
        virtual void aboutToChangeTool();
        void aboutToChangeScene(KTGraphicsScene *scene);
        
        virtual void itemResponse(const KTItemResponse *event);
        virtual void saveConfig();
        QCursor cursor() const;

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);

    private:
        void setupActions();
        void verifyActiveSelection();
        
    private:
        struct Private;
        Private *const k;
        InfoPanel *m_configurator;
        
    private slots:
        void syncNodes();
        void updateItems(KTGraphicsScene *);
        void applyFlip(InfoPanel::Flip flip);
};

#endif
