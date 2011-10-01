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
    KTColorValue *displayColorValue;
    KTColorPicker *colorPicker;
    KTLuminancePicker *luminancePicker;
    KTGradientCreator *gradientManager;

    QComboBox *labelType;
    QLineEdit *nameColor;
    KDualColorButton *outlineAndFillColors;
    QBrush currentOutlineColor;
    QBrush currentFillColor;

    bool flagGradient;
    BrushType type;
    KDualColorButton::ColorSpace currentSpace;
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
    k->tab = new QTabWidget;

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
    k->labelType->addItem(tr("Solid"));
    k->labelType->addItem(tr("Gradient"));

    connect(k->labelType, SIGNAL(activated(const QString &)), this, SLOT(changeBrushType(const QString &)));

    vlayout->addWidget(k->labelType);

    k->outlineAndFillColors = new KDualColorButton(k->currentOutlineColor, k->currentFillColor, viewColor);
    k->outlineAndFillColors->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    k->currentSpace = KDualColorButton::Foreground;

    connect(k->outlineAndFillColors, SIGNAL(selectionChanged(KDualColorButton::ColorSpace)),
            this, SLOT(updateColorSpace(KDualColorButton::ColorSpace)));

    connect(k->outlineAndFillColors, SIGNAL(switchColors()), this, SLOT(switchColors()));
    connect(k->outlineAndFillColors, SIGNAL(resetColors()), this, SLOT(resetColors()));

    vlayout->addWidget(k->outlineAndFillColors);

    QBoxLayout *layoutName = new QBoxLayout(QBoxLayout::TopToBottom);
    layoutName->setMargin(0);

    layoutName->addWidget(new QLabel("<b>HTML</b>", viewColor));
    k->nameColor = new QLineEdit(viewColor);
    k->nameColor->setMaximumWidth(70);
    k->nameColor->setMaxLength(7);
    k->nameColor->setText("#000000");

    connect(k->nameColor, SIGNAL(editingFinished()), this, SLOT(updateColor()));
    layoutName->addWidget(k->nameColor);
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
    connect(k->displayColorValue, SIGNAL(brushChanged(const QBrush&)), this, SLOT(updateColor(const QBrush &)));

    k->tab->addTab(colorMixer, tr("Color Mixer"));
}

void KTColorPalette::setupGradientManager()
{
    k->gradientManager = new KTGradientCreator(this);
    connect(k->gradientManager, SIGNAL(gradientChanged(const QBrush&)), this, SLOT(setColor(const QBrush &)));
    k->tab->addTab(k->gradientManager, tr("Gradients"));
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
            k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
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

void KTColorPalette::setGlobalColors(QBrush brush)
{
    if (k->currentSpace == KDualColorButton::Foreground) {
        k->outlineAndFillColors->setForeground(brush);
        k->currentOutlineColor = brush;
        KTPaintAreaEvent event(KTPaintAreaEvent::ChangeColorPen, brush.color());
        emit paintAreaEventTriggered(&event);
    } else {
        k->outlineAndFillColors->setBackground(brush);
        k->currentFillColor = brush;
        KTPaintAreaEvent event(KTPaintAreaEvent::ChangeBrush, brush);
        emit paintAreaEventTriggered(&event);
    }

    k->nameColor->setText(brush.color().name());
}

void KTColorPalette::updateColorFromPalette(const QBrush& brush)
{
    setGlobalColors(brush);

    QColor color = brush.color();
    // k->nameColor->setText(color.name());
    k->displayColorValue->setColor(color);
    k->luminancePicker->setColor(color.hue(), color.saturation(), color.value());
}

void KTColorPalette::updateColorSpace(KDualColorButton::ColorSpace space)
{
    tFatal() << "KTColorPalette::updateColorSpace() - Picking button #" << space;
    k->currentSpace = space;
    if (k->currentSpace == KDualColorButton::Foreground)
        k->nameColor->setText(k->currentOutlineColor.color().name());
    else
        k->nameColor->setText(k->currentFillColor.color().name());
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

void KTColorPalette::syncHsv(int h, int s, int v)
{
    QColor tmpColor = k->outlineAndFillColors->currentColor().color();
    tmpColor.setHsv(h, s, v, tmpColor.alpha());

    // if (k->luminancePicker == sender())
    //     setColor(tmpColor);

    setGlobalColors(QBrush(tmpColor));
}

void KTColorPalette::setHS(int h, int s)
{
    QColor tmpColor = k->outlineAndFillColors->currentColor().color();
    tmpColor.setHsv(h, s, k->luminancePicker->value(), tmpColor.alpha());
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

/*
void KTColorPalette::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton) {
        QMenu *menu = new QMenu(tr("type brush"), this);
        menu->exec(e->globalPos());
        delete menu;
    }
}
*/

void KTColorPalette::changeBrushType(const QString& type)
{
    if (type == tr("Solid")) {
        k->type = Solid;
    } else if (type == tr("Gradient")) {
               k->type = Gradient;
    }

    if (type != k->labelType->currentText()) {
        int index = k->labelType->findText(type);
        if (index >= 0)
            k->labelType->setCurrentIndex(index);
    }
}

void KTColorPalette::init()
{
    QBrush brush = QBrush(Qt::transparent);

    k->currentOutlineColor = QBrush(Qt::black);
    k->currentFillColor = brush;

    k->outlineAndFillColors->setForeground(QBrush(Qt::black));
    k->outlineAndFillColors->setBackground(brush);

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
}

void KTColorPalette::resetColors()
{
     k->currentOutlineColor = QBrush(Qt::black);
     k->currentFillColor = QBrush(Qt::transparent);

     KTPaintAreaEvent event(KTPaintAreaEvent::ChangeColorPen, k->currentOutlineColor.color());
     emit paintAreaEventTriggered(&event);

     event = KTPaintAreaEvent(KTPaintAreaEvent::ChangeBrush, k->currentFillColor);
     emit paintAreaEventTriggered(&event);
}
