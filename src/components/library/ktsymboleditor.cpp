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

#include "ktsymboleditor.h"
#include "ktlibraryobject.h"
#include "ktpluginmanager.h"
#include "kttoolplugin.h"
// Tupi Framework
#include "taction.h"
#include "tdebug.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QToolBar>
#include <QTimer>

/**
 * This class defines the toolbar at the left side of the paint area.
 * Here is where tool plugins are loaded and where the toolbar is created.
 * @author David Cuadrado
*/

class View : public QGraphicsView
{
    public:
        View();
        ~View();
};

View::View()
{
}

View::~View()
{
}

struct KTSymbolEditor::Private
{
    View *view;
    QGraphicsScene *scene;
    
    KTLibraryObject *symbol;
    
    QToolBar *selectionTools;
    QToolBar *fillTools;
    QToolBar *viewTools;
    QToolBar *brushTools;
};

KTSymbolEditor::KTSymbolEditor(QWidget *parent) : QMainWindow(parent), k(new Private)
{
    setWindowTitle(tr("Symbol editor"));
    
    k->view = new View;
    k->view->setRenderHints(QPainter::Antialiasing);
    
    k->scene = new QGraphicsScene;
    k->view->setScene(k->scene);
    
    setCentralWidget(k->view);
    
    k->brushTools = new QToolBar(tr("Brushes"));
    addToolBar(Qt::BottomToolBarArea, k->brushTools);
    
    k->selectionTools = new QToolBar(tr("Selection"));
    addToolBar(Qt::BottomToolBarArea, k->selectionTools);
    
    k->fillTools = new QToolBar(tr("Fill"));
    addToolBar(Qt::BottomToolBarArea, k->fillTools);
    
    k->viewTools = new QToolBar(tr("View"));
    addToolBar(Qt::BottomToolBarArea, k->viewTools);
    
    QTimer::singleShot(0, this, SLOT(loadTools()));
}


KTSymbolEditor::~KTSymbolEditor()
{
    delete k;
}


void KTSymbolEditor::setSymbol(KTLibraryObject *object)
{
    if (QGraphicsItem *item = qvariant_cast<QGraphicsItem *>(object->data())) {
        k->symbol = object;
        k->scene->addItem(item);
    }
}

void KTSymbolEditor::loadTools()
{
    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    
    foreach (QObject *plugin, KTPluginManager::instance()->tools()) {
             KTToolPlugin *tool = qobject_cast<KTToolPlugin *>(plugin);
        
             QStringList::iterator it;
             QStringList keys = tool->keys();
            
             for (it = keys.begin(); it != keys.end(); ++it) {
                  #ifdef K_DEBUG
                         tDebug("plugins") << "*** Tool Loaded: " << *it;
                  #endif
            
                  TAction *act = tool->actions()[*it];
                  if (act) {
                      connect(act, SIGNAL(triggered()), this, SLOT(selectTool()));
                
                      switch (tool->toolType()) {
                              case KTToolInterface::Selection:
                              {
                                   k->selectionTools->addAction(act);
                              }
                              break;
                              case KTToolInterface::Fill:
                              {
                                   k->fillTools->addAction(act);
                              }
                              break;
                              case KTToolInterface::View:
                              {
                                   k->viewTools->addAction(act);
                              }
                              break;
                              case KTToolInterface::Brush:
                              {
                                   k->brushTools->addAction(act);
                              }
                              break;
                      }
                
                      group->addAction(act);
                      act->setCheckable(true);
                      act->setParent(plugin);
                  }
             }
    }
}

void KTSymbolEditor::selectTool()
{
    #ifdef K_DEBUG
           T_FUNCINFO;                         
    #endif

    TAction *action = qobject_cast<TAction *>(sender());
    
    if (action) {
        KTToolPlugin *tool = qobject_cast<KTToolPlugin *>(action->parent());
        tool->setName(action->text());
    }
}
