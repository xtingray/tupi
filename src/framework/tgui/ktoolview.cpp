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

#include "ktoolview.h"
#include "kviewbutton.h"
#include "kmainwindow.h"
#include "kdebug.h"

#include <QtDebug>
#include <QAction>
#include <QMainWindow>
#include <QMouseEvent>
#include <QLayout>
#include <QEvent>

KToolView::KToolView(const QString &title, const QIcon &icon, const QString &code, QWidget * parent)
          : QDockWidget(title, parent), m_size(-1), m_perspective(0)
{
    setWindowIcon(icon);
    setup(title);
    setObjectName("KToolView-" + code);
    expanded = false;
}

KToolView::~KToolView()
{
}

void KToolView::setup(const QString &label)
{
    setFeatures(AllDockWidgetFeatures);
    m_button = new KViewButton(this);
    m_button->setToolTip(label);

    connect(toggleViewAction(), SIGNAL(toggled(bool)), this, SLOT(saveSize(bool)));
}

KViewButton *KToolView::button() const
{
    return m_button;
}

void KToolView::expandDock(bool flag)
{
    expanded = flag;

    if (flag) {
        show();
    } else { 
        close();
    }

    //m_button->setChecked(flag);
    m_button->setActivated(flag);
}

bool KToolView::isExpanded()
{
    return expanded;
}

void KToolView::setExpandingFlag() {
    if (expanded)
        expanded = false;
    else
        expanded = true;
}

void KToolView::setSizeHint() {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void KToolView::saveSize(bool checked)
{
    if (m_button->area() == Qt::LeftToolBarArea || m_button->area() == Qt::RightToolBarArea)
        m_size = width();
    else
        m_size = height();

    //setVisible(checked);
}

QSize KToolView::sizeHint() const
{
    QSize size = QDockWidget::sizeHint();

    if (m_size < 0) 
        return size;

    if (m_button->area() == Qt::LeftToolBarArea || m_button->area()  == Qt::RightToolBarArea)
        size.setWidth(m_size);
    else
        size.setHeight(m_size);

    return size;
}

void KToolView::setDescription(const QString &description)
{
    m_button->setStatusTip(description);
}

void KToolView::setShortcut(QKeySequence shortcut)
{
    m_button->setShortcut(shortcut);
}

void KToolView::setPerspective(int wsp)
{
    m_perspective = wsp;
}

int KToolView::perspective() const
{
    return m_perspective;
}

int KToolView::fixedSize() const
{
    return m_size;
}

void KToolView::setFixedSize(int s)
{
    m_size = s;
}

void KToolView::showEvent(QShowEvent *e)
{
    if (KMainWindow *mw = dynamic_cast<KMainWindow *>(parentWidget())) {
        if (!(mw->currentPerspective() & m_perspective)) {
            e->ignore(); // make sure!
            return;
        }
    }

    QDockWidget::showEvent(e);
}

void KToolView::enableButton(bool flag)
{
    m_button->setEnabled(flag);
}

QString KToolView::getObjectID() {
    return objectName();
}

bool KToolView::isChecked()
{
    if (m_button->isChecked())
        return true;

    return false;
}

/*
#if QT_VERSION < 0x040200

bool KToolView::event(QEvent *e)
{
    bool toReturn =  QDockWidget::event(e);

    if (e->type() == QEvent::MouseButtonPress) {
        if (QMainWindow *mw = dynamic_cast<QMainWindow *>(parentWidget())) {
            m_area = mw->dockWidgetArea(this);
        }
    } else if (e->type() == QEvent::MouseButtonRelease) {
               if (QMainWindow *mw = dynamic_cast<QMainWindow *>(parentWidget())) {
                   Qt::DockWidgetArea newArea = mw->dockWidgetArea(this);
                   if (m_area != newArea) {
                       mw->removeDockWidget(this);
                       mw->addDockWidget(newArea, this);
                       emit topLevelChanged(false);
                   }
               }
    }

    return toReturn;
}

#endif
*/

