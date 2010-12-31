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

#include "kmainwindow.h"
#include "kbuttonbar.h"
#include "ktoolview.h"
#include "kviewbutton.h"
#include "kmainwindowabstractsettings.h"

#include "kdebug.h"

#include <QTimer>
#include <QMenu>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QApplication>
#include <QtDebug>
#include <QHashIterator>
#include <QMenuBar>

#include <QCloseEvent>

class DefaultSettings : public KMainWindowAbstractSettings
{
    public:
        DefaultSettings(QObject *parent);
        ~DefaultSettings();
        void save(KMainWindow *w);
        void restore(KMainWindow *w);
};

DefaultSettings::DefaultSettings(QObject *parent) : KMainWindowAbstractSettings(parent)
{
}

DefaultSettings::~DefaultSettings()
{
}

void DefaultSettings::save(KMainWindow *w)
{
    QSettings settings(qApp->applicationName(), "ideality", this);

    QHash<Qt::ToolBarArea, KButtonBar *> buttonBars = w->buttonBars();
    QHash<KButtonBar *, QList<KToolView*> > toolViews = w->toolViews();

    foreach (KButtonBar *bar, buttonBars.values()) {
             settings.beginGroup(bar->windowTitle());
             settings.setValue("exclusive", bar->isExclusive());
             settings.endGroup();
             settings.beginGroup(bar->windowTitle());
             settings.setValue("autohide", bar->autohide());
             settings.endGroup();

             foreach (KToolView *view, toolViews[bar]) {
                      settings.beginGroup(view->objectName());
                      settings.setValue("area", int(view->button()->area()));
                      settings.setValue("size", view->fixedSize());
                      settings.setValue("style", view->button()->toolButtonStyle());
                      settings.setValue("sensibility", view->button()->isSensible());
                      settings.setValue("visible", view->isVisible() );
                      settings.setValue("floating", view->isFloating());
                      settings.setValue("position", view->pos());
                      settings.endGroup();
             }
    }

    settings.beginGroup("MainWindow");
    settings.setValue("size", w->size());
    settings.setValue("maximized", w->isMaximized());
    settings.setValue("position", w->pos());
    settings.endGroup();
}

void DefaultSettings::restore(KMainWindow *w)
{
    kFatal() << "*** Restoring Interface...";

    QSettings settings(qApp->applicationName(), "ideality", this);

    QHash<Qt::ToolBarArea, KButtonBar *> buttonBars = w->buttonBars();
    QHash<KButtonBar *, QList<KToolView*> > toolViews = w->toolViews();

    QList<KToolView *> toHide;

    foreach (KButtonBar *bar, buttonBars.values()) {
             bar->setExclusive(false);

             foreach (KToolView *view, toolViews[bar]) {
                      settings.beginGroup(view->objectName());

                      // Restore position
                      Qt::DockWidgetArea area = Qt::DockWidgetArea(settings.value("area", 0).toInt());
                      w->moveToolView(view, area);
                      view->setFixedSize(settings.value("size").toInt());
                      view->button()->setToolButtonStyle(Qt::ToolButtonStyle(settings.value("style", 
                                                         int(view->button()->toolButtonStyle())).toInt()));
                      view->button()->setSensible(settings.value("sensibility", view->button()->isSensible()).toBool());

                      bool visible = settings.value("visible", false).toBool();

                      if (visible && view->button()->isVisible()) {
                          view->button()->setChecked(true);
                          view->show();
                      } else {
                          toHide << view;
                      }

                      view->setFloating(settings.value("floating", false).toBool());

                      if (view->isFloating())
                          view->move(settings.value("position").toPoint());

                      settings.endGroup();
             }

             settings.beginGroup(bar->windowTitle());
             bar->setExclusive(settings.value("exclusive", true).toBool());
             settings.endGroup();

             settings.beginGroup(bar->windowTitle());
             bar->setAutoHide(settings.value("autohide", false).toBool());
             settings.endGroup();
    }
	
    foreach (KToolView *v, toHide) {
             v->button()->setChecked(false);
             v->setVisible(false);
             v->close();
    }

    settings.beginGroup("MainWindow");
    w->resize(settings.value("size").toSize());
    bool maximized = settings.value("maximized", false).toBool();

    if (maximized)
        w->showMaximized();

    w->move(settings.value("position").toPoint());

    settings.endGroup();
}

// KMainWindow


/**
 * Construct a main window
 * @param parent 
 * @return 
 */
KMainWindow::KMainWindow(QWidget *parent)
           : QMainWindow(parent), m_forRelayout(0), m_currentPerspective(DefaultPerspective), m_autoRestore(false)
{
    setObjectName("KMainWindow");

    m_settings = new DefaultSettings(this);

    addButtonBar(Qt::LeftToolBarArea);
    addButtonBar(Qt::RightToolBarArea);
    addButtonBar(Qt::TopToolBarArea);
    addButtonBar(Qt::BottomToolBarArea);

    setDockNestingEnabled(false);
}

/**
 * Default destructor
 * @return 
 */
KMainWindow::~KMainWindow()
{
}

void KMainWindow::addButtonBar(Qt::ToolBarArea area)
{
    KButtonBar *bar = new KButtonBar(area, this);
    addToolBar(area, bar);
    m_buttonBars.insert(area, bar);

    bar->hide();
}

/**
 * Add a tool view to the main window in the area and perspective.
 * 
 * @param widget 
 * @param area 
 * @param perspective 
 * @return 
 */
KToolView *KMainWindow::addToolView(QWidget *widget, Qt::DockWidgetArea area, int perspective, const QString &code, QKeySequence shortcut)
{
    KToolView *toolView = new KToolView(widget->windowTitle(), widget->windowIcon(), code);
    toolView->setShortcut(shortcut);
    toolView->setWidget(widget);
    toolView->setPerspective(perspective);
    toolView->button()->setArea(toToolBarArea(area));
    m_buttonBars[toToolBarArea(area)]->addButton(toolView->button());

    addDockWidget(area, toolView);
    widget->show();

    m_toolViews[m_buttonBars[toToolBarArea(area)]] << toolView;

    // Show separators
    if (area == Qt::TopDockWidgetArea || area == Qt::BottomDockWidgetArea) 
        m_buttonBars[toToolBarArea(area)]->showSeparator(! m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
    else if (area == Qt::LeftDockWidgetArea)
             m_buttonBars[Qt::TopToolBarArea]->showSeparator(m_buttonBars[Qt::TopToolBarArea]->isEmpty());

    connect(toolView, SIGNAL(topLevelChanged(bool)), this, SLOT(relayoutViewButton(bool)));

    if (toolView->isVisible())
        toolView->button()->click(); // Hide!

    return toolView;
}

void KMainWindow::removeToolView(KToolView *view)
{
    bool findIt = false;

    foreach (KButtonBar *bar, m_buttonBars.values()) {
             QList<KToolView *> views = m_toolViews[bar];
             QList<KToolView *>::iterator it = views.begin();

             while (it != views.end()) {
                    KToolView *v = *it;
                    if (v == view) {
                        views.erase(it);
                        bar->removeButton(view->button());
                        findIt = true;
                        break;
                    }
                    ++it;
             }

             if (findIt) 
                 break;
    }

    if (findIt)
        removeDockWidget(view);
}

void KMainWindow::enableToolViews(bool flag)
{
    foreach (KButtonBar *bar, m_buttonBars.values()) {
             QList<KToolView *> views = m_toolViews[bar];
             QList<KToolView *>::iterator it = views.begin();

             while (it != views.end()) {
                    KToolView *v = *it;

                    // SQA: Temporary code while Time Line components are fixed
                    if (v->getObjectID().compare("KToolView-Time Line")==0)
                        v->enableButton(false);
                    else
                        v->enableButton(flag);

                    ++it;
             }
    }
}

/**
 * Moves a tool view to newPlace
 * @param view 
 * @param newPlace 
 */
void KMainWindow::moveToolView(KToolView *view, Qt::DockWidgetArea newPlace)
{
    if (toDockWidgetArea(view->button()->area()) == newPlace || newPlace == Qt::AllDockWidgetAreas || newPlace == 0)
        return;

    addDockWidget(newPlace, view);
    m_forRelayout = view;
    relayoutToolView();
}

/**
 * Add a widget to perspective
 * @param widget 
 * @param perspective 
 */
void KMainWindow::addToPerspective(QWidget *widget, int perspective)
{
    if (QToolBar *bar = dynamic_cast<QToolBar*>(widget)) {
        if (toolBarArea(bar) == 0)
            addToolBar(bar);
    }

    if (! m_managedWidgets.contains(widget)) {
        m_managedWidgets.insert(widget, perspective);

        if (!(perspective & m_currentPerspective)) 
            widget->hide();
    }
}

/**
 * Remove widget from perspective
 * @param widget 
 */
void KMainWindow::removeFromPerspective(QWidget *widget)
{
    m_managedWidgets.remove(widget);
}

/**
 * Adds a QAction list to perspective
 * @param actions 
 * @param perspective 
 */
void KMainWindow::addToPerspective(const QList<QAction *> &actions, int perspective)
{
    foreach (QAction *a, actions)
             addToPerspective(a, perspective);
}

/**
 * Adds an action to perspective
 * @param action 
 * @param perspective 
 */
void KMainWindow::addToPerspective(QAction *action, int perspective)
{
    if (! m_managedActions.contains(action)) {
        m_managedActions.insert(action, perspective);

        if (!(perspective & m_currentPerspective))
            action->setVisible(false);
    }
}

/**
 * Remove an action from perspective
 * @param action 
 */
void KMainWindow::removeFromPerspective(QAction *action)
{
    m_managedActions.remove(action);
}

Qt::DockWidgetArea KMainWindow::toDockWidgetArea(Qt::ToolBarArea area)
{
    switch (area) {
            case Qt::LeftToolBarArea:
               {
                 return Qt::LeftDockWidgetArea;
               }
               break;
            case Qt::RightToolBarArea:
               {
                 return Qt::RightDockWidgetArea;
               }
               break;
            case Qt::TopToolBarArea:
               {
                 return Qt::TopDockWidgetArea;
               }
               break;
            case Qt::BottomToolBarArea:
               {
                 return Qt::BottomDockWidgetArea;
               }
               break;
            default: 
                 qWarning("toDockWidgetArea: Floating... %d", area); 
                 break;
    }

    return Qt::LeftDockWidgetArea;
}

Qt::ToolBarArea KMainWindow::toToolBarArea(Qt::DockWidgetArea area)
{
    switch (area) {
            case Qt::LeftDockWidgetArea:
               {
                 return Qt::LeftToolBarArea;
               }
               break;
            case Qt::RightDockWidgetArea:
               {
                 return Qt::RightToolBarArea;
               }
               break;
            case Qt::TopDockWidgetArea:
               {
                 return Qt::TopToolBarArea;
               }
               break;
            case Qt::BottomDockWidgetArea:
               {
                 return Qt::BottomToolBarArea;
               }
               break;
            default: 
                 qWarning("toToolBarArea: Floating... %d", area); 
                 break;
    }

    return Qt::LeftToolBarArea;
}


/**
 * Enable/disable button blending.
 * @param enable 
 */
void KMainWindow::setEnableButtonBlending(bool enable)
{
    foreach (KButtonBar *bar, m_buttonBars.values())
             bar->setEnableButtonBlending(enable);
}

void KMainWindow::relayoutViewButton(bool topLevel)
{
    if (!topLevel) {
        if (KToolView *toolView = dynamic_cast<KToolView *>(sender())) {
            m_forRelayout = toolView;

            QTimer::singleShot(0, this, SLOT(relayoutToolView()));

            // if a tool view is floating the button bar isn't exclusive
            KButtonBar *bar = m_buttonBars[m_forRelayout->button()->area()];

            bool exclusive = true;

            foreach (KToolView *v, m_toolViews[bar])
                     exclusive = exclusive && !v->isFloating();

            bar->setExclusive(exclusive);
            bar->onlyShow(m_forRelayout, true);
        }
    } else {
            // Floating tool views aren't exclusive
            if (KToolView *toolView = dynamic_cast<KToolView *>(sender()))
                m_buttonBars[toolView->button()->area()]->setExclusive(false);
    }
}

void KMainWindow::relayoutToolView()
{
    if (!m_forRelayout) 
        return;

    bool isVisible = m_forRelayout->isVisible();

    if (!isVisible) 
        m_forRelayout->show();

    KViewButton *button = m_forRelayout->button();

    Qt::ToolBarArea area = toToolBarArea(QMainWindow::dockWidgetArea(m_forRelayout));

    if (!isVisible) 
        m_forRelayout->close();

    if (area != button->area() && !m_forRelayout->isFloating()) {
        setUpdatesEnabled(false);
        m_buttonBars[button->area()]->removeButton(button);
        // Show separators
        if (button->area() == Qt::LeftToolBarArea) {
            m_buttonBars[Qt::BottomToolBarArea]->showSeparator(! m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
            m_buttonBars[Qt::TopToolBarArea]->showSeparator(! m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
        } else if (area == Qt::LeftToolBarArea) {
                   m_buttonBars[Qt::BottomToolBarArea]->showSeparator(m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
                   m_buttonBars[Qt::TopToolBarArea]->showSeparator(m_buttonBars[Qt::LeftToolBarArea]->isEmpty());
        }
        //////////

        m_toolViews[m_buttonBars[button->area()]].removeAll(m_forRelayout);
        m_toolViews[m_buttonBars[area]] << m_forRelayout;
        button->setArea(area);
        m_buttonBars[area]->addButton(button);
        m_buttonBars[area]->repaint();
        setUpdatesEnabled(true);
    }

    m_forRelayout = 0;
}

/**
 * Sets the current perspective.
 * @param workspace
 */
void KMainWindow::setCurrentPerspective(int workspace)
{
    if (m_currentPerspective == workspace)
        return;

    typedef QList<KToolView *> Views;

    QList<Views > viewsList = m_toolViews.values();

    setUpdatesEnabled(false);

    if (centralWidget())
        centralWidget()->setUpdatesEnabled(false);

    QHash<KButtonBar *, int> hideButtonCount;

    foreach (Views views, viewsList) {
             foreach (KToolView *view, views) {
                      KButtonBar *bar = m_buttonBars[view->button()->area()];
                      bar->setUpdatesEnabled(false);
                      view->setUpdatesEnabled(false);

                      if (view->perspective() & workspace) {

                          // SQA: Temporary code while Library and Time Line components are fixed
                          if (view->getObjectID().compare("KToolView-Time Line")==0) {
                              bar->enable(view->button());
                              view->enableButton(false);
                          } else {
                              bar->enable(view->button());
                          } 

                          // bar->enable(view->button());
 
                          if (view->button()->isChecked() && (workspace != 4) && view->getObjectID().compare("KToolView-Help")!=0) {
                              view->show();
                          }
                      } else {
                              bar->disable(view->button());
                              if (view->button()->isChecked() || view->isVisible())
                                  view->close();
                              hideButtonCount[bar]++;
                      }

                      if (bar->isEmpty() && bar->isVisible()) {
                          bar->hide();
                      } else {
                          if (!bar->isVisible())
                              bar->show();
                      }

                      view->setUpdatesEnabled(true);
                      bar->setUpdatesEnabled(true);
             }
    }

    QHashIterator<KButtonBar *, int> barIt(hideButtonCount);

    // This loop hides the bars with no buttons
    while (barIt.hasNext()) {
           barIt.next();

           if (barIt.key()->count() == barIt.value())
               barIt.key()->hide();
    }

    QHashIterator<QWidget *, int> widgetIt(m_managedWidgets);

    while (widgetIt.hasNext()) {
           widgetIt.next();

           if (widgetIt.value() & workspace) 
               widgetIt.key()->show();
           else
               widgetIt.key()->hide();
    }

    QHashIterator<QAction *, int> actionIt(m_managedActions);

    while (actionIt.hasNext()) {
           actionIt.next();

           if (actionIt.value() & workspace)
               actionIt.key()->setVisible(true);
           else
               actionIt.key()->setVisible(false);
    }

    if (centralWidget())
        centralWidget()->setUpdatesEnabled(true);

    setUpdatesEnabled(true);
    m_currentPerspective = workspace;

    emit perspectiveChanged(m_currentPerspective);
}

/**
 * Returns the current perspective
 * @return 
 */
int KMainWindow::currentPerspective() const
{
    return m_currentPerspective;
}

/**
 * if autoRestore is true, the widgets will be loaded when main window is showed (position and properties).
 * @param autoRestore 
 */
void KMainWindow::setAutoRestore(bool autoRestore)
{
    m_autoRestore = autoRestore;
}

/**
 * Returns the autoRestore property.
 * @return 
 */
bool KMainWindow::autoRestore() const
{
    return m_autoRestore;
}

QMenu *KMainWindow::createPopupMenu()
{
    QMenu *menu = QMainWindow::createPopupMenu();
    menu->addSeparator();

    return menu;
}

void KMainWindow::setSettingsHandler(KMainWindowAbstractSettings *settings)
{
    delete m_settings;

    m_settings = settings;
    m_settings->setParent(this);
}

void KMainWindow::closeEvent(QCloseEvent *e)
{
    saveGUI();
    QMainWindow::closeEvent(e);
}

void KMainWindow::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);

    if (!m_autoRestore) {
        m_autoRestore = true;
        restoreGUI();
        int cwsp = m_currentPerspective;
        m_currentPerspective -= 1;
        setCurrentPerspective(cwsp);
    }
}

bool KMainWindow::event(QEvent *e)
{
    if (e->type() == QEvent::HoverMove) {
        // Show bar if autohide is enabled
        QPoint pos = mapFromGlobal(QCursor::pos());
        KButtonBar *bar = 0;

        if (pos.x() <= m_buttonBars[Qt::LeftToolBarArea]->pos().x() + 3 ) { // Left

            bar = m_buttonBars[Qt::LeftToolBarArea];

        } else if (pos.y() <= m_buttonBars[Qt::TopToolBarArea]->pos().y() + 3 
                   && m_buttonBars[Qt::TopToolBarArea]->pos().y() <= pos.y()) {

            bar = m_buttonBars[Qt::TopToolBarArea];

        } else if (pos.x() >= m_buttonBars[Qt::RightToolBarArea]->pos().x() + 
                   m_buttonBars[Qt::RightToolBarArea]->width() - 3 ) {

            bar = m_buttonBars[Qt::RightToolBarArea];

        } else if (pos.y() >= m_buttonBars[Qt::BottomToolBarArea]->pos().y() +  
                   m_buttonBars[Qt::BottomToolBarArea]->height() - 3 
                   && m_buttonBars[Qt::BottomToolBarArea]->pos().y() + 
                   m_buttonBars[Qt::BottomToolBarArea]->height() > pos.y()) {

            bar = m_buttonBars[Qt::BottomToolBarArea];

        }

        if (bar) {
            if (bar->autohide() && !bar->isVisible() && !bar->isEmpty())
                bar->show();
        }
    }

    return QMainWindow::event(e);
}

void KMainWindow::saveGUI()
{
    m_settings->save(this);
}

void KMainWindow::restoreGUI()
{
    setUpdatesEnabled(false);
    m_settings->restore(this);
    setUpdatesEnabled(true);
}

/**
 * Returns the button bars
 * @return 
 */
QHash<Qt::ToolBarArea, KButtonBar *> KMainWindow::buttonBars() const
{
    return m_buttonBars;
}

/**
 * Returs the tool views.
 * @return 
 */
QHash<KButtonBar *, QList<KToolView*> > KMainWindow::toolViews() const
{
    return m_toolViews;
}
