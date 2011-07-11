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

#include "ktcolorpalette.h"
#include "tdebug.h"
#include "kimagebutton.h"
#include "tglobal.h"
#include "tconfig.h"
#include "ktpaintareaevent.h"
#include "ktcolorvalue.h"
#include "ktviewcolorcells.h"
#include "ktcolorpicker.h"
#include "ktluminancepicker.h"
#include "ktgradientcreator.h"

#include "ttoolbox.h"
#include "kvhbox.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QToolTip>
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>
#include <QMenu>

struct KTColorPalette::Private
{
    TToolBox *centralWidget;
    KTViewColorCells *containerPalette;
    KTColorValue *displayColorValue;
    KTColorPicker *colorPicker;
    KTLuminancePicker *luminancePicker;
    KTGradientCreator *gradientManager;
    QLineEdit *nameColor;
    KDualColorButton *outlineAndFillColors;
    QBrush currentOutlineColor;
    QBrush currentFillColor;
    QComboBox *labelType;
    QSplitter *splitter;
    bool flagGradient;
    TypeBrush type;

    //~Private();
};

KTColorPalette::KTColorPalette(QWidget *parent) : KTModuleWidgetBase(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->currentOutlineColor = Qt::black;
    k->currentFillColor = Qt::transparent;
    k->flagGradient = true;
    k->type = Solid;

    setWindowTitle(tr("Color Palette"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/color_palette.png"));

    k->splitter = new QSplitter(Qt::Vertical, this);
    addChild(k->splitter);

    k->centralWidget = new TToolBox(k->splitter);

    setupChooserTypeColor();

    // SQA: Temporarily unavailable
    // setupGradientManager();

    setupDisplayColor();
    k->splitter->addWidget(k->centralWidget);
    k->centralWidget->setPalette(palette());

    TCONFIG->beginGroup("ColorPalette");
    QColor foreground = QColor(TCONFIG->value("LastForegroundColor", Qt::black).toString());
    QColor background = QColor(TCONFIG->value("LastBackgroundColor", Qt::transparent).toString());
}

KTColorPalette::~KTColorPalette()
{
    #ifdef K_DEBUG
           TEND;
    #endif	

    TCONFIG->beginGroup("ColorPalette");
    TCONFIG->setValue("LastForegroundColor", color().first);
    TCONFIG->setValue("LastBackgroundColor", color().second);
    delete k;
}

void KTColorPalette::setupChooserTypeColor()
{
    QFrame *colorMixer = new QFrame(k->centralWidget);
    colorMixer->setFrameStyle(QFrame::Box | QFrame::Sunken);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);

    colorMixer->setLayout(layout);

    k->displayColorValue = new KTColorValue(colorMixer);

    QBoxLayout *layoutContainer = new QBoxLayout(QBoxLayout::LeftToRight);
    layoutContainer->addStretch(2);

    k->colorPicker = new KTColorPicker(colorMixer);
    connect(k->colorPicker, SIGNAL(newCol(int, int)), this, SLOT(setHS(int, int)));
    connect(k->displayColorValue, SIGNAL(hueChanged(int)), k->colorPicker, SLOT(setH(int)));
    connect(k->displayColorValue, SIGNAL(saturationChanged(int)), k->colorPicker, SLOT(setS(int)));

    layoutContainer->addWidget(k->colorPicker, 0, Qt::AlignLeft);

    k->luminancePicker = new KTLuminancePicker(colorMixer);
    connect(k->luminancePicker, SIGNAL(newHsv(int, int, int)), this, SLOT(syncHsv(int, int, int)));

    k->luminancePicker->setMaximumWidth(15);
    k->luminancePicker->setMinimumWidth(15);
    connect(k->displayColorValue, SIGNAL(valueChanged(int)), k->luminancePicker, SLOT(setVal(int)));

    layoutContainer->addWidget(k->luminancePicker, 0, Qt::AlignLeft);
    layoutContainer->setSpacing(3);
    layoutContainer->addStretch(2);

    layout->addLayout(layoutContainer);
    layout->addWidget(k->displayColorValue);

    k->displayColorValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(k->displayColorValue, SIGNAL(brushChanged(const QBrush&)), this, SLOT(setColor(const QBrush &)));

    k->centralWidget->addPage(colorMixer, tr("Color Mixer"));
}

void KTColorPalette::setupGradientManager()
{
    k->gradientManager = new KTGradientCreator(this);
    connect(k->gradientManager, SIGNAL(gradientChanged(const QBrush& )), this, SLOT(setColor(const QBrush &)));
    k->centralWidget->addPage(k->gradientManager,tr("Gradients"));
}

void KTColorPalette::setupDisplayColor()
{
    //palettes
    k->containerPalette = new KTViewColorCells(k->splitter);

    connect (k->containerPalette, SIGNAL(selectColor(const QBrush&)), this, SLOT(setColor(const QBrush&)));

    k->splitter->addWidget(k->containerPalette);

    //////////
    QFrame *viewColor = new QFrame(this);
    QBoxLayout *vlayout = new QBoxLayout(QBoxLayout::LeftToRight);
    vlayout->setMargin(0);

    viewColor->setLayout(vlayout);

    k->labelType = new QComboBox( viewColor);
    k->labelType->addItem(tr("Solid"));
    k->labelType->addItem(tr("Gradient"));

    connect(k->labelType, SIGNAL(activated(const QString &)), this, SLOT(changeBrushType(const QString &)));

    vlayout->addWidget(k->labelType);

    k->outlineAndFillColors = new KDualColorButton(k->currentOutlineColor, k->currentFillColor, viewColor);
    k->outlineAndFillColors->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(k->outlineAndFillColors,  SIGNAL(currentChanged(KDualColorButton::DualColor)),this, SLOT(changeTypeColor(KDualColorButton::DualColor)));

    connect(k->outlineAndFillColors, SIGNAL(fgChanged(const QBrush &)),this, SLOT(setFG(const QBrush &)));
    connect(k->outlineAndFillColors, SIGNAL(bgChanged(const QBrush &)),this, SLOT(setBG(const QBrush &)));

    vlayout->addWidget(k->outlineAndFillColors);

    QBoxLayout *layoutName = new  QBoxLayout(QBoxLayout::TopToBottom);
    layoutName->setMargin(0);

    layoutName->addWidget(new QLabel( "<b>HTML</b>", viewColor));
    k->nameColor = new QLineEdit(viewColor);
    QFontMetrics fm(font());
    k->nameColor->setMaximumWidth(fm.width("#000000"));
	
    connect(k->nameColor, SIGNAL(editingFinished()), this, SLOT(updateColor()));
    layoutName->addWidget(k->nameColor);
    vlayout->addLayout(layoutName);

    addChild(viewColor);
}

void KTColorPalette::setColor(const QBrush& brush)
{
    QColor color = brush.color();

    if (color.isValid()) {
        if (k->type == Gradient)
            k->gradientManager->setCurrentColor(color);

        if (k->displayColorValue && k->outlineAndFillColors && k->colorPicker && k->nameColor && k->luminancePicker) {

            k->colorPicker->setCol(color.hue(), color.saturation());
            if (k->type == Solid)
                k->outlineAndFillColors->setCurrentColor(color);

            k->nameColor->setText(color.name());
            k->luminancePicker->setCol(color.hue(), color.saturation(), color.value());
            k->containerPalette->setColor(brush);
            k->displayColorValue->setColor(color);

        }
    } else if (brush.gradient()) {
               QGradient gradient(*brush.gradient());
               changeBrushType(tr("Gradient"));

               k->containerPalette->setColor(gradient);
               k->outlineAndFillColors->setCurrentColor(gradient);
               if (sender() != k->gradientManager)
                   k->gradientManager->setGradient(gradient);
    }

    if (k->outlineAndFillColors->background().color() != Qt::transparent) {
        KTPaintAreaEvent event(KTPaintAreaEvent::ChangeColorPen, k->outlineAndFillColors->background().color());
        emit paintAreaEventTriggered(&event);
    }

    if (k->outlineAndFillColors->foreground().color() != Qt::transparent) {
        KTPaintAreaEvent event2(KTPaintAreaEvent::ChangeColorPen, k->outlineAndFillColors->foreground().color());
        emit paintAreaEventTriggered(&event2);
    }
}

void KTColorPalette::setFG(const QBrush &brush)
{
    if (brush.color().isValid())
        changeBrushType(tr("Solid"));
    else
        changeBrushType(tr("Gradient"));

    k->outlineAndFillColors->setCurrent(KDualColorButton::Foreground);
    setColor(brush);
}

void KTColorPalette::setBG(const QBrush &brush)
{
    if (brush.color().isValid())
        changeBrushType(tr("Solid"));
    else
        changeBrushType(tr("Gradient"));

    k->outlineAndFillColors->setCurrent( KDualColorButton::Background);
    setColor(brush);
}

void KTColorPalette::changeTypeColor(KDualColorButton::DualColor s)
{
    if (s == KDualColorButton::Background) {
        k->outlineAndFillColors->setCurrent(s);
        setColor(k->outlineAndFillColors->background());
    } else {
        k->outlineAndFillColors->setCurrent(s);
        k->flagGradient = false;
        setColor(k->outlineAndFillColors->foreground());
        k->flagGradient = true;
    }
}

void KTColorPalette::syncHsv(int h, int s, int v)
{
    QColor tmpColor = k->outlineAndFillColors->currentColor().color();
    tmpColor.setHsv( h, s, v, tmpColor.alpha() );
    if (k->luminancePicker == sender())
        setColor(tmpColor);
}

void KTColorPalette::setHS(int h , int s)
{
    QColor tmpColor = k->outlineAndFillColors->currentColor().color();
    tmpColor.setHsv( h, s, k->luminancePicker->value(), tmpColor.alpha() );
    setColor(QBrush(tmpColor));
}

void KTColorPalette::updateColor()
{
    QColor tmpColor(k->nameColor->text());
    tmpColor.setAlpha(k->displayColorValue->alpha());
    setColor(tmpColor);
}

QPair<QColor, QColor> KTColorPalette::color()
{
    QPair<QColor, QColor> colors;
    colors.first = k->outlineAndFillColors->foreground().color();
    colors.second = k->outlineAndFillColors->background().color();

    return colors;
}

void KTColorPalette::parsePaletteFile(const QString &file)
{
    k->containerPalette->readPaletteFile(file);
}

void KTColorPalette::mousePressEvent(QMouseEvent * e)
{
    if (e->button () == Qt::RightButton) {
        QMenu *menu = new QMenu(tr("type brush"), this);
        menu->exec(e->globalPos ());
        delete menu;
    }
}

void KTColorPalette::changeBrushType(const QString& type)
{
    if (type == tr("Solid")) {
        k->type = Solid;
    } else if(type == tr("Gradient")) {
        k->type = Gradient;
    }

    if (type != k->labelType->currentText ()) {
        int index =  k->labelType->findText(type);
        if (index >= 0)
            k->labelType->setCurrentIndex(index);
    }
}
