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
#include <QTabWidget>

struct KTColorPalette::Private
{
    QSplitter *splitter;
    QTabWidget *tab;

    KTViewColorCells *containerPalette;
    KTColorValue *displayColorForms;
    KTColorPicker *colorPickerArea;
    KTLuminancePicker *luminancePicker;
    KTGradientCreator *gradientManager;

    QComboBox *labelType;
    QLineEdit *htmlNameColor;
    KDualColorButton *outlineAndFillColors;
    QBrush currentOutlineColor;
    QBrush currentFillColor;

    bool flagGradient;
    BrushType type;
    KDualColorButton::ColorSpace currentSpace;
    KTColorPalette::BrushType fgType;
    KTColorPalette::BrushType bgType; 
};

KTColorPalette::KTColorPalette(QWidget *parent) : KTModuleWidgetBase(parent), k(new Private)
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

void KTColorPalette::setupDisplayColor()
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
    k->outlineAndFillColors = new KDualColorButton(k->currentOutlineColor, k->currentFillColor, viewColor);
    k->outlineAndFillColors->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(k->outlineAndFillColors, SIGNAL(selectionChanged(KDualColorButton::ColorSpace)),
            this, SLOT(updateColorSpace(KDualColorButton::ColorSpace)));

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

void KTColorPalette::setupMainPalette()
{
    //palettes
    k->containerPalette = new KTViewColorCells(k->splitter);
    connect(k->containerPalette, SIGNAL(selectColor(const QBrush&)), this, SLOT(updateColorFromPalette(const QBrush&)));

    k->splitter->addWidget(k->containerPalette);
}

void KTColorPalette::setupChooserTypeColor()
{
    QFrame *colorMixer = new QFrame;
    colorMixer->setFrameStyle(QFrame::Box | QFrame::Sunken);

    QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);

    colorMixer->setLayout(layout);

    QBoxLayout *layoutContainer = new QBoxLayout(QBoxLayout::LeftToRight);
    layoutContainer->addStretch(2);

    k->colorPickerArea = new KTColorPicker(colorMixer);
    connect(k->colorPickerArea, SIGNAL(newColor(int, int)), this, SLOT(setHS(int, int)));

    k->displayColorForms = new KTColorValue(colorMixer);
    k->displayColorForms->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(k->displayColorForms, SIGNAL(hueChanged(int)), k->colorPickerArea, SLOT(setHUE(int)));
    connect(k->displayColorForms, SIGNAL(saturationChanged(int)), k->colorPickerArea, SLOT(setSaturation(int)));
    connect(k->displayColorForms, SIGNAL(brushChanged(const QBrush&)), this, SLOT(updateColorFromDisplay(const QBrush&)));

    layoutContainer->addWidget(k->colorPickerArea, 0, Qt::AlignLeft);

    k->luminancePicker = new KTLuminancePicker(colorMixer);
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

void KTColorPalette::setupGradientManager()
{
    k->gradientManager = new KTGradientCreator(this);
    // connect(k->gradientManager, SIGNAL(gradientChanged(const QBrush&)), this, SLOT(updateGradientColor(const QBrush &)));

    k->tab->addTab(k->gradientManager, tr("Gradients"));
    // SQA: Temporary code
    k->tab->setTabEnabled(1, false);
}

void KTColorPalette::setColor(const QBrush& brush)
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
               tFatal() << "KTColorPalette::setColor() - Sending gradient value!";
               KTPaintAreaEvent event(KTPaintAreaEvent::ChangeBrush, brush);
               emit paintAreaEventTriggered(&event);
               return;
               */
    }

    tFatal() << "KTColorPalette::setColor() - Updating background color!";
    // KTPaintAreaEvent event(KTPaintAreaEvent::ChangeBrush, k->outlineAndFillColors->background().color());
    KTPaintAreaEvent event(KTPaintAreaEvent::ChangeBrush, brush);
    emit paintAreaEventTriggered(&event);

    tFatal() << "KTColorPalette::setColor() - Updating foreground color!";
    KTPaintAreaEvent event2(KTPaintAreaEvent::ChangeColorPen, k->outlineAndFillColors->foreground().color());
    emit paintAreaEventTriggered(&event2);
}

void KTColorPalette::setGlobalColors(const QBrush &brush)
{
    if (k->currentSpace == KDualColorButton::Foreground) {
        k->outlineAndFillColors->setForeground(brush);
        k->labelType->setItemIcon(KDualColorButton::Foreground, setComboColor(brush.color()));
        k->currentOutlineColor = brush;

        KTPaintAreaEvent event(KTPaintAreaEvent::ChangeColorPen, brush.color());
        emit paintAreaEventTriggered(&event);
    } else {
        k->outlineAndFillColors->setBackground(brush);
        k->labelType->setItemIcon(KDualColorButton::Background, setComboColor(brush.color()));
        k->currentFillColor = brush;

        KTPaintAreaEvent event(KTPaintAreaEvent::ChangeBrush, brush);
        emit paintAreaEventTriggered(&event);
    }

    k->htmlNameColor->setText(brush.color().name());
}

void KTColorPalette::updateColorFromPalette(const QBrush &brush)
{
    setGlobalColors(brush);
    QColor color = brush.color();
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
    k->displayColorForms->setColor(color);
    k->gradientManager->setCurrentColor(color);
}

void KTColorPalette::updateColorFromDisplay(const QBrush &brush)
{
    tFatal() << "KTColorPalette::updateColorFromDisplay() - Just tracing color: " << brush.color().name();
    setGlobalColors(brush);

    QColor color = brush.color();
    k->colorPickerArea->setColor(color.hue(), color.saturation());
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
}

void KTColorPalette::updateColorSpace(KDualColorButton::ColorSpace space)
{
    k->currentSpace = space;

    QColor color;
    if (k->currentSpace == KDualColorButton::Foreground)
        color = k->outlineAndFillColors->foreground().color();
        // color = k->currentOutlineColor.color().name();
    else
        color = k->outlineAndFillColors->background().color();

    k->labelType->setCurrentIndex(k->currentSpace);

    tFatal() << "KTColorPalette::updateColorSpace() - Picking button #" << space;
    tFatal() << "KTColorPalette::updateColorSpace() - Color: " << color.name();
    tFatal() << "KTColorPalette::updateColorSpace() - Alpha: " << color.alpha();

    k->htmlNameColor->setText(color.name());
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
    k->displayColorForms->setColor(color);
}

void KTColorPalette::updateGradientColor(const QBrush &brush)
{
    tFatal() << "KTColorPalette::updateGradientColor() - Just tracing!";
    setGlobalColors(brush);
}

void KTColorPalette::syncHsv(int h, int s, int v)
{
    QColor color = k->outlineAndFillColors->currentColor().color();
    color.setHsv(h, s, v, color.alpha());

    setGlobalColors(QBrush(color));
    k->displayColorForms->setColor(color);
    // k->colorPickerArea->setColor(color.hue(), color.saturation());
}

void KTColorPalette::setHS(int hue, int saturation)
{
    tFatal() << "KTColorPalette::setHS() - H: " << hue;
    tFatal() << "KTColorPalette::setHS() - S: " << saturation;

    int luminance = 255;
    if (hue == 0 && saturation == 0)
        luminance = 0;

    QColor color;
    color.setHsv(hue, saturation, luminance, 255);
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
    k->displayColorForms->setColor(color);

    tFatal() << "KTColorPalette::setHS() - Color: " << color.name();
    tDebug() << "";

    setGlobalColors(QBrush(color));
}

void KTColorPalette::updateColor()
{
    QColor tmpColor(k->htmlNameColor->text());
    tmpColor.setAlpha(k->displayColorForms->alpha());
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

void KTColorPalette::setColorSpace(int type)
{
    k->currentSpace = KDualColorButton::ColorSpace(type);
    k->outlineAndFillColors->setCurrent(k->currentSpace);

    if (k->currentSpace == KDualColorButton::Foreground) {
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

void KTColorPalette::init()
{
    k->currentSpace = KDualColorButton::Foreground;
    QBrush brush = QBrush(Qt::transparent);

    QColor color = Qt::black;
    k->currentOutlineColor = QBrush(color);
    k->currentFillColor = brush;

    k->outlineAndFillColors->setForeground(QBrush(color));
    k->outlineAndFillColors->setBackground(brush);

    k->labelType->setItemIcon(KDualColorButton::Foreground, setComboColor(Qt::black));
    k->labelType->setItemIcon(KDualColorButton::Background, setComboColor(Qt::transparent));

    k->colorPickerArea->setColor(color.hue(), color.saturation());
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
    k->displayColorForms->setColor(color);
    k->gradientManager->setCurrentColor(Qt::white);

    KTPaintAreaEvent event(KTPaintAreaEvent::ChangeColorPen, Qt::black);
    emit paintAreaEventTriggered(&event);

    event = KTPaintAreaEvent(KTPaintAreaEvent::ChangeBrush, brush);
    emit paintAreaEventTriggered(&event);
}

void KTColorPalette::switchColors()
{
     QBrush tmp = k->currentOutlineColor;
     k->currentOutlineColor = k->currentFillColor;
     k->currentFillColor = tmp;

     KTPaintAreaEvent event(KTPaintAreaEvent::ChangeColorPen, k->currentOutlineColor.color());
     emit paintAreaEventTriggered(&event);

     event = KTPaintAreaEvent(KTPaintAreaEvent::ChangeBrush, k->currentFillColor);
     emit paintAreaEventTriggered(&event);

     k->labelType->setItemIcon(KDualColorButton::Foreground, setComboColor(k->currentOutlineColor.color()));
     k->labelType->setItemIcon(KDualColorButton::Background, setComboColor(k->currentFillColor.color()));
}

void KTColorPalette::resetColors()
{
     QColor color = Qt::black;
     k->currentOutlineColor = QBrush(color);
     k->currentFillColor = QBrush(Qt::transparent);

     KTPaintAreaEvent event(KTPaintAreaEvent::ChangeColorPen, k->currentOutlineColor.color());
     emit paintAreaEventTriggered(&event);

     event = KTPaintAreaEvent(KTPaintAreaEvent::ChangeBrush, k->currentFillColor);
     emit paintAreaEventTriggered(&event);

     k->labelType->setItemIcon(KDualColorButton::Foreground, setComboColor(k->currentOutlineColor.color()));
     k->labelType->setItemIcon(KDualColorButton::Background, setComboColor(k->currentFillColor.color()));

     k->colorPickerArea->setColor(color.hue(), color.saturation());
     k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
     k->displayColorForms->setColor(color);
    // k->gradientManager->setCurrentColor(Qt::white);
}

/*
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

    k->outlineAndFillColors->setCurrent(KDualColorButton::Background);
    setColor(brush);
}

void KTColorPalette::changeTypeColor(KDualColorButton::ColorSpace s)
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
*/

QIcon KTColorPalette::setComboColor(const QColor &color) const
{
    QPixmap pixmap(10, 10);
    pixmap.fill(color);
    //TIcon icon(pixmap, QBrush(color));
    QIcon icon(pixmap); 

    return icon;
}

void KTColorPalette::updateColorType(int index)
{
    if (index == KTColorPalette::Solid) {
        tFatal() << "KTColorPalette::updateColorType() - Solid Color!";
        if (k->currentSpace == KDualColorButton::Foreground)
            k->fgType = Solid; 
        else
            k->bgType = Solid;
    } else {
        tFatal() << "KTColorPalette::updateColorType() - Gradient Color!";
        if (k->currentSpace == KDualColorButton::Foreground) 
            k->fgType = Gradient;
        else
            k->bgType = Gradient;
    }
}

