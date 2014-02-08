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

#include "tuplayerindex.h"
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

const int LAYER_COLUMN = 0;

// Header

class TupLayerIndexHeader : public QHeaderView
{
    public:
        TupLayerIndexHeader(QWidget * parent = 0);
        ~TupLayerIndexHeader();
        void paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const;

    /*         
    private:
        QPixmap m_lockIcon;
        QPixmap m_viewIcon;
    */
};

TupLayerIndexHeader::TupLayerIndexHeader(QWidget * parent) : QHeaderView(Qt::Horizontal , parent)
{
    setClickable(true);
    setCascadingSectionResizes(true);
    //setMaximumHeight(20);
    setFixedHeight(26);
}

TupLayerIndexHeader::~TupLayerIndexHeader()
{
}

void TupLayerIndexHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
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
    
    //painter->drawRect(rect.normalized().adjusted(0, 1, 0, -1));
    
    QString text = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();;
    
    //QFontMetrics fm(painter->font());
    QFont label("Arial", 9, QFont::Bold, false); 
    QFontMetrics fm(label);
    
    int x = rect.x() + (sectionSize(logicalIndex) - fm.width( text ))/2;
    //int y = fm.height() + (rect.y() / 2);
    int y = 17;
    painter->setFont(label); 
    painter->drawText(x, y, text);
}

// DELEGATE

class TupLayerIndexItemDelegate : public QItemDelegate
{
    public:
        TupLayerIndexItemDelegate(QObject * parent = 0);
        ~TupLayerIndexItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

TupLayerIndexItemDelegate::TupLayerIndexItemDelegate(QObject * parent) :  QItemDelegate(parent)
{
}

TupLayerIndexItemDelegate::~TupLayerIndexItemDelegate()
{
}

void TupLayerIndexItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QItemDelegate::paint(painter, option, index);

    TupLayerIndex *table = qobject_cast<TupLayerIndex *>(index.model()->parent());
    QTableWidgetItem *item = table->itemFromIndex(index);

    if (item) {
        if (item->isSelected()) {
            painter->setPen(QPen(QColor(255, 190, 31, 255), 1));
            painter->drawRect(option.rect.normalized().adjusted(1,1,-2, -2));
        } 
    }
}

////////////////////////////////

struct TupLayerIndex::Private
{
    Private() : allSelected(false), allVisible(true), allLock(false), rowHeight(20), sceneIndex(0) {}
    
    bool allSelected, allVisible, allLock;
    int rowHeight;
    int sceneIndex;
};

TupLayerIndex::TupLayerIndex(int sceneIndex, QWidget *parent) : QTableWidget(0, 1, parent), k(new Private)
{
    #ifdef K_DEBUG
        TINIT;
    #endif

    k->sceneIndex = sceneIndex;
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
    
    QTableWidgetItem *prototype = new QTableWidgetItem;
    prototype->setTextAlignment(Qt::AlignCenter);
    prototype->setBackgroundColor(palette().text().color());
    prototype->setTextColor(palette().background().color());
    
    setItemPrototype(prototype);
    
    setHorizontalHeaderLabels(QStringList() << tr("Layers"));
    verticalHeader()->hide();
    
    setHorizontalHeader(new TupLayerIndexHeader(this));
    setItemDelegate(new TupLayerIndexItemDelegate(this));

    connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(setLocalRequest(int, int)));
}

TupLayerIndex::~TupLayerIndex()
{
    #ifdef K_DEBUG
        TEND;
    #endif

    delete k;
}

void TupLayerIndex::insertLayer(int position, const QString &name)
{
    if (position >= 0 && position <= rowCount()) {

        QTableWidgetItem *newLayer = new QTableWidgetItem(name);
        newLayer->setFont(QFont("Arial", 8, QFont::Normal, false));
        newLayer->setTextAlignment(Qt::AlignCenter);
        
        newLayer->setBackgroundColor(palette().background().color());
        newLayer->setTextColor(palette().foreground().color());
        
        insertRow(position);

        setItem(position, 0, newLayer);
        fixSize();
    }
}

void TupLayerIndex::insertSoundLayer(int position, const QString &name)
{
    if (position >= 0 && position <= rowCount()) {
        QTableWidgetItem *newLayer = new QTableWidgetItem(name);
        newLayer->setTextAlignment(Qt::AlignCenter);
        
        newLayer->setBackgroundColor(palette().background().color());
        newLayer->setTextColor(palette().foreground().color());
        
        insertRow(position);
        setItem(position, 0, newLayer);
        fixSize();
    }
}

void TupLayerIndex::removeLayer(int position)
{
    removeRow(verticalHeader()->logicalIndex(position));
}

void TupLayerIndex::renameLayer(int position, const QString &name)
{
    QTableWidgetItem *item = this->item(0, verticalHeader()->logicalIndex(position));
    
    if (item)
        item->setText(name);
}

void TupLayerIndex::resizeEvent(QResizeEvent *)
{
    fixSize();
}

void TupLayerIndex::fixSize()
{
    horizontalHeader()->resizeSection(0, 196);

    for (int row = 0; row < rowCount(); row++)
         verticalHeader()->resizeSection(row, k->rowHeight);
}

void TupLayerIndex::setRowHeight(int rowHeight)
{
    k->rowHeight = rowHeight;
}

void TupLayerIndex::commitData(QWidget *editor)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    
    QTableWidget::commitData(0); // Don't rename
    
    if (lineEdit)
        emit requestRenameEvent(verticalHeader()->visualIndex(currentRow()), lineEdit->text());
}

void TupLayerIndex::moveLayer(int position, int newPosition)
{
    if (position < 0 || position >= rowCount() || newPosition < 0 || newPosition >= rowCount()) 
        return;
    
    verticalHeader()->moveSection((position), (newPosition));
}


void TupLayerIndex::lockLayer(int position, bool locked)
{
    if (position < 0 || position >= rowCount()) 
        return;
}

void TupLayerIndex::setLocalRequest(int layer, int column)
{
    emit localRequest();
}

void TupLayerIndex::keyPressEvent(QKeyEvent *event)
{
    int row = currentRow();
    int total = rowCount();

    if (event->key() == Qt::Key_Up) {
        if (row > 0) {
            setCurrentCell(row - 1, 0);
            emit localRequest();
        }
    }

    if (event->key() == Qt::Key_Down) {
        if (row < total-1) {
            setCurrentCell(row + 1, 0);
            emit localRequest();
        }
    }
}

void TupLayerIndex::refresh()
{
    // int w = width();
    // int h = height();
    // tFatal() << "Updating TupLayerIndex interface - Size: " << w << " : " << h;
    //repaint(0, 0, width(), height());
}
