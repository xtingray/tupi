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

#include "ktabbedmainwindow.h"

#include <QTabWidget>
#include <QToolButton>
#include <QPainter>
#include <QWheelEvent>
#include <QTabBar>

#include <QtDebug>

#ifndef TUPI_GUI
class TabWidgetPrivate : public QTabWidget
{
    Q_OBJECT;
    public:
        TabWidgetPrivate(QWidget *parent = 0);
        ~TabWidgetPrivate();

    protected:
        virtual void wheelEvent(QWheelEvent *e);
        virtual void wheelMove(int delta);
};

TabWidgetPrivate::TabWidgetPrivate(QWidget *parent) : QTabWidget(parent)
{
}

TabWidgetPrivate::~TabWidgetPrivate()
{
}

void TabWidgetPrivate::wheelEvent(QWheelEvent *ev)
{
    QRect rect = tabBar()->rect();
    rect.setWidth(width());

    if (rect.contains(ev->pos()))
        wheelMove(ev->delta());
}

void TabWidgetPrivate::wheelMove(int delta)
{
    if (count() > 1) {
        int current = currentIndex();
        if (delta < 0) {
            current = (current + 1) % count();
        } else {
            current--;
            if (current < 0)
                current = count() - 1;
        }
        setCurrentIndex(current);
    }
}

#include "ktabbedmainwindow.moc"

#else

#include <ktabwidget.h>
#define TabWidgetPrivate KTabWidget

#endif

// KTabbedMainWindow

/**
 * Construct a tabbed main window.
 * @param parent 
 * @return 
 */
KTabbedMainWindow::KTabbedMainWindow(QWidget *parent) : TMainWindow(parent)
{
    m_tabWidget = new TabWidgetPrivate;
    setupTabWidget(m_tabWidget);
    setCentralWidget(m_tabWidget);

    //connect(this, SIGNAL(perspectiveChanged(int)), this, SLOT(setupPerspective(int)));
}

/**
 * Destructor
 * @return 
 */
KTabbedMainWindow::~KTabbedMainWindow()
{
}

/**
 * Setup the tab widget.
 * @param w 
 */
void KTabbedMainWindow::setupTabWidget(QTabWidget *w)
{
    w->setFocusPolicy(Qt::NoFocus);
    connect(w, SIGNAL(currentChanged(int)), this, SLOT(emitWidgetChanged(int)));
}

void KTabbedMainWindow::addWidget(QWidget *widget, bool persistant, int perspective)
{
    if (perspective & currentPerspective())
        m_tabWidget->addTab(widget, widget->windowIcon(), widget->windowTitle());

    if (persistant)
        m_persistantWidgets << widget;

    m_pages << widget;
    m_tabs[widget] = perspective;
}

/**
 * Remove a widget from the window.
 * @param widget 
 */
void KTabbedMainWindow::removeWidget(QWidget *widget, bool force)
{
    if (force) 
        m_persistantWidgets.removeAll(widget);

    if (m_persistantWidgets.contains(widget)) 
        return;

    int index = m_tabWidget->indexOf(widget);
    if (index >= 0)
        m_tabWidget->removeTab(index);

    m_tabs.remove(widget);
    m_pages.removeAll(widget);
}

/**
 * Close the current tab.
 */
void KTabbedMainWindow::closeCurrentTab()
{
    int index = m_tabWidget->currentIndex();

    if (index >= 0)
        removeWidget(m_tabWidget->widget(index));
}

void KTabbedMainWindow::emitWidgetChanged(int index)
{
    if (index != -1) {
        //QWidget *w = m_tabWidget->widget(index);
        
       switch (index) {
               case 0:
                    setCurrentPerspective(Animation);
               break;
               case 1:
                    setCurrentPerspective(Player);
               break;
               case 2:
                    setCurrentPerspective(Help);
               break;
               case 3:
                    setCurrentPerspective(News);
               break;
       }

       //setCurrentPerspective(index + 1); 

       emit tabHasChanged(index);       
    }
}

/**
 * Sets other tab widget.
 * @param w 
 */
void KTabbedMainWindow::setTabWidget(QTabWidget *w)
{
    m_tabWidget->close();

    setupTabWidget(w);

    delete m_tabWidget;
    m_tabWidget = 0;

    setCentralWidget(w);

    m_tabWidget = w;
}

/**
 * Return the current tab widget.
 * @return 
 */
QTabWidget *KTabbedMainWindow::tabWidget() const
{
    return m_tabWidget;
}

/*
void KTabbedMainWindow::setupPerspective(int wps)
{
    // FIXME: It is Flickering =(
    m_tabWidget->setUpdatesEnabled(false);
    setUpdatesEnabled(false);

    foreach (QWidget *w, m_pages) {
             int perspective = m_tabs[w];

             if (wps & perspective) {
                 m_tabWidget->addTab(w, w->windowIcon(), w->windowTitle());
                 w->show();
             } else {
                 w->hide();
                 m_tabWidget->removeTab(m_tabWidget->indexOf(w));
             }
    }

    m_tabWidget->setUpdatesEnabled(true);
    setUpdatesEnabled(true);
	
    int count = m_tabWidget->count();

    for (int index = 0; index < count; index++) {
         QWidget *w = m_tabWidget->widget(index);

         if (m_tabs[w] == wps) {
             w->show();
             m_tabWidget->setTabEnabled(index, true);
         } else {
             w->hide();
             m_tabWidget->setTabEnabled(index, false);
         }
    }
}
*/

void KTabbedMainWindow::setCurrentTab(int index)
{
    if (index != -1) {
        m_tabWidget->setCurrentIndex(index);

        switch (index) {
               case 0:
                    setCurrentPerspective(Animation);
               break;
               case 1:
                    setCurrentPerspective(Player);
               break;
               case 2:
                    setCurrentPerspective(Help);
               break;
               case 3:
                    setCurrentPerspective(News);
               break;
        }
        //setCurrentPerspective(index + 1);
    }
}
