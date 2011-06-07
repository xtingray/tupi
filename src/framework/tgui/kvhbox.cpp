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

#include "kvhbox.h"
#include "tdebug.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPoint>

KVHBox::KVHBox(QWidget *parent, Qt::Orientation o) : QFrame(parent)
{
    if (o == Qt::Vertical)
        m_pLayout = new QVBoxLayout(this);
    else
        m_pLayout = new QHBoxLayout(this);
    
    m_pLayout->setMargin(1);
    m_pLayout->setSpacing(1);
}

KVHBox::KVHBox(QWidget *parent, bool isVertical) : QFrame(parent)
{
    if (isVertical)
        m_pLayout = new QVBoxLayout(this);
    else 
        m_pLayout = new QHBoxLayout(this);
    
    m_pLayout->setMargin(1);
    m_pLayout->setSpacing(1);
}

KVHBox::~KVHBox()
{
}

void KVHBox::addWidget(QWidget *child, Qt::Alignment alignment)
{
    child->setParent(this);
    m_pLayout->addWidget(child);

    if (alignment != 0)
        m_pLayout->setAlignment(child, alignment);
}

void KVHBox::moveWidgetUp(QWidget *widget)
{
    // dDebug() << "Childs " << children ().count() << endl;
    int position = m_pLayout->indexOf(widget);
    
    // dDebug() << "Position: " << position << endl;
    
    if (position > 0) {
        m_pLayout->removeWidget(widget);
        m_pLayout->insertWidget(position-1, widget);
    } else {
        tError() << "The widget isn't in the layout" << endl;
    }
}

void KVHBox::moveWidgetDown(QWidget *widget)
{
    int position = m_pLayout->indexOf(widget);
    
    if (position >= 0) {
        m_pLayout->removeWidget(widget);
        m_pLayout->insertWidget(position+1, widget);
    }
}

bool KVHBox::event(QEvent* ev)
{
    switch (ev->type()) {
        case QEvent::ChildAdded:
        {
            QChildEvent* childEv = static_cast<QChildEvent *>(ev);

            if (childEv->child()->isWidgetType()) {
                QWidget* w = static_cast<QWidget *>(childEv->child());
                static_cast<QBoxLayout *>(layout())->addWidget(w);
            }

            return QWidget::event(ev);
        }
        case QEvent::ChildRemoved:
        {
            QChildEvent* childEv = static_cast<QChildEvent *>(ev);

            if (childEv->child()->isWidgetType()) {
                QWidget* w = static_cast<QWidget *>(childEv->child());
                static_cast<QBoxLayout *>(layout())->removeWidget(w);
            }

            return QWidget::event(ev);
        }
        default:
            return QWidget::event(ev);
    }
}

QSize KVHBox::sizeHint() const
{
    KVHBox* that = const_cast<KVHBox *>(this);
    QApplication::sendPostedEvents(that, QEvent::ChildAdded);

    return QFrame::sizeHint();
}

void KVHBox::setSpacing(int sp)
{
    layout()->setSpacing(sp);
}

void KVHBox::setStretchFactor(QWidget* w, int stretch)
{
    static_cast<QBoxLayout *>(layout())->setStretchFactor(w, stretch);
}

void KVHBox::switchWidgetsPosition(QWidget *widget1, QWidget *widget2)
{
    int position1 = m_pLayout->indexOf(widget1);
    int position2 = m_pLayout->indexOf(widget2);
    
    if (position1 > 0 && position2 > 0) {
    }
}

void KVHBox::mouseMoveEvent(QMouseEvent *e)
{
    if (hasMouseTracking()) {
        emit mouseAt(e->pos());
    }
}

QBoxLayout *KVHBox::boxLayout()
{
    return m_pLayout;
}

