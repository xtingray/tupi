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

#include "tmainwindow.h"

class T_GUI_EXPORT DefaultSettings : public TMainWindowAbstractSettings
{
    public:
        DefaultSettings(QObject *parent);
        ~DefaultSettings();
        void save(TMainWindow *window);
        void restore(TMainWindow *window);
};

DefaultSettings::DefaultSettings(QObject *parent) : TMainWindowAbstractSettings(parent)
{
}

DefaultSettings::~DefaultSettings()
{
}

void DefaultSettings::save(TMainWindow *window)
{
    #ifdef K_DEBUG
            QString msg = "DefaultSettings::save() - Saving UI settings...";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    QSettings settings(qApp->applicationName(), "ideality", this);

    QHash<Qt::ToolBarArea, TButtonBar *> buttonBars = window->buttonBars();
    QHash<TButtonBar *, QList<ToolView*> > toolViews = window->toolViews();

    foreach (TButtonBar *bar, buttonBars.values()) {
             settings.beginGroup(bar->windowTitle());
             settings.setValue("exclusive", bar->isExclusive());
             settings.endGroup();
             settings.beginGroup(bar->windowTitle());
             settings.setValue("autohide", bar->autohide());
             settings.endGroup();
             settings.beginGroup(bar->windowTitle());
             settings.setValue("visible", bar->isVisible());
             settings.endGroup();

             foreach (ToolView *view, toolViews[bar]) {
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
    settings.setValue("size", window->size());
    settings.setValue("maximized", window->isMaximized());
    settings.setValue("position", window->pos());
    settings.endGroup();
}

void DefaultSettings::restore(TMainWindow *window)
{
    #ifdef K_DEBUG
	    QString msg = "DefaultSettings::restore() - Restoring UI settings...";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    QSettings settings(qApp->applicationName(), "ideality", this);

    QHash<Qt::ToolBarArea, TButtonBar *> buttonBars = window->buttonBars();
    QHash<TButtonBar *, QList<ToolView*> > toolViews = window->toolViews();

    QList<ToolView *> toHide;

    foreach (TButtonBar *bar, buttonBars.values()) {
             bar->setExclusive(false);

             foreach (ToolView *view, toolViews[bar]) {
                      settings.beginGroup(view->objectName());

                      // Restore position
                      Qt::DockWidgetArea area = Qt::DockWidgetArea(settings.value("area", 0).toInt());
                      window->moveToolView(view, area);
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

             settings.beginGroup(bar->windowTitle());
             bar->setShouldBeVisible(settings.value("visible", true).toBool());
             settings.endGroup();
    }
	
    foreach (ToolView *view, toHide) {
             view->button()->setChecked(false);
             view->setVisible(false);
             view->close();
    }

    settings.beginGroup("MainWindow");
    window->resize(settings.value("size").toSize());
    bool maximized = settings.value("maximized", false).toBool();

    if (maximized)
        window->showMaximized();

    window->move(settings.value("position").toPoint());

    settings.endGroup();
}

// TMainWindow


/**
 * Construct a main window
 * @param parent 
 * @return 
 */
TMainWindow::TMainWindow(QWidget *parent)
           : QMainWindow(parent), m_forRelayout(0), m_currentPerspective(DefaultPerspective), m_autoRestore(false)
{
    setObjectName("TMainWindow");

    m_settings = new DefaultSettings(this);

    specialToolBar = new QToolBar(tr("Show Top Panel"), this);
    specialToolBar->setIconSize(QSize(9, 5));
    addToolBar(Qt::LeftToolBarArea, specialToolBar);

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
TMainWindow::~TMainWindow()
{
}

void TMainWindow::addButtonBar(Qt::ToolBarArea area)
{
    TButtonBar *bar = new TButtonBar(area, this);
    addToolBar(area, bar);
    m_buttonBars.insert(area, bar);

    bar->hide();
}

void TMainWindow::enableSpecialBar(bool flag)
{
    specialToolBar->setVisible(flag);
}

void TMainWindow::addSpecialButton(TAction *action)
{
    specialToolBar->addAction(action);
}

/**
 * Add a tool view to the main window in the area and perspective.
 * 
 * @param widget 
 * @param area 
 * @param perspective 
 * @return 
 */
ToolView *TMainWindow::addToolView(QWidget *widget, Qt::DockWidgetArea area, int perspective, const QString &code, QKeySequence shortcut)
{
    ToolView *toolView = new ToolView(widget->windowTitle(), widget->windowIcon(), code);
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

void TMainWindow::removeToolView(ToolView *view)
{
    bool findIt = false;

    foreach (TButtonBar *bar, m_buttonBars.values()) {
             QList<ToolView *> views = m_toolViews[bar];
             QList<ToolView *>::iterator it = views.begin();

             while (it != views.end()) {
                    ToolView *v = *it;
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

void TMainWindow::enableToolViews(bool flag)
{
    foreach (TButtonBar *bar, m_buttonBars.values()) {
             QList<ToolView *> views = m_toolViews[bar];
             QList<ToolView *>::iterator it = views.begin();

             while (it != views.end()) {
                    ToolView *v = *it;
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
void TMainWindow::moveToolView(ToolView *view, Qt::DockWidgetArea newPlace)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TMainWindow::moveToolView()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

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
void TMainWindow::addToPerspective(QWidget *widget, int perspective)
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
void TMainWindow::removeFromPerspective(QWidget *widget)
{
    m_managedWidgets.remove(widget);
}

/**
 * Adds a QAction list to perspective
 * @param actions 
 * @param perspective 
 */
void TMainWindow::addToPerspective(const QList<QAction *> &actions, int perspective)
{
    foreach (QAction *a, actions)
             addToPerspective(a, perspective);
}

/**
 * Adds an action to perspective
 * @param action 
 * @param perspective 
 */
void TMainWindow::addToPerspective(QAction *action, int perspective)
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
void TMainWindow::removeFromPerspective(QAction *action)
{
    m_managedActions.remove(action);
}

Qt::DockWidgetArea TMainWindow::toDockWidgetArea(Qt::ToolBarArea area)
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
               {
                   #ifdef K_DEBUG
                       QString msg = "TMainWindow::toDockWidgetArea() - Floating -> " + QString::number(area);
                       #ifdef Q_OS_WIN
                           qWarning() << msg;
                       #else
                           tWarning() << msg;
                       #endif
                   #endif
               }
               break;
    }

    return Qt::LeftDockWidgetArea;
}

Qt::ToolBarArea TMainWindow::toToolBarArea(Qt::DockWidgetArea area)
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
               {
                 #ifdef K_DEBUG
                     QString msg = "TMainWindow::toToolBarArea() - Floating -> " + QString::number(area);
                     #ifdef Q_OS_WIN
                         qWarning() << msg;
                     #else
                         tWarning() << msg;
                     #endif
                 #endif
               }
               break;
    }

    return Qt::LeftToolBarArea;
}


/**
 * Enable/disable button blending.
 * @param enable 
 */
void TMainWindow::setEnableButtonBlending(bool enable)
{
    foreach (TButtonBar *bar, m_buttonBars.values())
             bar->setEnableButtonBlending(enable);
}

void TMainWindow::relayoutViewButton(bool topLevel)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TMainWindow::relayoutViewButton()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (!topLevel) {
        if (ToolView *toolView = dynamic_cast<ToolView *>(sender())) {
            m_forRelayout = toolView;
            QTimer::singleShot(0, this, SLOT(relayoutToolView()));

            // if a tool view is floating the button bar isn't exclusive
            TButtonBar *bar = m_buttonBars[m_forRelayout->button()->area()];
            bool exclusive = true;
            foreach (ToolView *v, m_toolViews[bar])
                     exclusive = exclusive && !v->isFloating();

            bar->setExclusive(exclusive);
            bar->onlyShow(m_forRelayout, true);
        }
    } else {
            // Floating tool views aren't exclusive
            if (ToolView *toolView = dynamic_cast<ToolView *>(sender()))
                m_buttonBars[toolView->button()->area()]->setExclusive(false);
    }
}

void TMainWindow::relayoutToolView()
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TMainWindow::relayoutToolView()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (!m_forRelayout) 
        return;

    bool isVisible = m_forRelayout->isVisible();

    if (!isVisible) 
        m_forRelayout->show();

    TViewButton *button = m_forRelayout->button();

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
void TMainWindow::setCurrentPerspective(int workspace)
{
    if (workspace != 1)
        specialToolBar->setVisible(false);
    else
        specialToolBar->setVisible(true);

    if (m_currentPerspective == workspace)
        return;

    typedef QList<ToolView *> Views;
    QList<Views > viewsList = m_toolViews.values();

    setUpdatesEnabled(false);

    if (centralWidget())
        centralWidget()->setUpdatesEnabled(false);

    QHash<TButtonBar *, int> hideButtonCount;

    foreach (Views views, viewsList) {
             foreach (ToolView *view, views) {
                      TButtonBar *bar = m_buttonBars[view->button()->area()];
                      bar->setUpdatesEnabled(false);
                      view->setUpdatesEnabled(false);

                      if (view->perspective() & workspace) {
                          bar->enable(view->button());
 
                          // if (view->button()->isChecked()) {
                          if (view->isChecked())
                              view->show(); 
                      } else {
                              bar->disable(view->button());
                              // if (view->button()->isChecked() || view->isVisible())
                              if (view->isChecked() || view->isVisible())
                                  view->close();
                              hideButtonCount[bar]++;
                      }

                      if (bar->isEmpty() && bar->isVisible()) {
                          bar->hide();
                      } else {
                          if (!bar->isVisible() && bar->shouldBeVisible())
                              bar->show();
                      }

                      view->setUpdatesEnabled(true);
                      bar->setUpdatesEnabled(true);
             }
    }

    QHashIterator<TButtonBar *, int> barIt(hideButtonCount);

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
int TMainWindow::currentPerspective() const
{
    return m_currentPerspective;
}

/**
 * if autoRestore is true, the widgets will be loaded when main window is showed (position and properties).
 * @param autoRestore 
 */
void TMainWindow::setAutoRestore(bool autoRestore)
{
    m_autoRestore = autoRestore;
}

/**
 * Returns the autoRestore property.
 * @return 
 */
bool TMainWindow::autoRestore() const
{
    return m_autoRestore;
}

QMenu *TMainWindow::createPopupMenu()
{
    QMenu *menu = QMainWindow::createPopupMenu();
    menu->addSeparator();

    return menu;
}

void TMainWindow::setSettingsHandler(TMainWindowAbstractSettings *settings)
{
    delete m_settings;

    m_settings = settings;
    m_settings->setParent(this);
}

void TMainWindow::closeEvent(QCloseEvent *e)
{
    saveGUI();
    QMainWindow::closeEvent(e);
}

void TMainWindow::showEvent(QShowEvent *e)
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

bool TMainWindow::event(QEvent *e)
{
    if (e->type() == QEvent::HoverMove) {
        // Show bar if autohide is enabled
        QPoint pos = mapFromGlobal(QCursor::pos());
        TButtonBar *bar = 0;

        if (pos.x() <= m_buttonBars[Qt::LeftToolBarArea]->pos().x() + 3 ) { // Left
            bar = m_buttonBars[Qt::LeftToolBarArea];
        } else if (pos.y() <= m_buttonBars[Qt::TopToolBarArea]->pos().y() + 3 && m_buttonBars[Qt::TopToolBarArea]->pos().y() <= pos.y()) {
                   bar = m_buttonBars[Qt::TopToolBarArea];
        } else if (pos.x() >= m_buttonBars[Qt::RightToolBarArea]->pos().x() + m_buttonBars[Qt::RightToolBarArea]->width() - 3 ) {
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

void TMainWindow::saveGUI()
{
    m_settings->save(this);
}

void TMainWindow::restoreGUI()
{
    setUpdatesEnabled(false);
    m_settings->restore(this);
    setUpdatesEnabled(true);
}

/**
 * Returns the button bars
 * @return 
 */
QHash<Qt::ToolBarArea, TButtonBar *> TMainWindow::buttonBars() const
{
    return m_buttonBars;
}

/**
 * Returs the tool views.
 * @return 
 */
QHash<TButtonBar *, QList<ToolView*> > TMainWindow::toolViews() const
{
    return m_toolViews;
}
