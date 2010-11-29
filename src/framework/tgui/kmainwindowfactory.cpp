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

#include "kmainwindowfactory.h"

#include <ktabbedmainwindow.h>
#include <kstackedmainwindow.h>
#include <kworkspacemainwindow.h>

#include <QDockWidget>
#include <QToolBar>
#include <QTabWidget>
#include <QEvent>
#include <QApplication>
#include <QShowEvent>

#include <QtDebug>


class EventFilter : public QObject
{
    public:
        EventFilter(KMainWindow *mw, QObject *parent = 0);
        ~EventFilter();

    protected:
        bool eventFilter(QObject *obj, QEvent *event);

    private:
        KMainWindow *m_mainWindow;
};

EventFilter::EventFilter(KMainWindow *mw, QObject *parent) : QObject(parent), m_mainWindow(mw)
{
}

EventFilter::~EventFilter()
{
}

bool EventFilter::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    if (event->type() == QEvent::Close) {
        m_mainWindow->close();
        return true;
    } else if (event->type() == QEvent::Show) {
               m_mainWindow->show();
               return true;
    } else if (event->type() == QEvent::Hide) {
               m_mainWindow->hide();
               return true;
    }

    return false;
}

/**
 * Construct a factory.
 * @return 
 */
KMainWindowFactory::KMainWindowFactory()
{
}

/**
 * Destructor
 * @return 
 */
KMainWindowFactory::~KMainWindowFactory()
{
}

/**
 * if centralWidget() is a QTabWidget an instance of KTabbedMainWindow will be created
 * @param other 
 * @return 
 */
KMainWindow *KMainWindowFactory::create(QMainWindow *other)
{
    KMainWindow *mainWindow = 0;

    if (other->isVisible())
        other->hide();

    if (other->inherits("KMainWindow")) {
        qWarning() << QObject::tr("Can't create a KMainWindow from KMainWindow");
        return static_cast<KMainWindow *>(other);
    }

    if (QWidget *central = other->centralWidget()) {
        if (QTabWidget *tabWidget = dynamic_cast<QTabWidget *>(central)) {
            mainWindow = new KTabbedMainWindow;
            static_cast<KTabbedMainWindow *>(mainWindow)->setTabWidget(tabWidget);
        } else {
            mainWindow = new KMainWindow;
            central->setParent(mainWindow);
            mainWindow->setCentralWidget(central);
        }
    }

    QList<QDockWidget *> docks = other->findChildren<QDockWidget *>();

    foreach (QDockWidget *dock, docks) {
             dock->widget()->setWindowTitle(dock->windowTitle());
             dock->widget()->setWindowIcon(dock->windowIcon());

             Qt::DockWidgetArea area = other->dockWidgetArea(dock);

             if (area == 0)
                 area = Qt::LeftDockWidgetArea;

             mainWindow->addToolView(dock->widget(), area);
    }
	
    QList<QToolBar *> toolBars = other->findChildren<QToolBar *>();

    foreach (QToolBar *toolBar, toolBars) {
             toolBar->setParent(mainWindow);

             Qt::ToolBarArea area = other->toolBarArea(toolBar);

             if (area == 0)
                 area = Qt::TopToolBarArea;

             mainWindow->addToolBar(area, toolBar);
    }

    mainWindow->setStatusBar(other->statusBar());
    mainWindow->setMenuBar(other->menuBar());
    mainWindow->setCurrentPerspective(KMainWindow::DefaultPerspective);

    EventFilter *eventFilter = new EventFilter(mainWindow, other);
    other->installEventFilter(eventFilter);

    return mainWindow;
}
