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

#include "tupscenetabwidget.h"

struct TupSceneTabWidget::Private
{
    QList<TupExposureTable *> tables;
    QTabWidget *tabber;
    QList<QDoubleSpinBox *> opacityControl;
};

TupSceneTabWidget::TupSceneTabWidget(QWidget *parent) : QFrame(parent), k(new Private)
{
   QVBoxLayout *layout = new QVBoxLayout(this);
   layout->setMargin(1);

   k->tabber = new QTabWidget;
   connect(k->tabber, SIGNAL(currentChanged(int)), this, SIGNAL(currentChanged(int)));
   layout->addWidget(k->tabber);

   setLayout(layout);
}

TupSceneTabWidget::~TupSceneTabWidget()
{
}

void TupSceneTabWidget::removeAllTabs()
{
    int count = k->tabber->count();
    for (int i = 0; i < count; i++)
         delete k->tabber->currentWidget();

    k->tables.clear();
    k->opacityControl.clear();
}

void TupSceneTabWidget::addScene(int index, const QString &name, TupExposureTable *table) 
{
    QFrame *frame = new QFrame;
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setMargin(1);

    QHBoxLayout *opacityLayout = new QHBoxLayout;
    opacityLayout->setAlignment(Qt::AlignHCenter);

    QLabel *header = new QLabel();
    QPixmap pix(THEME_DIR + "icons/layer_opacity.png");
    header->setToolTip(tr("Current Layer Opacity"));
    header->setPixmap(pix);

    QDoubleSpinBox *opacitySpinBox = new QDoubleSpinBox(this);
    opacitySpinBox->setRange(0.1, 1.0);
    opacitySpinBox->setSingleStep(0.1);
    opacitySpinBox->setValue(1.0);
    opacitySpinBox->setToolTip(tr("Current Layer Opacity"));
    connect(opacitySpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(updateLayerOpacity(double)));

    k->opacityControl << opacitySpinBox;

    opacityLayout->addWidget(header);
    opacityLayout->addWidget(opacitySpinBox);

    layout->addLayout(opacityLayout);
    layout->addWidget(table);
    frame->setLayout(layout);

    k->tables.insert(index, table);
    k->tabber->insertTab(index, frame, name);
}

void TupSceneTabWidget::removeScene(int index) 
{
    k->tables.removeAt(index);
    k->opacityControl.removeAt(index);

    blockSignals(true);
    k->tabber->removeTab(index);
    blockSignals(false);
}

void TupSceneTabWidget::renameScene(int index, const QString &name)
{
    k->tabber->setTabText(index, name);
}

TupExposureTable* TupSceneTabWidget::getCurrentTable() 
{
    int index = currentIndex();
    return getTable(index);
}

TupExposureTable* TupSceneTabWidget::getTable(int index)
{
    if (isTableIndexValid(index)) {
        TupExposureTable *table = k->tables.at(index);

        if (table) {
            return table;
        } else {
            #ifdef K_DEBUG
                QString msg = "TupSceneTabWidget::getTable() - [ Fatal Error ] - Table pointer is NULL!";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
        }
    }

    #ifdef K_DEBUG
        QString msg = "TupSceneTabWidget::getTable() - [ Fatal Error ] - Invalid table index: " + QString::number(index);
        #ifdef Q_OS_WIN32
            qDebug() << msg;
        #else
            tError() << msg;
        #endif
    #endif

    return 0;
}

void TupSceneTabWidget::setCurrentIndex(int index)
{
    k->tabber->setCurrentIndex(index);
}

int TupSceneTabWidget::currentIndex()
{
    int index = k->tabber->currentIndex();
    return index;
}

bool TupSceneTabWidget::isTableIndexValid(int index)
{
    if (index > -1 && index < k->tables.count())
        return true;
    return false;
}

int TupSceneTabWidget::count()
{
    return k->tables.count();
}

void TupSceneTabWidget::setLayerOpacity(int sceneIndex, double opacity)
{
    k->opacityControl.at(sceneIndex)->setValue(opacity);
}

void TupSceneTabWidget::setLayerVisibility(int sceneIndex, int layerIndex, bool visibility)
{
    if (isTableIndexValid(sceneIndex)) {
        TupExposureTable *table = k->tables.at(sceneIndex);
        table->setLayerVisibility(layerIndex, visibility);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupSceneTabWidget::setLayerVisibility() - [ Fatal Error ] - Invalid table index: " + QString::number(sceneIndex);
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

