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

#include "kelabel.h"

#include <QMouseEvent>

KELabel::KELabel(const QString &text, QWidget *parent) : KSqueezeLabel(text, parent)
{
    m_editor = new QLineEdit(text, this);
    m_editor->setFont(QFont(font().family(), 9));
    m_editor->hide();
    
    m_editor->setFocusPolicy(Qt::ClickFocus);
    
    connect(m_editor, SIGNAL(returnPressed()), SLOT(applyText()));
    connect(m_editor, SIGNAL(lostFocus()), SLOT(applyText()));
}

KELabel::~KELabel()
{
}

void KELabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        edit();
        event->accept();
    } else {
        event->ignore();
    }
}

void KELabel::applyText()
{
    if (completeText() != m_editor->text()) {
        setText(m_editor -> text());
        emit edited(m_editor->text());
    }
    
    m_editor->hide();
}

void KELabel::edit()
{
    m_editor->setText(completeText());
    m_editor->selectAll();
    m_editor->resize(size());
    m_editor->show();
    m_editor->setFocus();
}

void KELabel::clearFocus()
{
    m_editor->clearFocus();
}

void KELabel::setValidator(const QValidator * v)
{
    m_editor->setValidator(v);
}
