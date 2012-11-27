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

#include "tupcolorpalette.h"
#include "tdebug.h"
#include "timagebutton.h"
#include "tglobal.h"
#include "tconfig.h"
#include "tuppaintareaevent.h"
#include "tupcolorvalue.h"
#include "tupviewcolorcells.h"
#include "tupcolorpicker.h"
#include "tupluminancepicker.h"
#include "tupgradientcreator.h"
#include "tvhbox.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QToolTip>
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>
#include <QMenu>
#include <QTabWidget>

struct TupColorPalette::Private
{
    QSplitter *splitter;
    QTabWidget *tab;

    TupViewColorCells *containerPalette;
    TupColorValue *displayColorForms;
    TupColorPicker *colorPickerArea;
    TupLuminancePicker *luminancePicker;
    TupGradientCreator *gradientManager;

    QComboBox *labelType;
    QLineEdit *htmlNameColor;
    TDualColorButton *outlineAndFillColors;
    QBrush currentOutlineColor;
    QBrush currentFillColor;

    bool flagGradient;
    BrushType type;
    TDualColorButton::ColorSpace currentSpace;
    TupColorPalette::BrushType fgType;
    TupColorPalette::BrushType bgType; 
};

TupColorPalette::TupColorPalette(QWidget *parent) : TupModuleWidgetBase(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->currentOutlineColor = Qt::black;
    k->currentFillColor = Qt::transparent;
    k->flagGradient = true;
    // k->type = Solid;

    setWindowTitle(tr("Color Palette"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/color_palette.png"));

    k->splitter = new QSplitter(Qt::Vertical, this);
    k->tab = new QTabWidget;
    connect(k->tab, SIGNAL(currentChanged(int)), this, SLOT(updateColorType(int)));

    setupDisplayColor();

    addChild(k->splitter);

    setupMainPalette();
    setupChooserTypeColor();
    setupGradientManager();

    k->tab->setPalette(palette());
    k->splitter->addWidget(k->tab);

    TCONFIG->beginGroup("ColorPalette");
    QColor foreground = QColor(TCONFIG->value("LastForegroundColor", Qt::black).toString());
    QColor background = QColor(TCONFIG->value("LastBackgroundColor", Qt::transparent).toString());
}

TupColorPalette::~TupColorPalette()
{
    #ifdef K_DEBUG
           TEND;
    #endif	

    TCONFIG->beginGroup("ColorPalette");
    TCONFIG->setValue("LastForegroundColor", color().first);
    TCONFIG->setValue("LastBackgroundColor", color().second);
    delete k;
}

void TupColorPalette::setupDisplayColor()
{
    //////////
    QFrame *viewColor = new QFrame(this);
    QBoxLayout *vlayout = new QBoxLayout(QBoxLayout::LeftToRight);
    vlayout->setMargin(0);

    viewColor->setLayout(vlayout);

    k->labelType = new QComboBox(viewColor);

    k->labelType->addItem(setComboColor(Qt::black), tr("Contour"));
    k->labelType->addItem(setComboColor(Qt::transparent), tr("Fill"));

    connect(k->labelType, SIGNAL(activated(int)), this, SLOT(setColorSpace(int)));

    vlayout->addWidget(k->labelType);

    // foreground/background color buttons
    k->outlineAndFillColors = new TDualColorButton(k->currentOutlineColor, k->currentFillColor, viewColor);
    k->outlineAndFillColors->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(k->outlineAndFillColors, SIGNAL(selectionChanged(TDualColorButton::ColorSpace)),
            this, SLOT(updateColorSpace(TDualColorButton::ColorSpace)));

    connect(k->outlineAndFillColors, SIGNAL(switchColors()), this, SLOT(switchColors()));
    connect(k->outlineAndFillColors, SIGNAL(resetColors()), this, SLOT(resetColors()));

    vlayout->addWidget(k->outlineAndFillColors);

    QBoxLayout *layoutName = new QBoxLayout(QBoxLayout::TopToBottom);
    layoutName->setMargin(0);

    layoutName->addWidget(new QLabel("<b>HTML</b>", viewColor));
    k->htmlNameColor = new QLineEdit(viewColor);
    k->htmlNameColor->setMaximumWidth(70);
    k->htmlNameColor->setMaxLength(7);
    k->htmlNameColor->setText("#000000");

    connect(k->htmlNameColor, SIGNAL(editingFinished()), this, SLOT(updateColor()));
    layoutName->addWidget(k->htmlNameColor);
    vlayout->addLayout(layoutName);

    addChild(viewColor);
}

void TupColorPalette::setupMainPalette()
{
    //palettes
    k->containerPalette = new TupViewColorCells(k->splitter);
    connect(k->containerPalette, SIGNAL(selectColor(const QBrush&)), this, SLOT(updateColorFromPalette(const QBrush&)));

    k->splitter->addWidget(k->containerPalette);
}

void TupColorPalette::setupChooserTypeColor()
{
    QFrame *colorMixer = new QFrame;
    colorMixer->setFrameStyle(QFrame::Box | QFrame::Sunken);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);

    colorMixer->setLayout(layout);

    QBoxLayout *layoutContainer = new QBoxLayout(QBoxLayout::LeftToRight);
    layoutContainer->addStretch(2);

    k->colorPickerArea = new TupColorPicker(colorMixer);
    connect(k->colorPickerArea, SIGNAL(newColor(int, int)), this, SLOT(setHS(int, int)));

    k->displayColorForms = new TupColorValue(colorMixer);
    k->displayColorForms->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(k->displayColorForms, SIGNAL(hueChanged(int)), k->colorPickerArea, SLOT(setHUE(int)));
    connect(k->displayColorForms, SIGNAL(saturationChanged(int)), k->colorPickerArea, SLOT(setSaturation(int)));
    connect(k->displayColorForms, SIGNAL(brushChanged(const QBrush&)), this, SLOT(updateColorFromDisplay(const QBrush&)));

    layoutContainer->addWidget(k->colorPickerArea, 0, Qt::AlignLeft);

    k->luminancePicker = new TupLuminancePicker(colorMixer);
    connect(k->luminancePicker, SIGNAL(newHsv(int, int, int)), this, SLOT(syncHsv(int, int, int)));

    k->luminancePicker->setMaximumWidth(15);
    k->luminancePicker->setMinimumWidth(15);

    layoutContainer->addWidget(k->luminancePicker, 0, Qt::AlignLeft);
    layoutContainer->setSpacing(3);
    layoutContainer->addStretch(2);

    layout->addLayout(layoutContainer);
    layout->addWidget(k->displayColorForms);

    k->tab->addTab(colorMixer, tr("Color Mixer"));
}

void TupColorPalette::setupGradientManager()
{
    k->gradientManager = new TupGradientCreator(this);
    // connect(k->gradientManager, SIGNAL(gradientChanged(const QBrush&)), this, SLOT(updateGradientColor(const QBrush &)));

    k->tab->addTab(k->gradientManager, tr("Gradients"));
    // SQA: Temporary code
    k->tab->setTabEnabled(1, false);
}

void TupColorPalette::setColor(const QBrush& brush)
{
    QColor color = brush.color();

    if (color.isValid()) {
        if (k->type == Gradient)
            k->gradientManager->setCurrentColor(color);

        if (k->displayColorForms && k->outlineAndFillColors && k->colorPickerArea && k->htmlNameColor && k->luminancePicker) {

            k->colorPickerArea->setColor(color.hue(), color.saturation());
            if (k->type == Solid)
                k->outlineAndFillColors->setCurrentColor(color);

            k->htmlNameColor->setText(color.name());
            k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
            k->containerPalette->setColor(brush);
            k->displayColorForms->setColor(color);
        }
    } else if (brush.gradient()) {
               QGradient gradient(*brush.gradient());
               // changeBrushType(tr("Gradient"));

               k->containerPalette->setColor(gradient);
               k->outlineAndFillColors->setCurrentColor(gradient);
               if (sender() != k->gradientManager)
                   k->gradientManager->setGradient(gradient);

               /* SQA: Gradient issue pending for revision
               tFatal() << "TupColorPalette::setColor() - Sending gradient value!";
               TupPaintAreaEvent event(TupPaintAreaEvent::ChangeBrush, brush);
               emit paintAreaEventTriggered(&event);
               return;
               */
    }

    tFatal() << "TupColorPalette::setColor() - Updating background color!";
    // TupPaintAreaEvent event(TupPaintAreaEvent::ChangeBrush, k->outlineAndFillColors->background().color());
    TupPaintAreaEvent event(TupPaintAreaEvent::ChangeBrush, brush);
    emit paintAreaEventTriggered(&event);

    tFatal() << "TupColorPalette::setColor() - Updating foreground color!";
    TupPaintAreaEvent event2(TupPaintAreaEvent::ChangeColorPen, k->outlineAndFillColors->foreground().color());
    emit paintAreaEventTriggered(&event2);
}

void TupColorPalette::setGlobalColors(const QBrush &brush)
{
    if (k->currentSpace == TDualColorButton::Foreground) {
        k->outlineAndFillColors->setForeground(brush);
        k->labelType->setItemIcon(TDualColorButton::Foreground, setComboColor(brush.color()));
        k->currentOutlineColor = brush;

        TupPaintAreaEvent event(TupPaintAreaEvent::ChangeColorPen, brush.color());
        emit paintAreaEventTriggered(&event);
    } else {
        k->outlineAndFillColors->setBackground(brush);
        k->labelType->setItemIcon(TDualColorButton::Background, setComboColor(brush.color()));
        k->currentFillColor = brush;

        TupPaintAreaEvent event(TupPaintAreaEvent::ChangeBrush, brush);
        emit paintAreaEventTriggered(&event);
    }

    k->htmlNameColor->setText(brush.color().name());
}

void TupColorPalette::updateColorFromPalette(const QBrush &brush)
{
    setGlobalColors(brush);
    QColor color = brush.color();
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
    k->displayColorForms->setColor(color);
    k->gradientManager->setCurrentColor(color);
}

void TupColorPalette::updateColorFromDisplay(const QBrush &brush)
{
    tFatal() << "TupColorPalette::updateColorFromDisplay() - Just tracing color: " << brush.color().name();
    setGlobalColors(brush);

    QColor color = brush.color();
    k->colorPickerArea->setColor(color.hue(), color.saturation());
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
}

void TupColorPalette::updateColorSpace(TDualColorButton::ColorSpace space)
{
    k->currentSpace = space;

    QColor color;
    if (k->currentSpace == TDualColorButton::Foreground)
        color = k->outlineAndFillColors->foreground().color();
        // color = k->currentOutlineColor.color().name();
    else
        color = k->outlineAndFillColors->background().color();

    k->labelType->setCurrentIndex(k->currentSpace);

    tFatal() << "TupColorPalette::updateColorSpace() - Picking button #" << space;
    tFatal() << "TupColorPalette::updateColorSpace() - Color: " << color.name();
    tFatal() << "TupColorPalette::updateColorSpace() - Alpha: " << color.alpha();

    k->htmlNameColor->setText(color.name());
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
    k->displayColorForms->setColor(color);
}

void TupColorPalette::updateGradientColor(const QBrush &brush)
{
    tFatal() << "TupColorPalette::updateGradientColor() - Just tracing!";
    setGlobalColors(brush);
}

void TupColorPalette::syncHsv(int h, int s, int v)
{
    QColor color = k->outlineAndFillColors->currentColor().color();
    color.setHsv(h, s, v, color.alpha());

    setGlobalColors(QBrush(color));
    k->displayColorForms->setColor(color);
    // k->colorPickerArea->setColor(color.hue(), color.saturation());
}

void TupColorPalette::setHS(int hue, int saturation)
{
    tFatal() << "TupColorPalette::setHS() - H: " << hue;
    tFatal() << "TupColorPalette::setHS() - S: " << saturation;

    int luminance = 255;
    if (hue == 0 && saturation == 0)
        luminance = 0;

    QColor color;
    color.setHsv(hue, saturation, luminance, 255);
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
    k->displayColorForms->setColor(color);

    tFatal() << "TupColorPalette::setHS() - Color: " << color.name();
    tDebug() << "";

    setGlobalColors(QBrush(color));
}

void TupColorPalette::updateColor()
{
    QColor tmpColor(k->htmlNameColor->text());
    tmpColor.setAlpha(k->displayColorForms->alpha());
    setColor(tmpColor);
}

QPair<QColor, QColor> TupColorPalette::color()
{
    QPair<QColor, QColor> colors;
    colors.first = k->outlineAndFillColors->foreground().color();
    colors.second = k->outlineAndFillColors->background().color();

    return colors;
}

void TupColorPalette::parsePaletteFile(const QString &file)
{
    k->containerPalette->readPaletteFile(file);
}

void TupColorPalette::setColorSpace(int type)
{
    k->currentSpace = TDualColorButton::ColorSpace(type);
    k->outlineAndFillColors->setCurrent(k->currentSpace);

    if (k->currentSpace == TDualColorButton::Foreground) {
        if (k->fgType == Solid && k->tab->currentIndex() != 0) {
            k->tab->setCurrentIndex(0);
        } else if (k->fgType == Gradient && k->tab->currentIndex() != 1) {
                   k->tab->setCurrentIndex(1);
        }
    } else {
        if (k->bgType == Solid && k->tab->currentIndex() != 0) {
            k->tab->setCurrentIndex(0);
        } else if (k->bgType == Gradient && k->tab->currentIndex() != 1) {
                   k->tab->setCurrentIndex(1);
        }
    }   

    /*
    if (type == tr("Solid")) {
        k->type = Solid;
        if (k->tab->currentIndex() != Solid)
            k->tab->setCurrentIndex(Solid);
    } else if (type == tr("Gradient")) {
               k->type = Gradient;
               if (k->tab->currentIndex() != Gradient)
                   k->tab->setCurrentIndex(Gradient);
    }

    // This code has been deprecated!
    if (type != k->labelType->currentText()) {
        int index = k->labelType->findText(type);
        if (index >= 0)
            k->labelType->setCurrentIndex(index);
    }
    */
}

void TupColorPalette::init()
{
    k->currentSpace = TDualColorButton::Foreground;
    QBrush brush = QBrush(Qt::transparent);

    QColor color = Qt::black;
    k->currentOutlineColor = QBrush(color);
    k->currentFillColor = brush;

    k->outlineAndFillColors->setForeground(QBrush(color));
    k->outlineAndFillColors->setBackground(brush);

    k->labelType->setItemIcon(TDualColorButton::Foreground, setComboColor(Qt::black));
    k->labelType->setItemIcon(TDualColorButton::Background, setComboColor(Qt::transparent));

    k->colorPickerArea->setColor(color.hue(), color.saturation());
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
    k->displayColorForms->setColor(color);
    k->gradientManager->setCurrentColor(Qt::white);

    TupPaintAreaEvent event(TupPaintAreaEvent::ChangeColorPen, Qt::black);
    emit paintAreaEventTriggered(&event);

    event = TupPaintAreaEvent(TupPaintAreaEvent::ChangeBrush, brush);
    emit paintAreaEventTriggered(&event);
}

void TupColorPalette::switchColors()
{
     QBrush tmp = k->currentOutlineColor;
     k->currentOutlineColor = k->currentFillColor;
     k->currentFillColor = tmp;

     TupPaintAreaEvent event(TupPaintAreaEvent::ChangeColorPen, k->currentOutlineColor.color());
     emit paintAreaEventTriggered(&event);

     event = TupPaintAreaEvent(TupPaintAreaEvent::ChangeBrush, k->currentFillColor);
     emit paintAreaEventTriggered(&event);

     k->labelType->setItemIcon(TDualColorButton::Foreground, setComboColor(k->currentOutlineColor.color()));
     k->labelType->setItemIcon(TDualColorButton::Background, setComboColor(k->currentFillColor.color()));
}

void TupColorPalette::resetColors()
{
     QColor color = Qt::black;
     k->currentOutlineColor = QBrush(color);
     k->currentFillColor = QBrush(Qt::transparent);

     TupPaintAreaEvent event(TupPaintAreaEvent::ChangeColorPen, k->currentOutlineColor.color());
     emit paintAreaEventTriggered(&event);

     event = TupPaintAreaEvent(TupPaintAreaEvent::ChangeBrush, k->currentFillColor);
     emit paintAreaEventTriggered(&event);

     k->labelType->setItemIcon(TDualColorButton::Foreground, setComboColor(k->currentOutlineColor.color()));
     k->labelType->setItemIcon(TDualColorButton::Background, setComboColor(k->currentFillColor.color()));

     k->colorPickerArea->setColor(color.hue(), color.saturation());
     k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
     k->displayColorForms->setColor(color);
    // k->gradientManager->setCurrentColor(Qt::white);
}

/*
void TupColorPalette::setFG(const QBrush &brush)
{
    if (brush.color().isValid())
        changeBrushType(tr("Solid"));
    else
        changeBrushType(tr("Gradient"));

    k->outlineAndFillColors->setCurrent(TDualColorButton::Foreground);
    setColor(brush);
}

void TupColorPalette::setBG(const QBrush &brush)
{
    if (brush.color().isValid())
        changeBrushType(tr("Solid"));
    else
        changeBrushType(tr("Gradient"));

    k->outlineAndFillColors->setCurrent(TDualColorButton::Background);
    setColor(brush);
}

void TupColorPalette::changeTypeColor(TDualColorButton::ColorSpace s)
{
    if (s == TDualColorButton::Background) {
        k->outlineAndFillColors->setCurrent(s);
        setColor(k->outlineAndFillColors->background());
    } else {
        k->outlineAndFillColors->setCurrent(s);
        k->flagGradient = false;
        setColor(k->outlineAndFillColors->foreground());
        k->flagGradient = true;
    }
}
*/

QIcon TupColorPalette::setComboColor(const QColor &color) const
{
    QPixmap pixmap(10, 10);
    pixmap.fill(color);
    //TIcon icon(pixmap, QBrush(color));
    QIcon icon(pixmap); 

    return icon;
}

void TupColorPalette::updateColorType(int index)
{
    if (index == TupColorPalette::Solid) {
        tFatal() << "TupColorPalette::updateColorType() - Solid Color!";
        if (k->currentSpace == TDualColorButton::Foreground)
            k->fgType = Solid; 
        else
            k->bgType = Solid;
    } else {
        tFatal() << "TupColorPalette::updateColorType() - Gradient Color!";
        if (k->currentSpace == TDualColorButton::Foreground) 
            k->fgType = Gradient;
        else
            k->bgType = Gradient;
    }
}

