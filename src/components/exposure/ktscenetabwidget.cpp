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
#include "tdebug.h"

#include <QWheelEvent>
#include <QTabBar>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>

struct KTSceneTabWidget::Private
{
    QList<KTExposureTable *> tables;
    QTabWidget *tabber;
};

KTSceneTabWidget::KTSceneTabWidget(QWidget *parent) : QFrame(parent), k(new Private)
{
   QVBoxLayout *layout = new QVBoxLayout(this);
   layout->setMargin(1);

   k->tabber = new QTabWidget;
   connect(k->tabber, SIGNAL(currentChanged(int)), this, SIGNAL(currentChanged(int)));
   layout->addWidget(k->tabber);

   setLayout(layout);
}

KTSceneTabWidget::~KTSceneTabWidget()
{
}

void KTSceneTabWidget::removeAllTabs()
{
    int count = k->tabber->count();
    for (int i = 0; i < count; i++)
         delete k->tabber->currentWidget();

    k->tables.clear();
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

    k->tables.insert(index, table);
    k->tabber->insertTab(index, frame, name);
}

void KTSceneTabWidget::removeScene(int index) 
{
    // k->tables.remove(index);
    k->tables.removeAt(index);

    blockSignals(true);
    k->tabber->removeTab(index);
    blockSignals(false);

    tError() << "KTSceneTabWidget::removeScene() - Removing scene at index: " << index;
    tError() << "KTSceneTabWidget::removeScene() - Scenes count: " << k->tables.count();
}

void KTSceneTabWidget::renameScene(int index, const QString &name)
{
    k->tabber->setTabText(index, name);
}

KTExposureTable* KTSceneTabWidget::getCurrentTable() 
{
    int index = currentIndex();

    tError() << "KTSceneTabWidget::getCurrentTable() - Getting table at index: " << index;

    return getTable(index);
}

KTExposureTable* KTSceneTabWidget::getTable(int index)
{
    // KTExposureTable *table = k->tables.value(index);
    KTExposureTable *table = k->tables.at(index);

    if (table) {
        return table;
    } else {
        #ifdef K_DEBUG
               tError() << "KTSceneTabWidget::getTable() - [ Fatal Error ] - Invalid table index: " << index;
        #endif
    }

    return 0;
}

void KTSceneTabWidget::setCurrentIndex(int index)
{
    k->tabber->setCurrentIndex(index);
}

int KTSceneTabWidget::currentIndex()
{
    int index = k->tabber->currentIndex();
    return index;
}

int KTSceneTabWidget::count()
{
    // return k->tabber->count();
    return k->tables.count();
}
