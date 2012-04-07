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

#include "kinaswidget.h"
#include "tdebug.h"

KinasWidget::KinasWidget(QWidget *parent) : TupModuleWidgetBase(parent)
{
    TINIT;

    setWindowTitle(tr("Tupi Script"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/color_palette.png"));

    m_splitter = new QSplitter(this);
    addChild(m_splitter);

    m_functionViewer = new TupSFunctionView(m_splitter);
    m_splitter->addWidget(m_functionViewer);

    m_editors = new QTabWidget(this);
    m_splitter->addWidget(m_editors);

    m_splitter->setSizes(QList<int>()<<160 << 400);

    addEditor("Example.kts");
}


KinasWidget::~KinasWidget()
{
    TEND;
}

void KinasWidget::addEditor(const QString &title)
{
    TupSEditor *m_editor = new TupSEditor;
    m_editors->addTab(m_editor, title);
}
