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

#include "tupcolorpalette.h"
#include "timagebutton.h"

struct TupColorPalette::Private
{
    QSplitter *splitter;
    QTabWidget *tab;

    TupViewColorCells *paletteContainer;
    TupColorValue *displayColorForms;
    TupColorPicker *colorPickerArea;
    TSlider *luminancePicker;
    TupGradientCreator *gradientManager;

    QLineEdit *htmlField;
    QBrush currentContourBrush;
    QBrush currentFillBrush;

    TColorCell *contourColor;
    TColorCell *fillColor;

    bool flagGradient;
    BrushType type;

    TColorCell::FillType currentSpace;
    TupColorPalette::BrushType fgType;
    TupColorPalette::BrushType bgType; 
};

TupColorPalette::TupColorPalette(QWidget *parent) : TupModuleWidgetBase(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupColorPalette()]";
        #else
            TINIT;
        #endif
    #endif

    k->currentSpace = TColorCell::Contour;
    k->currentContourBrush = Qt::black;
    k->currentFillBrush = Qt::transparent;
    k->flagGradient = true;
    // k->type = Solid;

    setWindowTitle(tr("Color Palette"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/color_palette.png"));

    k->splitter = new QSplitter(Qt::Vertical, this);

    k->tab = new QTabWidget;
    connect(k->tab, SIGNAL(currentChanged(int)), this, SLOT(updateColorType(int)));

    setupColorDisplay();

    addChild(k->splitter);

    setupMainPalette();
    setupChooserTypeColor();
    setupGradientManager();

    k->tab->setPalette(palette());
    k->tab->setMinimumHeight(300);
    k->splitter->addWidget(k->tab);

    setMinimumWidth(316);

    /*
    TCONFIG->beginGroup("ColorPalette");
    QColor foreground = QColor(TCONFIG->value("LastForegroundColor", Qt::black).toString());
    QColor background = QColor(TCONFIG->value("LastBackgroundColor", Qt::transparent).toString());
    */
}

TupColorPalette::~TupColorPalette()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupColorPalette()]";
        #else
            TEND;
        #endif
    #endif

    /*
    TCONFIG->beginGroup("ColorPalette");
    TCONFIG->setValue("LastForegroundColor", color().first);
    TCONFIG->setValue("LastBackgroundColor", color().second);
    */
}

void TupColorPalette::setupColorDisplay()
{
    QFrame *topPanel = new QFrame(this);
    QBoxLayout *generalLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    generalLayout->setMargin(0);
    topPanel->setLayout(generalLayout);

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(1);

    mainLayout->addWidget(new QWidget());

    TImageButton *changeButton = new TImageButton(QIcon(QPixmap(THEME_DIR + "icons/exchange_colors.png")), 20, this, true);
    changeButton->setToolTip(tr("Exchange colors"));
    connect(changeButton, SIGNAL(clicked()), this, SLOT(switchColors()));
    mainLayout->addWidget(changeButton);
    mainLayout->setSpacing(5);

    QBoxLayout *listLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    QLabel *contourLabel = new QLabel(tr("Contour"));
    QLabel *fillLabel = new QLabel(tr("Fill"));

    QSize cellSize(25, 25);
    QColor contourColor(0, 0, 0);
    QBrush contourBrush = QBrush(Qt::black);
    k->contourColor = new TColorCell(TColorCell::Contour, contourBrush, cellSize);
    k->contourColor->setSelected(true);
    connect(k->contourColor, SIGNAL(clicked(TColorCell::FillType)), this, SLOT(updateColorMode(TColorCell::FillType)));

    QBrush fillBrush = QBrush(Qt::transparent);
    k->fillColor = new TColorCell(TColorCell::Inner, fillBrush, cellSize);
    connect(k->fillColor, SIGNAL(clicked(TColorCell::FillType)), this, SLOT(updateColorMode(TColorCell::FillType)));

    QBoxLayout *contourLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    contourLayout->addWidget(k->contourColor);
    contourLayout->addWidget(contourLabel);

    QBoxLayout *fillLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    fillLayout->addWidget(k->fillColor);
    fillLayout->addWidget(fillLabel);

    listLayout->addLayout(contourLayout);
    listLayout->setSpacing(5);
    listLayout->addLayout(fillLayout);

    mainLayout->addLayout(listLayout);

    TImageButton *resetButton = new TImageButton(QIcon(QPixmap(THEME_DIR + "icons/reset_colors.png")), 15, this, true);
    resetButton->setToolTip(tr("Reset colors"));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(init()));
    mainLayout->addWidget(resetButton);

    mainLayout->addWidget(new QWidget());

    QLabel *htmlLabel = new QLabel(tr("HTML"), topPanel);
    htmlLabel->setMaximumWidth(50);
    htmlLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    mainLayout->addWidget(htmlLabel);

    k->htmlField = new QLineEdit(topPanel);
    k->htmlField->setMaximumWidth(70);
    k->htmlField->setText("#000000");
    connect(k->htmlField, SIGNAL(editingFinished()), this, SLOT(updateColor()));
    mainLayout->addWidget(k->htmlField);

    mainLayout->addWidget(new QWidget());

    generalLayout->addLayout(mainLayout);
    generalLayout->addWidget(new QWidget());
    generalLayout->setAlignment(mainLayout, Qt::AlignHCenter|Qt::AlignVCenter);

    addChild(topPanel);
}

void TupColorPalette::updateColorMode(TColorCell::FillType flag)
{
    QBrush brush;
    if (flag == TColorCell::Contour) {
        k->currentSpace = TColorCell::Contour;
        brush = k->contourColor->brush();
        if (k->fillColor->isSelected()) 
            k->fillColor->setSelected(false);

        if (k->fgType == Solid && k->tab->currentIndex() != 0) {
            k->tab->setCurrentIndex(0);
        } else if (k->fgType == Gradient && k->tab->currentIndex() != 1) {
                   k->tab->setCurrentIndex(1);
        }
    } else {
        if (flag == TColorCell::Inner) {
            k->currentSpace = TColorCell::Inner;
            brush = k->fillColor->brush();
            if (k->contourColor->isSelected())
                k->contourColor->setSelected(false);

            if (k->bgType == Solid && k->tab->currentIndex() != 0) {
                k->tab->setCurrentIndex(0);
            } else if (k->bgType == Gradient && k->tab->currentIndex() != 1) {
                       k->tab->setCurrentIndex(1);
            }
        }
    }

    QColor color = brush.color();
    k->htmlField->setText(color.name());
    updateLuminancePicker(color);
    k->displayColorForms->setColor(color);
    k->gradientManager->setCurrentColor(color);
}

void TupColorPalette::setupMainPalette()
{
    // Palettes
    k->paletteContainer = new TupViewColorCells(k->splitter);
    connect(k->paletteContainer, SIGNAL(selectColor(const QBrush&)), this, SLOT(updateColorFromPalette(const QBrush&)));

    k->splitter->addWidget(k->paletteContainer);
}

void TupColorPalette::setupChooserTypeColor()
{
    QFrame *colorMixer = new QFrame;
    colorMixer->setFrameStyle(QFrame::Box | QFrame::Sunken);

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    colorMixer->setLayout(mainLayout);

    k->colorPickerArea = new TupColorPicker(colorMixer);
    connect(k->colorPickerArea, SIGNAL(newColor(int, int)), this, SLOT(setHS(int, int)));

    k->luminancePicker = new TSlider(Qt::Horizontal, TSlider::Color, QColor(0, 0, 0), QColor(255, 255, 255));
    connect(k->luminancePicker, SIGNAL(colorChanged(const QColor &)), this, SLOT(syncColor(const QColor &)));
    k->luminancePicker->setRange(0, 100);
    k->luminancePicker->setValue(100);

    k->displayColorForms = new TupColorValue(colorMixer);
    k->displayColorForms->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(k->displayColorForms, SIGNAL(hueChanged(int)), k->colorPickerArea, SLOT(setHUE(int)));
    connect(k->displayColorForms, SIGNAL(saturationChanged(int)), k->colorPickerArea, SLOT(setSaturation(int)));
    connect(k->displayColorForms, SIGNAL(brushChanged(const QBrush&)), this, SLOT(updateColorFromDisplay(const QBrush&)));

    mainLayout->addWidget(k->colorPickerArea);
    mainLayout->setAlignment(k->colorPickerArea, Qt::AlignHCenter);

    mainLayout->addWidget(k->luminancePicker);
    mainLayout->setAlignment(k->luminancePicker, Qt::AlignHCenter);

    mainLayout->addWidget(k->displayColorForms);
    mainLayout->setAlignment(k->displayColorForms, Qt::AlignHCenter);

    mainLayout->addStretch(2);

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

        k->colorPickerArea->setColor(color.hue(), color.saturation());
        // if (k->type == Solid)
        //     k->outlineAndFillColors->setCurrentColor(color);

        k->htmlField->setText(color.name());
        k->paletteContainer->setColor(brush);
        k->displayColorForms->setColor(color);
    } else if (brush.gradient()) {
               QGradient gradient(*brush.gradient());
               // changeBrushType(tr("Gradient"));

               k->paletteContainer->setColor(gradient);
               // k->outlineAndFillColors->setCurrentColor(gradient);
               if (sender() != k->gradientManager)
                   k->gradientManager->setGradient(gradient);

               /* SQA: Gradient issue pending for revision
               tFatal() << "TupColorPalette::setColor() - Sending gradient value!";
               TupPaintAreaEvent event(TupPaintAreaEvent::ChangeBrush, brush);
               emit paintAreaEventTriggered(&event);
               return;
               */
    }

    // tFatal() << "TupColorPalette::setColor() - Updating background color!";
    TupPaintAreaEvent fillEvent(TupPaintAreaEvent::ChangeBrush, brush);
    emit paintAreaEventTriggered(&fillEvent);

    // tFatal() << "TupColorPalette::setColor() - Updating foreground color!";
    TupPaintAreaEvent contourEvent(TupPaintAreaEvent::ChangePenColor, k->contourColor->color()); 
    emit paintAreaEventTriggered(&contourEvent);
}

void TupColorPalette::setGlobalColors(const QBrush &brush)
{
    if (k->currentSpace == TColorCell::Contour) {
        k->contourColor->setBrush(brush);
        k->currentContourBrush = brush;

        TupPaintAreaEvent event(TupPaintAreaEvent::ChangePenColor, brush.color());
        emit paintAreaEventTriggered(&event);
    } else {
        k->fillColor->setBrush(brush);
        k->currentFillBrush = brush;

        TupPaintAreaEvent event(TupPaintAreaEvent::ChangeBrush, brush);
        emit paintAreaEventTriggered(&event);
    }

    k->htmlField->setText(brush.color().name());
}

void TupColorPalette::updateColorFromPalette(const QBrush &brush)
{
    setGlobalColors(brush);
    QColor color = brush.color();

    updateLuminancePicker(color);
    k->displayColorForms->setColor(color);
    k->gradientManager->setCurrentColor(color);
}

void TupColorPalette::updateColorFromDisplay(const QBrush &brush)
{
    setGlobalColors(brush);

    QColor color = brush.color();
    k->colorPickerArea->setColor(color.hue(), color.saturation());

    updateLuminancePicker(color);
}

void TupColorPalette::updateGradientColor(const QBrush &brush)
{
    setGlobalColors(brush);
}

void TupColorPalette::syncColor(const QColor &color)
{
    setGlobalColors(QBrush(color));
    k->displayColorForms->setColor(color);
}

void TupColorPalette::setHS(int hue, int saturation)
{
    int luminance = 255;
    if (hue == 0 && saturation == 0)
        luminance = 0;

    QColor color;
    color.setHsv(hue, saturation, luminance, 255);

    setGlobalColors(QBrush(color));
    updateLuminancePicker(color);
    k->displayColorForms->setColor(color);
}

void TupColorPalette::updateColor()
{
    QColor tmpColor(k->htmlField->text());
    tmpColor.setAlpha(k->displayColorForms->alpha());
    setColor(tmpColor);
}

QPair<QColor, QColor> TupColorPalette::color()
{
    QPair<QColor, QColor> colors;
    colors.first = k->contourColor->color();
    colors.second = k->fillColor->color();

    return colors;
}

void TupColorPalette::parsePaletteFile(const QString &file)
{
    k->paletteContainer->readPaletteFile(file);
}

void TupColorPalette::init()
{
    k->currentSpace = TColorCell::Contour;

    QColor contourColor = Qt::black;
    k->currentContourBrush = QBrush(contourColor);
    k->htmlField->setText("#000000");
    QBrush fillBrush = QBrush(Qt::transparent);
    k->currentFillBrush = fillBrush;

    k->contourColor->setBrush(k->currentContourBrush); 
    k->fillColor->setBrush(fillBrush);

    k->contourColor->setSelected(true);
    if (k->fillColor->isSelected())
        k->fillColor->setSelected(false);

    k->paletteContainer->clearSelection();

    k->colorPickerArea->setColor(contourColor.hue(), contourColor.saturation());

    if (!k->luminancePicker->isEnabled())
        k->luminancePicker->setEnabled(true);

    k->luminancePicker->setColors(Qt::black, Qt::white);
    k->luminancePicker->setValue(0);

    k->displayColorForms->setColor(contourColor);
    k->gradientManager->setCurrentColor(Qt::white);

    TupPaintAreaEvent event(TupPaintAreaEvent::ChangePenColor, contourColor);
    emit paintAreaEventTriggered(&event);

    event = TupPaintAreaEvent(TupPaintAreaEvent::ChangeBrush, fillBrush);
    emit paintAreaEventTriggered(&event);
}

void TupColorPalette::switchColors()
{
    k->paletteContainer->clearSelection();

    QBrush tmp = k->currentContourBrush;
    k->currentContourBrush = k->currentFillBrush;
    k->currentFillBrush = tmp;

    k->contourColor->setBrush(k->currentContourBrush); 
    k->fillColor->setBrush(k->currentFillBrush);

    QColor color;
    if (k->currentSpace == TColorCell::Contour)
        color = k->contourColor->color();
    else
        color = k->fillColor->color();

    k->htmlField->setText(color.name());
    k->colorPickerArea->setColor(color.hue(), color.saturation());
    updateLuminancePicker(color);
    k->displayColorForms->setColor(color);

    TupPaintAreaEvent event(TupPaintAreaEvent::ChangePenColor, k->currentContourBrush.color());
    emit paintAreaEventTriggered(&event);

    event = TupPaintAreaEvent(TupPaintAreaEvent::ChangeBrush, k->currentFillBrush);
    emit paintAreaEventTriggered(&event);
}

QIcon TupColorPalette::setComboColor(const QColor &color) const
{
    QPixmap pixmap(10, 10);
    pixmap.fill(color);
    QIcon icon(pixmap); 

    return icon;
}

void TupColorPalette::updateColorType(int index)
{
    if (index == TupColorPalette::Solid) {
        if (k->currentSpace == TColorCell::Contour)
            k->fgType = Solid; 
        else
            k->bgType = Solid;
    } else {
        if (k->currentSpace == TColorCell::Inner) 
            k->fgType = Gradient;
        else
            k->bgType = Gradient;
    }
}

void TupColorPalette::updateLuminancePicker(const QColor &color)
{
    if (color == Qt::transparent) {
        k->luminancePicker->setEnabled(false);
        return;
    } 

    if (!k->luminancePicker->isEnabled())
        k->luminancePicker->setEnabled(true);

    if (color != Qt::black) {
        k->luminancePicker->setColors(Qt::black, color);
        k->luminancePicker->setValue(100);
    } else {
        k->luminancePicker->setColors(Qt::black, Qt::white);
        k->luminancePicker->setValue(0);
    }
}
