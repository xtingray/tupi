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

#include "tupbrushwidget.h"

struct TupBrushWidget::Private
{
    TEditSpinBox *thickness;

    QPushButton *roundCapButton;
    QPushButton *flatCapButton;
    QPushButton *squareCapButton;

    QPushButton *miterJoinButton;
    QPushButton *bevelJoinButton;
    QPushButton *roundJoinButton;

    QComboBox *style;
    QListWidget *brushesList;
    QPen pen;
    QBrush brush;
    TupPenThicknessWidget *thickPreview;
};

TupBrushWidget::TupBrushWidget(QWidget *parent) : TupModuleWidgetBase(parent), k(new Private)
{
    setWindowTitle(tr("Brush Properties"));

    TCONFIG->beginGroup("BrushParameters");
    int thicknessValue = TCONFIG->value("Thickness", 3).toInt();
    if (thicknessValue > 100)
        thicknessValue = 3;

    k->thickPreview = new TupPenThicknessWidget(this);
    k->thickPreview->render(thicknessValue);

    k->thickness = new TEditSpinBox(thicknessValue, 1, 100, 1, tr("Thickness"));
    k->thickness->setValue(thicknessValue);

    connect(k->thickness, SIGNAL(valueChanged(int)), this, SLOT(setThickness(int)));
    connect(k->thickness, SIGNAL(valueChanged(int)), k->thickPreview, SLOT(render(int)));

    addChild(k->thickPreview);
    addChild(k->thickness);

    QWidget *space = new QWidget(this);
    space->setFixedHeight(5);
    addChild(space);

    QLabel *label = new QLabel(tr("Dashes") + ":", this);
    addChild(label);

    QWidget *styleWidget = new QWidget(this);
    QBoxLayout *styleLayout = new QHBoxLayout(styleWidget);

    k->style = new QComboBox();
    k->style->setIconSize(QSize(145, 13));
    k->style->setFixedWidth(180);

    int flag = Qt::SolidLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style01.png"), "", QVariant(flag));
    flag = Qt::DashLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style02.png"), "", QVariant(flag));
    flag = Qt::DotLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style03.png"), "", QVariant(flag));
    flag = Qt::DashDotLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style04.png"), "", QVariant(flag));
    flag = Qt::DashDotDotLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style05.png"), "", QVariant(flag));

    styleLayout->addWidget(k->style);
    connect(k->style, SIGNAL(currentIndexChanged(int)), this, SLOT(setStyle(int)));

    addChild(styleWidget);

    space = new QWidget(this);
    space->setFixedHeight(5);
    addChild(space);

    label = new QLabel(tr("Cap") + ":", this);
    addChild(label);

    QWidget *capWidget = new QWidget(this);
    QBoxLayout *capLayout = new QHBoxLayout(capWidget);

    k->roundCapButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/round_cap.png")), "");
    k->roundCapButton->setToolTip(tr("Round"));
    k->roundCapButton->setIconSize(QSize(30, 15));
    k->roundCapButton->setCheckable(true);

    connect(k->roundCapButton, SIGNAL(clicked()), this, SLOT(enableRoundCapStyle()));

    k->squareCapButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/square_cap.png")), "");
    k->squareCapButton->setToolTip(tr("Square"));
    k->squareCapButton->setIconSize(QSize(33, 15));
    k->squareCapButton->setCheckable(true);

    connect(k->squareCapButton, SIGNAL(clicked()), this, SLOT(enableSquareCapStyle()));

    k->flatCapButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/flat_cap.png")), "");
    k->flatCapButton->setToolTip(tr("Flat"));
    k->flatCapButton->setIconSize(QSize(27, 15));
    k->flatCapButton->setCheckable(true);

    connect(k->flatCapButton, SIGNAL(clicked()), this, SLOT(enableFlatCapStyle()));

    capLayout->addWidget(k->roundCapButton);
    capLayout->addWidget(k->squareCapButton);
    capLayout->addWidget(k->flatCapButton);
    addChild(capWidget);

    space = new QWidget(this);
    space->setFixedHeight(5);
    addChild(space);

    label = new QLabel(tr("Join") + ":", this);
    addChild(label);

    QWidget *joinWidget = new QWidget(this);
    QBoxLayout *joinLayout = new QHBoxLayout(joinWidget);

    k->roundJoinButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/round_join.png")), "");
    k->roundJoinButton->setToolTip(tr("Round"));
    k->roundJoinButton->setIconSize(QSize(30, 15));
    k->roundJoinButton->setCheckable(true);

    connect(k->roundJoinButton, SIGNAL(clicked()), this, SLOT(enableRoundJoinStyle()));

    k->bevelJoinButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/bevel_join.png")), "");
    k->bevelJoinButton->setToolTip(tr("Bevel"));
    k->bevelJoinButton->setIconSize(QSize(33, 15));
    k->bevelJoinButton->setCheckable(true);

    connect(k->bevelJoinButton, SIGNAL(clicked()), this, SLOT(enableBevelJoinStyle()));

    k->miterJoinButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons/miter_join.png")), "");
    k->miterJoinButton->setToolTip(tr("Miter"));
    k->miterJoinButton->setIconSize(QSize(27, 15));
    k->miterJoinButton->setCheckable(true);

    connect(k->miterJoinButton, SIGNAL(clicked()), this, SLOT(enableMiterJoinStyle()));

    joinLayout->addWidget(k->roundJoinButton);
    joinLayout->addWidget(k->bevelJoinButton);
    joinLayout->addWidget(k->miterJoinButton);
    addChild(joinWidget);

    space = new QWidget(this);
    space->setFixedHeight(5);
    addChild(space);

    label = new QLabel(tr("Brush") + ":", this);
    addChild(label);

    addBrushesList();
 
    boxLayout()->addStretch(2);
    
    setWindowIcon(QIcon(THEME_DIR + "icons/brush.png"));
}

TupBrushWidget::~TupBrushWidget()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupBrushWidget()]";
        #else
            TEND;
        #endif
    #endif 
		
    delete k;
}

void TupBrushWidget::setThickness(int thickness)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupBrushWidget::setThickness()]";
        #else
            T_FUNCINFO << "thickness: " << thickness;
        #endif
    #endif

    if (thickness > 0) {
        k->pen.setWidth(thickness);
        TCONFIG->beginGroup("BrushParameters");
        TCONFIG->setValue("Thickness", thickness);
        updatePenProperties();
    }
}

void TupBrushWidget::setStyle(int style)
{
    k->pen.setStyle(Qt::PenStyle(k->style->itemData(style).toInt()));
    updatePenProperties();
}

void TupBrushWidget::setBrushStyle(QListWidgetItem *item)
{
    if (item->toolTip().compare("TexturePattern") == 0) {
        k->brush = QBrush(QPixmap(THEME_DIR + "icons/brush_15.png"));
        k->thickPreview->setBrush(24);
    } else {
        int index = k->brushesList->row(item);
        k->thickPreview->setBrush(index+1);
        k->brush.setStyle(Qt::BrushStyle(index+1));
    }

    updatePenProperties();
}

void TupBrushWidget::setPenColor(const QColor color)
{
    k->brush.setColor(color);
    k->thickPreview->setColor(color);
}

void TupBrushWidget::setPenThickness(int thickness)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupBrushWidget::setPenThickness()]";
        #else
            T_FUNCINFO << "thickness: " << thickness;
        #endif
    #endif

    k->pen.setWidth(thickness);
    TCONFIG->beginGroup("BrushParameters");
    TCONFIG->setValue("Thickness", thickness);
    k->thickPreview->render(thickness);

    k->thickness->blockSignals(true);
    k->thickness->setValue(thickness);
    k->thickness->blockSignals(false);
}

void TupBrushWidget::setBrush(const QBrush brush)
{
    k->brush = brush;
    k->thickPreview->setBrush(brush);
}

void TupBrushWidget::init(int thickness)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupBrushWidget::init()]";
        #else
            T_FUNCINFO << "thickness: " << thickness;
        #endif
    #endif

    blockSignals(true);
    setPenColor(QColor(0, 0, 0));

    enableRoundCapStyle();
    enableRoundJoinStyle();
    k->style->setCurrentIndex(0);

    QListWidgetItem *first = k->brushesList->item(0);
    k->brushesList->setCurrentItem(first);
    setBrushStyle(first);
    blockSignals(false);

    setThickness(thickness);
}

QPen TupBrushWidget::pen() const
{
    return k->pen;
}

void TupBrushWidget::updatePenProperties()
{
    k->pen.setBrush(k->brush);

    TupPaintAreaEvent event(TupPaintAreaEvent::ChangePen, k->pen);
    emit paintAreaEventTriggered(&event);
}

void TupBrushWidget::updateBrushProperties()
{
    TupPaintAreaEvent event(TupPaintAreaEvent::ChangeBrush, k->brush);
    emit paintAreaEventTriggered(&event);
}

void TupBrushWidget::addBrushesList()
{
    k->brushesList = new QListWidget(this);
    k->brushesList->setViewMode(QListView::IconMode);
    k->brushesList->setFlow(QListView::LeftToRight);
    k->brushesList->setMovement(QListView::Static);

    QListWidgetItem *brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_01.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("Solid");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_02.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("Dense1Pattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_03.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("Dense2Pattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_04.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("Dense3Pattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_05.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("Dense4Pattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_06.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("Dense5Pattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_07.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("Dense6Pattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_08.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("Dense7Pattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_09.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("HotPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_10.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("VerPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_11.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("CrossPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_12.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("BDiagPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_13.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("FDiagPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_14.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("DiagCrossPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    /*
    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush_15.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("TexturePattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("LinearGradientPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("ConicalGradientPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    brushItem = new QListWidgetItem(k->brushesList);
    brushItem->setIcon(QIcon(THEME_DIR + "icons/brush.png"));
    brushItem->setFont(QFont("verdana", 8));
    brushItem->setToolTip("RadialGradientPattern");
    brushItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    */

    k->brushesList->setFixedWidth(255);
    k->brushesList->setFixedHeight(63);

    addChild(k->brushesList);
    connect(k->brushesList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(setBrushStyle(QListWidgetItem *)));
}

void TupBrushWidget::enableRoundCapStyle()
{
    if (!k->roundCapButton->isDown())
        k->roundCapButton->setChecked(true);

    if (k->squareCapButton->isChecked())
        k->squareCapButton->setChecked(false);

    if (k->flatCapButton->isChecked())
        k->flatCapButton->setChecked(false);

    k->pen.setCapStyle(Qt::RoundCap);

    updatePenProperties();
}

void TupBrushWidget::enableSquareCapStyle()
{
    if (!k->squareCapButton->isDown())
        k->squareCapButton->setChecked(true);

    if (k->roundCapButton->isChecked())
        k->roundCapButton->setChecked(false);

    if (k->flatCapButton->isChecked())
        k->flatCapButton->setChecked(false);

    k->pen.setCapStyle(Qt::SquareCap);
    updatePenProperties();
}

void TupBrushWidget::enableFlatCapStyle()
{
    if (!k->flatCapButton->isDown())
        k->flatCapButton->setChecked(true);

    if (k->roundCapButton->isChecked())
        k->roundCapButton->setChecked(false);

    if (k->squareCapButton->isChecked())
        k->squareCapButton->setChecked(false);

    k->pen.setCapStyle(Qt::FlatCap);
    updatePenProperties();
}

void TupBrushWidget::enableRoundJoinStyle()
{
    if (!k->roundJoinButton->isDown())
        k->roundJoinButton->setChecked(true);

    if (k->miterJoinButton->isChecked())
        k->miterJoinButton->setChecked(false);

    if (k->bevelJoinButton->isChecked())
        k->bevelJoinButton->setChecked(false);

    k->pen.setJoinStyle(Qt::RoundJoin);
    updatePenProperties();
}

void TupBrushWidget::enableMiterJoinStyle()
{
    if (!k->miterJoinButton->isDown())
        k->miterJoinButton->setChecked(true);

    if (k->bevelJoinButton->isChecked())
        k->bevelJoinButton->setChecked(false);

    if (k->roundJoinButton->isChecked())
        k->roundJoinButton->setChecked(false);

    k->pen.setJoinStyle(Qt::MiterJoin);
    updatePenProperties();
}

void TupBrushWidget::enableBevelJoinStyle()
{
    if (!k->bevelJoinButton->isDown())
        k->bevelJoinButton->setChecked(true);

    if (k->miterJoinButton->isChecked())
        k->miterJoinButton->setChecked(false);
    
    if (k->roundJoinButton->isChecked())
        k->roundJoinButton->setChecked(false);

    k->pen.setJoinStyle(Qt::BevelJoin);
    updatePenProperties();
}
