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

#include "exactnessconfigurator.h"
#include <QBoxLayout>

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPushButton>

#include "tglobal.h"
#include "kimagebutton.h"
#include "tdebug.h"
#include "tconfig.h"

ExactnessConfigurator::ExactnessConfigurator(QWidget *parent) :QWidget(parent)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom, this);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *label = new QLabel(tr("Smoothness"));
    label->setAlignment(Qt::AlignHCenter); 
    layout->addWidget(label);
    m_exactness = new QDoubleSpinBox();

    //m_exactness->setValue(4.0);
    m_exactness->setDecimals(2);
    m_exactness->setSingleStep(0.1);
    m_exactness->setMaximum(100);
    layout->addWidget(m_exactness);

    mainLayout->addLayout(layout);

    QLabel *previews = new QLabel(tr("My Values:"));
    previews->setAlignment(Qt::AlignHCenter);
    mainLayout->addWidget(previews);

    m_table = new QTableWidget(3, 3);
    connect(m_table, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(updateValueFromItem(QTableWidgetItem *)));

    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->hide();
    m_table->verticalHeader()->hide();

    for (int row = 0; row < m_table->rowCount(); row++) {
         m_table->verticalHeader()->resizeSection(row, 15);

         for (int col = 0; col < m_table->columnCount(); col++) {
              QTableWidgetItem *newItem = new QTableWidgetItem;
              m_table->setItem(row, col, newItem);
         }
    }

    m_table->setItemSelected(m_table->item(0,0), true); 
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_table->setMaximumHeight(22*m_table->rowCount() + 3);
    m_table->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    m_table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    mainLayout->addWidget(m_table);

    QBoxLayout *buttonLayout = new QBoxLayout(QBoxLayout::LeftToRight);

    KImageButton *add = new KImageButton(QIcon(THEME_DIR + "icons/plus_sign.png"),22, 0, false);
    buttonLayout->addWidget(add);

    connect(add, SIGNAL(clicked()), this, SLOT(addCurrentValue()));

    KImageButton *del = new KImageButton(QIcon(THEME_DIR + "icons/minus_sign.png"), 22, 0, false);

    connect(del, SIGNAL(clicked()), this, SLOT(removeCurrentValue()));

    buttonLayout->addWidget(del);

    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(2);

    TCONFIG->beginGroup("BrushTool");
    double smoothness = TCONFIG->value("Smoothness", -1).toDouble();

    if (smoothness > 0) 
        m_exactness->setValue(smoothness);
    else
        m_exactness->setValue(4.0);
}

ExactnessConfigurator::~ExactnessConfigurator()
{
    #ifdef K_DEBUG
           TEND;
    #endif
}

double ExactnessConfigurator::exactness() const
{
    return m_exactness->value();
}

void ExactnessConfigurator::resizeEvent(QResizeEvent *)
{
    int cellWidth = m_table->viewport()->width() / m_table->columnCount();
	
    for (int colIndex = 0; colIndex < m_table->columnCount(); colIndex++)
         m_table->horizontalHeader()->resizeSection(colIndex, cellWidth);
}

void ExactnessConfigurator::addCurrentValue()
{
    double value = m_exactness->value();

    QList<QTableWidgetItem *> selectionList = m_table->selectedItems();

    if (!selectionList.isEmpty())
        selectionList[0]->setText(QString::number(value));
}

void ExactnessConfigurator::removeCurrentValue()
{
    QList<QTableWidgetItem *> selectionList = m_table->selectedItems();

    if (!selectionList.isEmpty())
        selectionList[0]->setText("");
}

void ExactnessConfigurator::updateValueFromItem(QTableWidgetItem *item)
{
    if (item) {
        bool ok = false;
        double value = item->text().toDouble(&ok);

        if (ok)
            m_exactness->setValue(value);
    }
}
