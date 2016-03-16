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

#include "tupviewcolorcells.h"

struct TupViewColorCells::Private
{
    QComboBox *chooserPalette;
    QStackedWidget *containerPalette;
    TupCellsColor *defaultPalette;
    TupCellsColor *qtColorPalette;
    TupCellsColor *customColorPalette;
    TupCellsColor *customGradientPalette;
    int numColorRecent;
    QBrush currentColor;
    QTableWidgetItem* currentCell;
    QVBoxLayout *layout;
};

TupViewColorCells::TupViewColorCells(QWidget *parent) : QFrame(parent), k(new Private)
{
    k->numColorRecent = 0;
    k->currentCell = 0;
    k->layout = new QVBoxLayout;
    k->layout->setMargin(0);
    k->layout->setSpacing(0);

    setFrameStyle(QFrame::Box | QFrame::Raised);
    setupForm();
    // setupButtons();

    setLayout(k->layout);
}

TupViewColorCells::~TupViewColorCells()
{
    TCONFIG->beginGroup("ColorPalette");
    TCONFIG->setValue("LastPalette", k->chooserPalette->currentIndex());

    QDir brushesDir(CONFIG_DIR + "palettes");

    if (! brushesDir.exists()) 
        brushesDir.mkdir(brushesDir.path());

    #ifdef K_DEBUG
        QString msg = "TupViewColorCells::~TupViewColorCells() - Saving color palettes in: " + brushesDir.path();
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning("palette") << msg;
        #endif
    #endif

    for (int i = 0; i < k->containerPalette->count(); i++) {
         TupCellsColor *palette = qobject_cast<TupCellsColor *>(k->containerPalette->widget(i));
         if (palette) {
             if (!palette->isReadOnly())
                 palette->save(CONFIG_DIR + "palettes/" + palette->name() + ".tpal");
         }
    }

    // delete k;
	
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupViewColorCells()]";
        #else
            TEND;
        #endif
    #endif
}

void TupViewColorCells::setupForm()
{
    k->chooserPalette = new QComboBox(this);
    k->chooserPalette->setStyleSheet("combobox-popup: 0;");

    k->containerPalette = new QStackedWidget(this);
    k->layout->addWidget(k->chooserPalette);
    k->layout->addWidget(k->containerPalette);

    // Default Palette
    k->defaultPalette = new TupCellsColor(k->containerPalette);
    k->defaultPalette->setName(tr("Default Palette"));
    k->defaultPalette->setReadOnly(true);
    // fillDefaultColors();
    addPalette(k->defaultPalette);

    //Named Colors
    k->qtColorPalette = new TupCellsColor(k->containerPalette);
    k->qtColorPalette->setReadOnly(true);
    k->qtColorPalette->setName(tr("Named Colors"));
    fillNamedColor();
    addPalette(k->qtColorPalette);

    //Custom Color Palette
    // SQA: This palette must be implemented
    k->customColorPalette = new TupCellsColor(k->containerPalette);
    k->customColorPalette->setName(tr("Custom Color Palette"));
    addPalette(k->customColorPalette);

    //Custom Gradient Palette
    // SQA: This palette must be implemented
    k->customGradientPalette = new TupCellsColor(k->containerPalette);
    k->customGradientPalette->setName(tr("Custom Gradient Palette"));
    k->customGradientPalette->setType(TupCellsColor::Gradient);
    addPalette(k->customGradientPalette);

#ifdef Q_OS_WIN
    QString palettesPath = SHARE_DIR + "palettes";
#else
    QString palettesPath = SHARE_DIR + "data/palettes";
#endif
    readPalettes(palettesPath); // Pre-installed
    readPalettes(CONFIG_DIR + "palettes"); // Locals

    connect(k->chooserPalette, SIGNAL(activated(int)), k->containerPalette, SLOT(setCurrentIndex(int)));

    TCONFIG->beginGroup("ColorPalette");
    int lastIndex = TCONFIG->value("LastPalette").toInt();

    if (lastIndex < 0)
        lastIndex = 0;

    k->chooserPalette->setCurrentIndex(lastIndex);
    k->containerPalette->setCurrentIndex(lastIndex);
}

void TupViewColorCells::readPalettes(const QString &paletteDir)
{
    #ifdef K_DEBUG
        QString msg = "TupViewColorCells::readPalettes() - Reading palettes from: " + paletteDir;
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning("palette") << msg;
        #endif
    #endif

    QDir dir(paletteDir);

    if (dir.exists()) {
        QStringList files = dir.entryList(QStringList() << "*.tpal");
        QStringList::ConstIterator it = files.begin();

        while (it != files.end()) {
               readPaletteFile(dir.path() + "/" + *it);
               ++it;
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupViewColorCells::readPalettes() - Error: Invalid path -> " + paletteDir;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError("palette") << msg;
            #endif
        #endif	
	}
}

void TupViewColorCells::readPaletteFile(const QString &paletteFile)
{
    TupPaletteParser parser;
    QFile file(paletteFile);
    if (parser.parse(&file)) {
        QList<QBrush> brushes = parser.brushes();
        QString name = parser.paletteName();
        bool editable = parser.paletteIsEditable();
        addPalette(name, brushes, editable);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupViewColorCells::readPaletteFile() - Fatal error while parsing palette file: " + paletteFile;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupViewColorCells::addPalette(const QString & name, const QList<QBrush> & brushes, bool editable)
{
    /*
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupViewColorCells::addPalette()]";
        #else
            T_FUNCINFO;
        #endif
    #endif
    */

    if (name == "Default Palette") {
        QList<QBrush>::ConstIterator it = brushes.begin();

        while (it != brushes.end()) {
               k->defaultPalette->addItem(*it);
               ++it;
        }
    } else if (name == k->customColorPalette->name()) {
        QList<QBrush>::ConstIterator it = brushes.begin();

        while (it != brushes.end()) {
               k->customColorPalette->addItem(*it);
               ++it;
        }
    } else if (name == k->customGradientPalette->name()) {
               QList<QBrush>::ConstIterator it = brushes.begin();

               while (it != brushes.end()) {
                      k->customGradientPalette->addItem(*it);
                      ++it;
               }
    } else {
           TupCellsColor *palette = new  TupCellsColor(k->containerPalette);
           QList<QBrush>::ConstIterator it = brushes.begin();

           while (it != brushes.end()) {
                  palette->addItem(*it);
                  ++it;
           }

           palette->setName(name);
           addPalette(palette);
           palette->setReadOnly(!editable);
    }
}

void TupViewColorCells::addPalette(TupCellsColor *palette)
{
    connect(palette, SIGNAL(itemEntered(QTableWidgetItem *)), this, SLOT(changeColor(QTableWidgetItem *)));
    connect(palette, SIGNAL(itemPressed(QTableWidgetItem *)), this, SLOT(changeColor(QTableWidgetItem *)));
    k->chooserPalette->addItem(palette->name());
    k->containerPalette->addWidget(palette);
}

void TupViewColorCells::changeColor(QTableWidgetItem* item)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupViewColorCells::changeColor()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (item) {
        k->currentCell = item;
        emit selectColor(item->background());
    }
}

void TupViewColorCells::clearSelection()
{
    if (k->currentCell)
        k->currentCell->setSelected(false);
}

void TupViewColorCells::fillNamedColor()
{
    QStringList strColors = QColor::colorNames();
    QStringList::ConstIterator it = strColors.begin();

    while (it != strColors.end()) {
           k->qtColorPalette->addItem(QColor(*it));
           ++it;
    }

    k->qtColorPalette->addItem(QColor(0,0,0,0));
    k->qtColorPalette->addItem(QColor(0,0,0,50));
}

void TupViewColorCells::addCurrentColor()
{
    TupCellsColor *palette = qobject_cast<TupCellsColor*>(k->containerPalette->currentWidget());

    if (palette) {
        if (palette->isReadOnly() || (k->currentColor.gradient()  && palette->type() == TupCellsColor::Color)
            || (k->currentColor.color().isValid() && palette->type() == TupCellsColor::Gradient)) {
            if (15 <= k->currentColor.style() && k->currentColor.style() < 18) {
                palette = k->customGradientPalette;
                k->chooserPalette->setCurrentIndex(k->chooserPalette->findText(k->customGradientPalette->name()));
                k->containerPalette->setCurrentWidget(k->customGradientPalette);
            } else {
                palette = k->customColorPalette;
                k->chooserPalette->setCurrentIndex(k->chooserPalette->findText(k->customColorPalette->name()));
                k->containerPalette->setCurrentWidget(k->customColorPalette);
            }
        }
        palette->addItem(k->currentColor);
    }
}

void TupViewColorCells::removeCurrentColor()
{
     TCellView *palette = qobject_cast<TCellView *>(k->containerPalette->currentWidget());
     if (palette) {
          if (k->defaultPalette != palette) {
              //TODO: Add function removeItem in TCellView
          }
     }
}

void TupViewColorCells::setupButtons()
{
    QGroupBox *containerButtons = new QGroupBox(this);
    QBoxLayout *bLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    bLayout->setMargin(0);
    bLayout->setSpacing(0);

    containerButtons->setLayout(bLayout);

    TImageButton *addItem = new TImageButton(QPixmap(THEME_DIR  + "icons/plus_sign.png") , 22);
    connect(addItem, SIGNAL(clicked()), SLOT(addCurrentColor()));
    addItem->setToolTip(tr("Add Color"));
    bLayout->addWidget(addItem);
    // SQA instruction - temporary code
    addItem->setEnabled(false);

    TImageButton *removeColor = new TImageButton(QPixmap(THEME_DIR + "icons/minus_sign.png"), 22);
    connect( removeColor, SIGNAL(clicked()), SLOT(removeCurrentColor()));
    removeColor->setToolTip(tr("Remove Color"));
    bLayout->addWidget(removeColor);
    // SQA instruction - temporary code
    removeColor->setEnabled(false);

    layout()->addWidget(containerButtons);
}

void TupViewColorCells::setColor(const QBrush& brush)
{
    k->currentColor = brush;
}
