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

#include "ktscenetabwidget.h"
#include <QWheelEvent>
#include <QTabBar>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>

#include "kdebug.h"

KTSceneTabWidget::KTSceneTabWidget(QWidget *parent) : QFrame(parent)
{
   QVBoxLayout *layout = new QVBoxLayout(this);
   layout->setMargin(1);

   tabber = new QTabWidget;
   layout->addWidget(tabber);

   setLayout(layout);
}

KTSceneTabWidget::~KTSceneTabWidget()
{
}

QTabWidget* KTSceneTabWidget::TabWidget()
{
   return tabber;
}

void KTSceneTabWidget::removeAllTabs()
{
    int count = tabber->count();
    for (int i = 0; i < count; i++)
         delete tabber->currentWidget();
}

void KTSceneTabWidget::addScene(int index, const QString &name, KTExposureTable *table) {

    QFrame *frame = new QFrame;
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(1);

    QLabel *header = new QLabel();
    QPixmap pix(THEME_DIR + "icons/background_foreground.png");
    header->setToolTip(tr("Layers"));
    header->setPixmap(pix);

    layout->addWidget(header, 0, Qt::AlignHCenter);
    layout->addWidget(table);
    frame->setLayout(layout);

    tables.insert(index, table);
    tabber->insertTab(index, frame, name);
}

KTExposureTable* KTSceneTabWidget::getCurrentTable() 
{
    int index = currentIndex();

    return getTable(index);
}

KTExposureTable* KTSceneTabWidget::getTable(int index)
{
    KTExposureTable *table = tables.value(index);

    if (table)
        return table;

    return 0;
}

int KTSceneTabWidget::currentIndex()
{
    int index = tabber->currentIndex();
    return index;
}

int KTSceneTabWidget::count()
{
    return tabber->count();
}
