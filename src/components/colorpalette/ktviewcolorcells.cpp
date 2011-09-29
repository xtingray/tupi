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

#include "ktviewcolorcells.h"
#include "tdebug.h"
#include "tglobal.h"
#include "kimagebutton.h"
#include "tconfig.h"

#include <QScrollArea>
#include <QGroupBox>

struct KTViewColorCells::Private
{
    QComboBox *chooserPalette;
    QStackedWidget *containerPalette;
    KTCellsColor *defaultPalette;
    KTCellsColor *qtColorPalette;
    KTCellsColor *customColorPalette;
    KTCellsColor *customGradientPalette;
    int numColorRecent;
    QBrush currentColor;
};

KTViewColorCells::KTViewColorCells(QWidget *parent) : QFrame(parent), k(new Private)
{
    k->numColorRecent = 0;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    setLayout(layout);
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setupForm();
    // setupButtons();
}

KTViewColorCells::~KTViewColorCells()
{
    TCONFIG->beginGroup("ColorPalette");
    TCONFIG->setValue("LastPalette", k->chooserPalette->currentIndex());;

    QDir brushesDir(CONFIG_DIR + "palettes");

    if (! brushesDir.exists()) 
        brushesDir.mkdir(brushesDir.path());

    #ifdef K_DEBUG
           tDebug("palette") << brushesDir.path();
    #endif

    for (int i = 0; i < k->containerPalette->count(); i++) {
         KTCellsColor *palette = qobject_cast<KTCellsColor *>(k->containerPalette->widget(i));
         if (palette) {
             if(!palette->isReadOnly())
                palette->save(CONFIG_DIR + "palettes/" + palette->name() + ".tpal");
         }
    }

    delete k;
	
    #ifdef K_DEBUG
          TEND;
    #endif
}

void KTViewColorCells::setupForm()
{
    k->chooserPalette = new QComboBox(this);

    k->containerPalette = new QStackedWidget(this);
    layout()->addWidget(k->chooserPalette);
    layout()->addWidget(k->containerPalette);

    // Default Palette
    k->defaultPalette = new KTCellsColor(k->containerPalette);
    k->defaultPalette->setName(tr("Default Palette"));
    k->defaultPalette->setReadOnly( true);

    fillDefaultColors();
    addPalette(k->defaultPalette);

    //Named Colors
    k->qtColorPalette = new KTCellsColor(k->containerPalette);
    k->qtColorPalette->setReadOnly(true);
    k->qtColorPalette->setName(tr("Named Colors"));
    addPalette(k->qtColorPalette);

    fillNamedColor();

    //Custom Color Palette
    k->customColorPalette = new KTCellsColor(k->containerPalette);
    k->customColorPalette->setName(tr("Custom Color Palette"));
    addPalette(k->customColorPalette);

    //Custom Gradient Palette
    k->customGradientPalette = new KTCellsColor(k->containerPalette);
    k->customGradientPalette->setName(tr("Custom Gradient Palette"));
    k->customGradientPalette->setType(KTCellsColor::Gradient);
    addPalette(k->customGradientPalette);

    connect(k->chooserPalette, SIGNAL(activated(int )), k->containerPalette, SLOT(setCurrentIndex(int )));

    TCONFIG->beginGroup("ColorPalette");
    int lastIndex = TCONFIG->value("LastPalette").toInt();

    if (lastIndex > 0) {
        k->chooserPalette->setCurrentIndex(lastIndex);
        k->containerPalette->setCurrentIndex(lastIndex);
    }
	
    readPalettes(SHARE_DIR + "data/palettes"); // Pre-installed
    readPalettes(CONFIG_DIR + "palettes"); // Locals
}

void KTViewColorCells::readPalettes(const QString &paletteDir)
{
    #ifdef K_DEBUG
           tDebug("palette") << "Reading palettes from: " << paletteDir;
    #endif
    QDir dir(paletteDir);

    if (dir.exists ()) {
        QStringList files = dir.entryList(QStringList() << "*.tpal");
        QStringList::ConstIterator it = files.begin();

        while (it != files.end()) {
               readPaletteFile(dir.path() + "/" + *it);
               ++it;
        }
    }
}

void KTViewColorCells::readPaletteFile(const QString &file)
{
    KTPaletteParser parser;
    QFile f(file);
    if (parser.parse(&f)) {
        QList<QBrush> brushes = parser.brushes();
        QString name = parser.paletteName();
        bool editable = parser.paletteIsEditable();
        addPalette(name,brushes,editable );
    } else {
        #ifdef K_DEBUG
               tError() << "Error while parse palette file: " << file;
        #endif
    }
}

void KTViewColorCells::addPalette(const QString & name, const QList<QBrush> & brushes, bool editable )
{
    if (name == k->customColorPalette->name()) {
        QList<QBrush>::ConstIterator it = brushes.begin();

        while (it != brushes.end()) {
               k->customColorPalette->addItem( *it);
               ++it;
        }
    } else if (name == k->customGradientPalette->name()) {
               QList<QBrush>::ConstIterator it = brushes.begin();

               while (it != brushes.end()) {
                      k->customGradientPalette->addItem(*it);
                      ++it;
               }
    } else {
           KTCellsColor *palette = new  KTCellsColor(k->containerPalette);
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

void KTViewColorCells::addPalette(KTCellsColor *palette)
{
    connect(palette, SIGNAL(itemEntered(QTableWidgetItem *)), this, SLOT(changeColor(QTableWidgetItem *)));
    connect(palette, SIGNAL(itemPressed(QTableWidgetItem *)), this, SLOT(changeColor(QTableWidgetItem *)));
    k->chooserPalette->addItem(palette->name());
    k->containerPalette->addWidget(palette);
}


void KTViewColorCells::changeColor(QTableWidgetItem* item)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (item)
        emit selectColor(item->background());
}

void KTViewColorCells::fillDefaultColors()
{
    int i;
    int j;
    j = 0;
    //First column, first 6 rows, a gray scale
    for (i = 0; i <= 5; i++)
         k->defaultPalette->addItem(QColor(i * 51, i * 51, i * 51));

    //First column, last 6 rows, basic colors
    k->defaultPalette->addItem(QColor(255, 0, 0));
    k->defaultPalette->addItem(QColor(0, 255, 0));
    k->defaultPalette->addItem(QColor(0, 0, 255));
    k->defaultPalette->addItem(QColor(255, 255, 0));
    k->defaultPalette->addItem(QColor(0, 255, 255));
    k->defaultPalette->addItem(QColor(255, 0, 255));

    //Segment from column 1 to 6 and row 0 to 5
    for (i = 0; i <= 5; i++) {
         for (j = 1; j <= 6; j++)
              k->defaultPalette->addItem(QColor(0, (j - 1) * 51, i * 51));
    }

    //Segment from column 1 to 6 and row 6 to 11
    for (i = 6; i <= 11; i++) {
         for (j = 1; j <= 6; j++)
              k->defaultPalette->addItem(QColor(153, (j - 1) * 51, (i - 6) * 51));
    }

    //Segment from column 7 to 12 and row 0 to 5
    for (i = 0; i <= 5; i++) {
         for (j = 7; j <= 12; j++) 
              k->defaultPalette->addItem(QColor(51, (j - 7) * 51, i * 51));
    }

    //Segment from column 7 to 12 and row 6 to 11
    for (i = 6; i <= 11; i++) {
         for (j = 7; j <= 12; j++)
              k->defaultPalette->addItem(QColor(204, (j - 7) * 51, (i - 6) * 51));
    }

    //Segment from column 13 to 18 and row 0 to 5
    for (i = 0; i <= 5; i++) {
         for (j = 13; j <= 18; j++)
              k->defaultPalette->addItem(QColor(102, (j - 13) * 51, i * 51));
    }

    //Segment from column 13 to 18 and row 6 to 11
    for (i = 6; i <= 11; i++) {
         for (j = 13; j <= 18; j++)
              k->defaultPalette->addItem(QColor(255, (j - 13) * 51, (i - 6) * 51));
    }
}

void KTViewColorCells::fillNamedColor()
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

void KTViewColorCells::addCurrentColor()
{
    KTCellsColor *palette = qobject_cast<KTCellsColor*>(k->containerPalette->currentWidget());

    if (palette) {
        if (palette->isReadOnly() || (k->currentColor.gradient()  && palette->type() == KTCellsColor::Color)
            || (k->currentColor.color().isValid() && palette->type() == KTCellsColor::Gradient)) {
            if (15 <= k->currentColor.style() && k->currentColor.style() < 18) {
                palette = k->customGradientPalette;
                k->chooserPalette->setCurrentIndex( k->chooserPalette->findText ( k->customGradientPalette->name()));
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

void KTViewColorCells::removeCurrentColor()
{
     KCellView *palette = qobject_cast<KCellView *>(k->containerPalette->currentWidget());
     if (palette) {
          if (k->defaultPalette != palette) {
              //TODO: Add function removeItem in KCellView
          }
     }
}

void KTViewColorCells::setupButtons()
{
    QGroupBox *containerButtons = new QGroupBox(this);
    QBoxLayout *bLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    bLayout->setMargin(0);
    bLayout->setSpacing(0);

    containerButtons->setLayout(bLayout);

    KImageButton *addItem = new KImageButton(QPixmap(THEME_DIR  + "icons/plus_sign.png") , 22);
    connect(addItem, SIGNAL(clicked()), SLOT(addCurrentColor()));
    addItem->setToolTip(tr("Add Color"));
    bLayout->addWidget(addItem);
    // SQA instruction - temporary code
    addItem->setEnabled(false);

    KImageButton *removeColor = new KImageButton(QPixmap(THEME_DIR + "icons/minus_sign.png"), 22);
    connect( removeColor, SIGNAL(clicked()), SLOT(removeCurrentColor()));
    removeColor->setToolTip(tr("Remove Color"));
    bLayout->addWidget(removeColor);
    // SQA instruction - temporary code
    removeColor->setEnabled(false);

    layout()->addWidget(containerButtons);
}

void KTViewColorCells::setColor(const QBrush& b)
{
    k->currentColor = b;
}
