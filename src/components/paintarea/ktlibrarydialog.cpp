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

#include "ktlibrarydialog.h"
#include "ktitempreview.h"
#include "tformfactory.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QToolBox>
#include <QGraphicsItem>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMap>

struct KTLibraryDialog::Private
{
    QToolBox *toolBox;
    QMap<QGraphicsItem *, QLineEdit *> symbolNames;
    QMap<int, QLineEdit *> tabs;
};

KTLibraryDialog::KTLibraryDialog() : QDialog(), k(new Private)
{
    setWindowTitle(tr("Library Object"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    k->toolBox = new QToolBox;
    layout->addWidget(k->toolBox);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok 
                                | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttons, SIGNAL(accepted ()), this, SLOT(checkNames()));
    connect(buttons, SIGNAL(rejected ()), this, SLOT(reject()));

    layout->addWidget(buttons, 0, Qt::AlignCenter);
}

KTLibraryDialog::~KTLibraryDialog()
{
    delete k;
}

void KTLibraryDialog::addItem(QGraphicsItem *item)
{
    KTItemPreview *preview = new KTItemPreview;    
    preview->render(item);

    QWidget *container = new QWidget;

    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->addWidget(preview);

    QLineEdit *name = new QLineEdit;
    connect(name, SIGNAL(returnPressed()), this, SLOT(checkNames()));

    QLayout *grid = TFormFactory::makeGrid(QStringList() << tr("Name"), QWidgetList() << name);
    layout->addLayout(grid);

    int index = k->toolBox->addItem(container, tr("Item %1").arg(k->toolBox->count()+1));
    k->symbolNames.insert(item, name);
    k->tabs.insert(index, name);
}

QString KTLibraryDialog::symbolName(QGraphicsItem *item) const
{
    return k->symbolNames[item]->text();
}

void KTLibraryDialog::checkNames()
{
    for (int i = 0; i < k->toolBox->count(); i++) {
         if (k->tabs[i]->text().isEmpty()) {
             k->toolBox->setCurrentIndex (i);
             k->tabs[i]->setFocus();
             return;
         }
    }

    accept();
}
