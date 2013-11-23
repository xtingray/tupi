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

#include "tupnewitemdialog.h"
#include "talgorithm.h"
#include "tapplicationproperties.h"
#include "tdebug.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QFile>

struct TupNewItemDialog::Private
{
    QLineEdit *itemName;
    QComboBox *extension;
    QSpinBox *width;
    QSpinBox *height;
    QComboBox *background;
    QComboBox *editor;
    QString name;
    ThirdParty software;
    QString fileExtension;
    QSize size;
    QColor colors[3];
    QColor bg;
};

TupNewItemDialog::TupNewItemDialog(QString &name, DialogType type, QSize size) : QDialog(), k(new Private)
{
    k->name = name;

    k->colors[0] = Qt::transparent;
    k->colors[1] = Qt::white;
    k->colors[2] = Qt::black;

    k->extension = new QComboBox();
    k->editor = new QComboBox();

    if (type == Raster) {
        setWindowTitle(tr("Create new raster item"));
        setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/bitmap.png")));
        k->extension->addItem("PNG");
        k->extension->addItem("JPG");
        k->fileExtension = "PNG"; 

        k->background = new QComboBox();
        k->background->addItem(tr("Transparent"));
        k->background->addItem(tr("White"));
        k->background->addItem(tr("Black"));
        k->bg = Qt::transparent;

        k->software = Gimp;
#ifdef Q_OS_UNIX
        if (QFile::exists("/usr/bin/gimp"))
            k->editor->addItem("Gimp");
        if (QFile::exists("/usr/bin/krita"))
            k->editor->addItem("Krita");
        if (QFile::exists("/usr/bin/mypaint"))
            k->editor->addItem("MyPaint");
#endif
    } else {
        setWindowTitle(tr("Create new vector item"));
        setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/svg.png")));
        k->extension->addItem("SVG");
        k->editor->addItem("Inkscape");
        k->fileExtension = "SVG";
        k->software = Inkscape;
    }

    connect(k->extension, SIGNAL(currentIndexChanged(int)), this, SLOT(updateExtension(int)));
    connect(k->editor, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEditor(int)));

    QFormLayout *formLayout = new QFormLayout;

    k->itemName = new QLineEdit;
    k->itemName->setText(name);

    k->width = new QSpinBox;
    k->width->setMaximum(size.width());
    k->width->setMinimumWidth(60);
    k->width->setValue(50);

    k->height = new QSpinBox;
    k->height->setMaximum(size.height());
    k->height->setMinimumWidth(60);
    k->height->setValue(50);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                | QDialogButtonBox::Cancel, Qt::Horizontal);
    connect(buttons, SIGNAL(accepted ()), this, SLOT(checkValues()));
    connect(buttons, SIGNAL(rejected ()), this, SLOT(reject()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(buttons);

    formLayout->addRow(tr("&Name:"), k->itemName);
    formLayout->addRow(tr("&Extension:"), k->extension);
    formLayout->addRow(tr("&Width:"), k->width);
    formLayout->addRow(tr("&Height:"), k->height);

    if (type == Raster) {
        formLayout->addRow(tr("&Background:"), k->background);
        connect(k->background, SIGNAL(currentIndexChanged(int)), this, SLOT(updateBackground(int)));
    }

    formLayout->addRow(tr("&Open it with:"), k->editor);
    formLayout->addRow(buttonsLayout);

    setLayout(formLayout);
}

TupNewItemDialog::~TupNewItemDialog()
{
    delete k;
}

void TupNewItemDialog::checkValues()
{
    QString name = k->itemName->text();
    if (name.length() == 0) {
        name = TAlgorithm::randomString(8);
        k->itemName->setText(name);
        return;
    }

    bool alert = false;

    if (k->width->value() == 0) {
        k->width->setValue(100);
        alert = true;
    }

    if (k->height->value() == 0) {
        k->height->setValue(100);
        alert = true;
    }

    if (alert)
        return;

    name.replace(" ", "_");
    name.replace(".", "_");
    k->name = name;
    k->size.setWidth(k->width->value());
    k->size.setHeight(k->height->value());

    accept();
}

void TupNewItemDialog::updateExtension(int index)
{
    k->fileExtension = k->extension->itemText(index);

    if (index == 1) {
        if (k->background->itemText(0).compare(tr("Transparent")) == 0)
            k->background->removeItem(0);
    } else {
        if (k->background->count() == 2)
            k->background->insertItem(0, tr("Transparent"));
    }
}

void TupNewItemDialog::updateBackground(int index)
{
    k->bg = k->colors[index];
}

void TupNewItemDialog::updateEditor(int index)
{
    QString software = k->editor->itemText(index);
    if (k->fileExtension.compare("SVG") == 0)
        k->software = Inkscape;
    else
        k->software = TupNewItemDialog::ThirdParty(index);
}

QString TupNewItemDialog::itemName() const
{
    return k->name;
}

QSize TupNewItemDialog::itemSize() const
{
    return k->size;
}

QString TupNewItemDialog::itemExtension() const
{
    return k->fileExtension;
}

QColor TupNewItemDialog::background() const
{
    return k->bg;
}

TupNewItemDialog::ThirdParty TupNewItemDialog::software() const
{
    return k->software;
}
