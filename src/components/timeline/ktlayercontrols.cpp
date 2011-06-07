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

#include "ktlayercontrols.h"
#include "ktrequestbuilder.h"

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

class KTLayerControlsHeader : public QHeaderView
{
    public:
        KTLayerControlsHeader(QWidget * parent = 0);
        ~KTLayerControlsHeader();
        void paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const;
         
    private:
        QPixmap m_lockIcon;
        QPixmap m_viewIcon;
};

KTLayerControlsHeader::KTLayerControlsHeader(QWidget * parent) : QHeaderView(Qt::Horizontal , parent)
{
    setClickable(true);
    setCascadingSectionResizes(true);
    setFixedHeight(26);
    
    m_lockIcon = QPixmap(THEME_DIR + "icons/padlock.png");
    m_viewIcon = QPixmap(THEME_DIR + "icons/show_hide_all_layers.png");
}

KTLayerControlsHeader::~KTLayerControlsHeader()
{
}

void KTLayerControlsHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
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

class KTLayerControlsItemDelegate : public QItemDelegate
{
    public:
        KTLayerControlsItemDelegate(QObject * parent = 0);
        ~KTLayerControlsItemDelegate();
        virtual void drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &, Qt::CheckState state) const;
        virtual void drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

KTLayerControlsItemDelegate::KTLayerControlsItemDelegate(QObject * parent) :  QItemDelegate(parent)
{
}

KTLayerControlsItemDelegate::~KTLayerControlsItemDelegate()
{
}

void KTLayerControlsItemDelegate::drawCheck(QPainter *painter, const QStyleOptionViewItem &option,
                                           const QRect &, Qt::CheckState state) const
{
    const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

    QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                          check(option, option.rect, Qt::Checked).size(),
                                          QRect(option.rect.x() + textMargin, option.rect.y(),
                                          option.rect.width() - (textMargin * 2), option.rect.height()));
    QItemDelegate::drawCheck(painter, option, checkRect, state);
}

void KTLayerControlsItemDelegate::drawFocus(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const
{
}

void KTLayerControlsItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QItemDelegate::paint(painter, option, index);
}

////////////////////////////////

struct KTLayerControls::Private
{
    Private() : allSelected(false), allVisible(true), allLock(false), rowHeight(20), sceneIndex(0) {}
    
    bool allSelected, allVisible, allLock;
    int rowHeight;
    int sceneIndex;
};

KTLayerControls::KTLayerControls(int sceneIndex, QWidget *parent) : QTableWidget(0, 2, parent), k(new Private)
{
    #ifdef K_DEBUG
        TINIT;
    #endif

    setItemDelegate(new KTLayerControlsItemDelegate(this));

    k->sceneIndex = sceneIndex;
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QTableWidgetItem *prototype = new QTableWidgetItem;
    prototype->setTextAlignment(Qt::AlignCenter);
    prototype->setBackgroundColor(palette().text().color());
    prototype->setTextColor(palette().background().color());
    
    setItemPrototype(prototype);
    setHorizontalHeader(new KTLayerControlsHeader(this));  
    setHorizontalHeaderLabels(QStringList() << tr("") << tr(""));
    verticalHeader()->hide();
    
    setHorizontalHeader(new KTLayerControlsHeader(this));

    connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(setLocalRequest(int, int)));
}

KTLayerControls::~KTLayerControls()
{
    #ifdef K_DEBUG
        TEND;
    #endif

    delete k;
}

void KTLayerControls::insertLayer(int position)
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

void KTLayerControls::insertSoundLayer(int position, const QString &name)
{
    if (position >= 0 && position <= rowCount()) {
        fixSize();
    }
}

void KTLayerControls::removeLayer(int position)
{
    removeRow(verticalHeader()->logicalIndex(position));
}

void KTLayerControls::renameLayer(int position, const QString &name)
{
    QTableWidgetItem *item = this->item(0, verticalHeader()->logicalIndex(position));
    
    if (item)
        item->setText(name);
}

void KTLayerControls::resizeEvent(QResizeEvent *)
{
    fixSize();
}

void KTLayerControls::fixSize()
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

void KTLayerControls::setRowHeight(int rowHeight)
{
    k->rowHeight = rowHeight;
}

void KTLayerControls::commitData(QWidget *editor)
{
}

void KTLayerControls::moveLayer(int position, int newPosition)
{
    if (position < 0 || position >= rowCount() || newPosition < 0 || newPosition >= rowCount()) 
        return;
    
    verticalHeader()->moveSection((position), (newPosition));
}

void KTLayerControls::lockLayer(int position, bool locked)
{
    if (position < 0 || position >= rowCount()) 
        return;
}

void KTLayerControls::setLocalRequest(int layerIndex, int column)
{
    if (column == 0) {
        tFatal() << "KTLayerControls::setLocalRequest -> Column 0!";
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

void KTLayerControls::setLayerVisibility(int layerIndex, const QString &isChecked)
{
    QTableWidgetItem *item = this->item(verticalHeader()->logicalIndex(layerIndex), 1);

    if (isChecked.compare("true")==0) 
        item->setCheckState(Qt::Checked);
    else
        item->setCheckState(Qt::Unchecked);
}
