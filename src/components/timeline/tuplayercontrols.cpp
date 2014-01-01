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

#include "tuplayercontrols.h"
#include "tuprequestbuilder.h"

#include <qlabel.h>
#include <qlayout.h>

#include <QPixmap>
#include <QHeaderView>
#include <QPainter>
#include <QItemDelegate>

#include "tseparator.h"
#include "tapplication.h"
#include "tdebug.h"

const int LOCK_COLUMN = 0;
const int VIEW_COLUMN = 1;

// Header

class TupLayerControlsHeader : public QHeaderView
{
    public:
        TupLayerControlsHeader(QWidget * parent = 0);
        ~TupLayerControlsHeader();
        void paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const;
         
    private:
        QPixmap m_lockIcon;
        QPixmap m_viewIcon;
};

TupLayerControlsHeader::TupLayerControlsHeader(QWidget * parent) : QHeaderView(Qt::Horizontal , parent)
{
    // setClickable(true);
    setCascadingSectionResizes(true);
    setFixedHeight(26);
    
    m_lockIcon = QPixmap(THEME_DIR + "icons/padlock.png");
    m_viewIcon = QPixmap(THEME_DIR + "icons/show_hide_all_layers.png");
}

TupLayerControlsHeader::~TupLayerControlsHeader()
{
}

void TupLayerControlsHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
    if (!rect.isValid()) 
        return;
    
    QStyleOptionHeader headerOption;
    headerOption.rect = rect;
    headerOption.orientation = Qt::Horizontal;
    headerOption.position = QStyleOptionHeader::Middle;
    
    QStyle::State state = QStyle::State_None;

    if (isEnabled())
        state |= QStyle::State_Enabled;

    if (window()->isActiveWindow())
        state |= QStyle::State_Active;
    
    style()->drawControl(QStyle::CE_HeaderSection, &headerOption, painter);
    
    switch (logicalIndex) {
            case LOCK_COLUMN:
            {
                 painter->drawPixmap(QPointF(rect.x() + (rect.width()-10)/2, rect.y() + 6), m_lockIcon, QRectF(0, 0, 10, 13));
            }
            break;
            case VIEW_COLUMN:
            {
                 painter->drawPixmap(QPointF(rect.x() + (rect.width()-16)/2, rect.y() + 5), m_viewIcon, QRectF(0, 0, 16, 16));
            }
            break;
    }
}

// DELEGATE

class TupLayerControlsItemDelegate : public QItemDelegate
{
    public:
        TupLayerControlsItemDelegate(QObject * parent = 0);
        ~TupLayerControlsItemDelegate();
        virtual void drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &, Qt::CheckState state) const;
        virtual void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

TupLayerControlsItemDelegate::TupLayerControlsItemDelegate(QObject * parent) :  QItemDelegate(parent)
{
}

TupLayerControlsItemDelegate::~TupLayerControlsItemDelegate()
{
}

void TupLayerControlsItemDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option,
                                           const QRect &, Qt::CheckState state) const
{
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

    /*
    QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                          check(option, option.rect, Qt::Checked).size(),
                                          QRect(option.rect.x() + textMargin, option.rect.y(),
                                          option.rect.width() - (textMargin * 2), option.rect.height()));
    QItemDelegate::drawCheck(painter, option, checkRect, state);
    */
}

void TupLayerControlsItemDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
}

void TupLayerControlsItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QItemDelegate::paint(painter, option, index);
}

////////////////////////////////

struct TupLayerControls::Private
{
    Private() : allSelected(false), allVisible(true), allLock(false), rowHeight(20), sceneIndex(0) {}
    
    bool allSelected, allVisible, allLock;
    int rowHeight;
    int sceneIndex;
};

TupLayerControls::TupLayerControls(int sceneIndex, QWidget *parent) : QTableWidget(0, 2, parent), k(new Private)
{
    #ifdef K_DEBUG
        TINIT;
    #endif

    setItemDelegate(new TupLayerControlsItemDelegate(this));

    k->sceneIndex = sceneIndex;
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QTableWidgetItem *prototype = new QTableWidgetItem;
    prototype->setTextAlignment(Qt::AlignCenter);
    prototype->setBackgroundColor(palette().text().color());
    prototype->setTextColor(palette().background().color());
    
    setItemPrototype(prototype);
    setHorizontalHeader(new TupLayerControlsHeader(this));  
    setHorizontalHeaderLabels(QStringList() << tr("") << tr(""));
    verticalHeader()->hide();
    
    setHorizontalHeader(new TupLayerControlsHeader(this));

    connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(setLocalRequest(int, int)));
}

TupLayerControls::~TupLayerControls()
{
    #ifdef K_DEBUG
        TEND;
    #endif

    delete k;
}

void TupLayerControls::insertLayer(int position)
{
    if (position >= 0 && position <= rowCount()) {

        insertRow(position);

        QTableWidgetItem *lockItem = new QTableWidgetItem;
        lockItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        lockItem->setCheckState(Qt::Unchecked);
        
        setItem(position, 0, lockItem);
        
        QTableWidgetItem *viewItem = new QTableWidgetItem;
        viewItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        viewItem->setCheckState(Qt::Checked);
        
        setItem(position, 1, viewItem);

        fixSize();
    }
}

void TupLayerControls::insertSoundLayer(int position, const QString &name)
{
    if (position >= 0 && position <= rowCount()) {
        fixSize();
    }
}

void TupLayerControls::removeLayer(int position)
{
    removeRow(verticalHeader()->logicalIndex(position));
}

void TupLayerControls::renameLayer(int position, const QString &name)
{
    QTableWidgetItem *item = this->item(0, verticalHeader()->logicalIndex(position));
    
    if (item)
        item->setText(name);
}

void TupLayerControls::resizeEvent(QResizeEvent *)
{
    fixSize();
}

void TupLayerControls::fixSize()
{
    int offset = 0;

    if (verticalScrollBar()->isVisible())
        offset = verticalScrollBar()->width()-2;
    else
        offset = -2;

    int width = this->width() - offset;
    horizontalHeader()->resizeSection(0, 20);
    horizontalHeader()->resizeSection(1, 20);

    for (int row = 0; row < rowCount(); row++)
         verticalHeader()->resizeSection(row, k->rowHeight);
}

void TupLayerControls::setRowHeight(int rowHeight)
{
    k->rowHeight = rowHeight;
}

void TupLayerControls::commitData(QWidget *editor)
{
}

void TupLayerControls::moveLayer(int position, int newPosition)
{
    if (position < 0 || position >= rowCount() || newPosition < 0 || newPosition >= rowCount()) 
        return;
    
    verticalHeader()->moveSection((position), (newPosition));
}

void TupLayerControls::lockLayer(int position, bool locked)
{
    if (position < 0 || position >= rowCount()) 
        return;
}

void TupLayerControls::setLocalRequest(int layerIndex, int column)
{
    if (column == 0) {
        tFatal() << "TupLayerControls::setLocalRequest -> Column 0!";
    }

    if (column == 1) {
        QTableWidgetItem *item = this->item(verticalHeader()->logicalIndex(layerIndex), 1);

        bool checked = false;
        if (item->checkState() == Qt::Checked)
            checked = true;

        emit layerVisibility(k->sceneIndex, layerIndex, checked);
    }

    emit localRequest();
}

void TupLayerControls::setLayerVisibility(int layerIndex, const QString &isChecked)
{
    QTableWidgetItem *item = this->item(verticalHeader()->logicalIndex(layerIndex), 1);

    if (isChecked.compare("true")==0) 
        item->setCheckState(Qt::Checked);
    else
        item->setCheckState(Qt::Unchecked);
}
