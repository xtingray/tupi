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

#include "tuppenwidget.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tconfig.h"

#include "tuppaintareaevent.h"
#include "tuppenthicknesswidget.h"

#include <QListWidget>

struct TupPenWidget::Private
{
    TEditSpinBox *thickness;
    QComboBox *capStyle;
    QComboBox *joinStyle;
    QComboBox *style;
    QListWidget *brushesList;
    QPen pen;
    QBrush brush;
    TupPenThicknessWidget *thickPreview;
};

TupPenWidget::TupPenWidget(QWidget *parent) : TupModuleWidgetBase(parent), k(new Private)
{
    setWindowTitle(tr("Pen Properties"));

    TCONFIG->beginGroup("PenParameters");
    int thicknessValue = TCONFIG->value("Thickness", -1).toInt();

    if (thicknessValue <= 0)
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
    
    k->style = new QComboBox();
    k->style->setIconSize(QSize(140, 13));

    int flag = Qt::SolidLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style01.png"), tr("Solid"), QVariant(flag));
    flag = Qt::DashLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style02.png"), tr("Dash"), QVariant(flag));
    flag = Qt::DotLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style03.png"), tr("Dot"), QVariant(flag));
    flag = Qt::DashDotLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style04.png"), tr("Dash dot"), QVariant(flag));
    flag = Qt::DashDotDotLine;
    k->style->addItem(QIcon(THEME_DIR + "icons/line_style05.png"), tr("Dash dot dot"), QVariant(flag));
    
    addChild(k->style);
    connect(k->style, SIGNAL(currentIndexChanged(int)), this, SLOT(setStyle(int)));

    space = new QWidget(this);
    space->setFixedHeight(5);
    addChild(space);

    label = new QLabel(tr("Cap") + ":", this);
    addChild(label);
    
    k->capStyle = new QComboBox();
    k->capStyle->setIconSize(QSize(140, 13));
    
    k->capStyle->addItem(QIcon(THEME_DIR + "icons/border01.png"), tr("Flat"), Qt::FlatCap);
    k->capStyle->addItem(QIcon(THEME_DIR + "icons/border02.png"), tr("Square"), Qt::SquareCap);
    k->capStyle->addItem(QIcon(THEME_DIR + "icons/border03.png"), tr("Round"), Qt::RoundCap);
    
    addChild(k->capStyle);
    connect(k->capStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(setCapStyle(int)));

    space = new QWidget(this);
    space->setFixedHeight(5);
    addChild(space);

    label = new QLabel(tr("Join") + ":", this);
    addChild(label);
    
    k->joinStyle = new QComboBox();
    
    k->joinStyle->addItem(tr("Miter"), Qt::MiterJoin);
    k->joinStyle->addItem(tr("Bevel"), Qt::BevelJoin);
    k->joinStyle->addItem(tr("Round"), Qt::RoundJoin);
    
    addChild(k->joinStyle);
    connect(k->joinStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(setJoinStyle(int)));

    space = new QWidget(this);
    space->setFixedHeight(5);
    addChild(space);

    label = new QLabel(tr("Brush") + ":", this);
    addChild(label);

    addBrushesList();
 
    boxLayout()->addStretch(2);
    
    setWindowIcon(QIcon(THEME_DIR + "icons/brush.png"));
}

TupPenWidget::~TupPenWidget()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    delete k;
}

void TupPenWidget::setThickness(int value)
{
    if (value > 0) {
        k->pen.setWidth(value);
        // updatePenParams();

        TCONFIG->beginGroup("PenParameters");
        TCONFIG->setValue("Thickness", value);

        emitPenChanged();
    }
}

void TupPenWidget::setStyle(int s)
{
    k->pen.setStyle(Qt::PenStyle(k->style->itemData(s).toInt()));
    // updatePenParams();

    emitPenChanged();
}

void TupPenWidget::setJoinStyle(int s)
{
    k->pen.setJoinStyle(Qt::PenJoinStyle(k->joinStyle->itemData(s).toInt()));
    // updatePenParams();

    emitPenChanged();
}

void TupPenWidget::setCapStyle(int s)
{
    k->pen.setCapStyle(Qt::PenCapStyle(k->capStyle->itemData(s).toInt()));
    // updatePenParams();

    emitPenChanged();
}

void TupPenWidget::setBrushStyle(QListWidgetItem *item)
{
    int index = k->brushesList->row(item);
 
    if (item->toolTip().compare("TexturePattern") == 0) {
        k->brush = QBrush(QPixmap(THEME_DIR + "icons/brush_15.png"));
        k->thickPreview->setBrush(24);
    } else {
        k->thickPreview->setBrush(index+1);
        k->brush.setStyle(Qt::BrushStyle(index+1));
    }

    // updatePenParams(); 

    emitPenChanged();
}

void TupPenWidget::setPenColor(const QColor color)
{
    k->brush.setColor(color);
    k->thickPreview->setColor(color);
}

void TupPenWidget::setBrush(const QBrush brush)
{
    k->brush = brush;
    k->thickPreview->setBrush(brush);

    emitPenChanged();
    // emitBrushChanged();
}

void TupPenWidget::init()
{
    setPenColor(QColor(0, 0, 0));

    k->capStyle->setCurrentIndex(2);
    k->joinStyle->setCurrentIndex(2);
    k->style->setCurrentIndex(0);

    QListWidgetItem *first = k->brushesList->item(0);
    k->brushesList->setCurrentItem(first);
    setBrushStyle(first);
}

QPen TupPenWidget::pen() const
{
    return k->pen;
}

void TupPenWidget::emitPenChanged()
{
    k->pen.setBrush(k->brush);
    emit penChanged(k->pen);

    TupPaintAreaEvent event(TupPaintAreaEvent::ChangePen, k->pen);
    emit paintAreaEventTriggered(&event);
}

void TupPenWidget::emitBrushChanged()
{
    //emit brushChanged(k->pen.brush());
    emit brushChanged(k->brush);

    TupPaintAreaEvent event(TupPaintAreaEvent::ChangeBrush, k->brush);
    emit paintAreaEventTriggered(&event);
}

/*
void TupPenWidget::updatePenParams()
{
    k->pen.setBrush(k->brush);
}
*/

void TupPenWidget::addBrushesList()
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
