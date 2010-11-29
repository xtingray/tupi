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

#include "ktframestable.h"

#include <QPainter>
#include <QPaintEvent>
#include <QItemSelectionModel>
#include <QPainterPath>
#include <QScrollBar>
#include <QHeaderView>

#include "kdebug.h"
#include "kttlruler.h"
//#include "ktprojectrequest.h"
#include "ktprojectactionbar.h"

////////// KTFramesTableItemDelegate ///////////

class KTFramesTableItemDelegate : public QAbstractItemDelegate
{
    public:
        KTFramesTableItemDelegate(QObject * parent = 0);
        ~KTFramesTableItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
        virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

KTFramesTableItemDelegate::KTFramesTableItemDelegate(QObject * parent) : QAbstractItemDelegate(parent)
{
}

KTFramesTableItemDelegate::~KTFramesTableItemDelegate()
{
}

void KTFramesTableItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    Q_ASSERT(index.isValid());
    
    KTFramesTable *table = qobject_cast<KTFramesTable *>(index.model()->parent());
    KTFramesTableItem *item = dynamic_cast<KTFramesTableItem *>(table->itemFromIndex(index));
    
    QVariant value;
    QStyleOptionViewItem opt = option;
    
    // draw the background color
    value = index.data(Qt::BackgroundColorRole);
    
    if (value.isValid()) {

        painter->save();
        
        bool sound = table->isSoundLayer(index.row());
        
        if (!sound)
            painter->fillRect(option.rect, value.value<QColor>());
        
        painter->restore();

    } else {

        painter->save();
        
        bool sound = table->isSoundLayer(index.row());
        
        if (!sound) {
            int counter = index.column() + 1;
            if (counter == 1 || counter % 5 == 0) 
                painter->fillRect(option.rect, QColor(230, 230, 230));
            else 
                painter->fillRect(option.rect, Qt::white);
        } else {

        }
        
        painter->restore();
    }
    
    // Selection!
    if (option.showDecorationSelected && (option.state & QStyle::State_Selected)) {

        QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
        painter->save();
        painter->setPen(QPen(option.palette.brush(cg, QPalette::Highlight), 3));
        painter->fillRect(option.rect, QColor(250, 209, 132, 255));
        painter->restore();

    }
    
    // Draw attributes
    
    int offset = option.rect.width() - 5;
    
    if (item && index.isValid()) {

        if (item->isUsed()) {
            painter->save();
            painter->setBrush(Qt::black);
            painter->setRenderHint(QPainter::Antialiasing, true);
            
            if (!item->isSound()) {
                painter->drawEllipse(option.rect.x() + ((option.rect.width() - offset)/2), 
                                     option.rect.y() + ((option.rect.height() + offset)/2), 
                                     offset, offset);
            } else {
                painter->setBrush(Qt::blue);
                painter->drawRect(option.rect.left(), option.rect.bottom() - offset, offset - 2, offset - 2);
            }
            
            painter->restore();
        }
        
        if (item->isLocked()) {
            painter->save();
            painter->setBrush(Qt::red);
            painter->drawEllipse(option.rect.left(), option.rect.bottom() - offset, offset, offset);
            painter->restore();
        }
    }
}

QSize KTFramesTableItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    Q_ASSERT(index.isValid());
    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);

    QVariant value = model->data(index, Qt::FontRole);
    QFont fnt = value.isValid() ? qvariant_cast<QFont>(value) : option.font;
    QString text = model->data(index, Qt::DisplayRole).toString();
    QRect pixmapRect;
    if (model->data(index, Qt::DecorationRole).isValid())
        pixmapRect = QRect(0, 0, option.decorationSize.width(), option.decorationSize.height());

    QFontMetrics fontMetrics(fnt);
    
    return (pixmapRect).size();
}

////////// KTFramesTableItem ////////

KTFramesTableItem::KTFramesTableItem()
{
}

KTFramesTableItem::~KTFramesTableItem()
{
}

bool KTFramesTableItem::isUsed()
{
    return data(IsUsed).toBool();
}

bool KTFramesTableItem::isLocked()
{
    return data(IsLocked).toBool();
}

bool KTFramesTableItem::isSound()
{
    QVariant data = this->data(IsSound);
    
    if (data.canConvert<bool>()) {
        return data.toBool();
    }

    return false;
}

//// KTFramesTable

struct KTFramesTable::Private
{
    struct LayerItem {
        LayerItem() : lastItem(-1), sound(false) {};
        int lastItem;
        bool sound;
    };
    
    int rectWidth;
    int rectHeight;
    int sceneIndex;
    int layerIndex;
    int frameIndex;
    QList<LayerItem> layers;
    KTTLRuler *ruler;
};

KTFramesTable::KTFramesTable(int sceneIndex, QWidget *parent) : QTableWidget(0, 100, parent), k(new Private)
{
    k->sceneIndex = sceneIndex;
    k->frameIndex = 0;
    k->layerIndex = 0;
    k->ruler = new KTTLRuler;
    setup();
}

KTFramesTable::~KTFramesTable()
{
    delete k;
}

void KTFramesTable::setup()
{
    setItemDelegate(new KTFramesTableItemDelegate(this));
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
    
    setHorizontalHeader(k->ruler);

    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(emitRequestSelectFrame(int, int, int, int)));

    connect(k->ruler, SIGNAL(logicalSectionSelected(int)), this, SLOT(emitFrameSelected(int)));

    // connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(emitFrameSelectionChanged()));

    connect(this, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, 
            SLOT(emitFrameSelected(QTableWidgetItem *, QTableWidgetItem *)));

    verticalHeader()->hide();
    
    setItemSize(10, 25);
    
    horizontalHeader()->setResizeMode(QHeaderView::Custom);
    verticalHeader()->setResizeMode(QHeaderView::Custom);
}

void KTFramesTable::emitFrameSelected(int col)
{
    selectColumn(col);
    
    KTFramesTableItem *item = dynamic_cast<KTFramesTableItem *>(this->item(currentRow(), col));
    
    if (item) {
        if (item->isUsed())
            emit emitRequestChangeFrame(k->sceneIndex, verticalHeader()->visualIndex(this->row(item)), this->column(item));
            //emit frameRequest(KTProjectActionBar::SelectFrame, this->column(item), verticalHeader()->visualIndex(this->row(item)), -1);
    } 
}

/*
void KTFramesTable::emitFrameSelectionChanged()
{
    KTFramesTableItem *item = dynamic_cast<KTFramesTableItem *>(this->item(currentRow(), currentColumn()));

    if (!item)
        emit frameRequest(KTProjectActionBar::InsertFrame, currentColumn(), currentRow(), k->sceneIndex);
}
*/

void KTFramesTable::emitFrameSelected(QTableWidgetItem *current, QTableWidgetItem *prev)
{
    KTFramesTableItem *item = dynamic_cast<KTFramesTableItem *>(current);
    
    if (item) {
        if (item->isUsed())
            emit emitRequestChangeFrame(k->sceneIndex, verticalHeader()->visualIndex(this->row(item)), this->column(item));
            // emit frameRequest(KTProjectActionBar::SelectFrame, this->column(item), verticalHeader()->visualIndex(this->row(item)), -1);
        else
            kFatal() << "KTFramesTable::emitFrameSelected <- item exists but isn't used right now";
    } else { 
        emit frameRequest(KTProjectActionBar::InsertFrame, currentColumn(), currentRow(), k->sceneIndex);
    }
}

void KTFramesTable::setItemSize(int w, int h)
{
    k->rectHeight = h;
    k->rectWidth = w;
    
    fixSize();
}

bool KTFramesTable::isSoundLayer(int row)
{
    if (row < 0 && row >= k->layers.count())
        return false;
    
    return k->layers[row].sound;
}

void KTFramesTable::insertLayer(int pos, const QString &name)
{
    insertRow(pos);
    
    Private::LayerItem layer;
    layer.sound = false;
    k->layers.insert(pos, layer);
    
    fixSize();
}

void KTFramesTable::insertSoundLayer(int layerPos, const QString &name)
{
    insertRow(layerPos);
    
    Private::LayerItem layer;
    layer.sound = true;
    k->layers.insert(layerPos, layer);
    
    fixSize();
}

void KTFramesTable::removeCurrentLayer()
{
    int pos = verticalHeader()->logicalIndex(currentRow());
    removeLayer(pos);
}

void KTFramesTable::removeLayer(int pos)
{
    pos = verticalHeader()->logicalIndex(pos);
    removeRow( pos );
    k->layers.removeAt(pos);
}

void KTFramesTable::moveLayer(int position, int newPosition)
{
    if (position < 0 || position >= rowCount() || newPosition < 0 || newPosition >= rowCount()) 
        return;
    
    blockSignals(true);
    
    verticalHeader()->moveSection(position, newPosition);
    
    blockSignals(false);

    /*
    
     KTFramesTableItem *item1 = takeItem(position, 0);
     
     bool up = true;
     if (position > newPosition) {
         up = false; // down
     }
     
     if (up) {
         for (int i = position+1; i <= newPosition; i++) {
              setItem(i-1, 0, takeItem(i, 0));
         }
     } else {
         for (int i = position-1;i >= newPosition;i--) {
              setItem(i+1, 0, takeItem(i, 0));
         }
     }
     
     setItem(newPosition, 0, item1);
     
     setCurrentItem(item1);
    */
}

int KTFramesTable::lastFrameByLayer(int layerPos)
{
    int pos = verticalHeader()->logicalIndex(layerPos);

    if (pos < 0 || pos > k->layers.count()) {
        return -1;
    }

    return k->layers[pos].lastItem;
}

// FRAMES


void KTFramesTable::insertFrame(int layerPos, const QString &name)
{
    if (layerPos < 0 || layerPos >= k->layers.count()) 
        return;
    
    layerPos = verticalHeader()->logicalIndex(layerPos);
    
    k->layers[layerPos].lastItem++;
    
    if (k->layers[layerPos].lastItem >= columnCount())
        insertColumn(k->layers[layerPos].lastItem);
    
    setAttribute(layerPos, k->layers[layerPos].lastItem, KTFramesTableItem::IsUsed, true);
    setAttribute(layerPos, k->layers[layerPos].lastItem, KTFramesTableItem::IsSound, k->layers[layerPos].sound);
    
    viewport()->update();
}

void KTFramesTable::setCurrentFrame(KTFramesTableItem *item)
{
    setCurrentItem(item);
}

void KTFramesTable::setCurrentLayer(int layerPos)
{
    setCurrentItem(item(verticalHeader()->logicalIndex(layerPos), 0));
}

void KTFramesTable::selectFrame(int index)
{
    setCurrentItem(item(currentRow(), index));
}

void KTFramesTable::removeFrame(int layerPos, int position)
{
    /*
    for (int frameIndex = position; frameIndex < columnCount(); frameIndex++)
         setAttribute( layerPos, position, KTFramesTableItem::IsUsed, false);
    */
    
    if (layerPos < 0 || layerPos >= k->layers.count())
        return;
    
    layerPos = verticalHeader()->logicalIndex(layerPos);
    setAttribute(layerPos, k->layers[layerPos].lastItem, KTFramesTableItem::IsUsed, false);
    k->layers[layerPos].lastItem--;
    viewport()->update();
}

void KTFramesTable::lockFrame(int layerPos, int position, bool lock)
{
    if (layerPos < 0 || layerPos >= k->layers.count())
        return;
    
    layerPos = verticalHeader()->logicalIndex(layerPos);
    setAttribute(layerPos, position, KTFramesTableItem::IsLocked, lock);
    viewport()->update();
}

void KTFramesTable::setAttribute(int row, int col, KTFramesTableItem::Attributes att, bool value)
{
    QTableWidgetItem *item = this->item(row, col);
    
    if (!item) {
        item = new KTFramesTableItem;
        setItem(row, col, item);
    }
    
    item->setData(att, value);
}

void KTFramesTable::fixSize()
{
    for (int column = 0; column < columnCount(); column++) {
        horizontalHeader()->resizeSection(column, k->rectWidth);
    }

    for (int row = 0; row < rowCount(); row++) {
         verticalHeader()->resizeSection(row, k->rectHeight);
    }
}

/*
void KTFramesTable::fixSectionMoved(int logical, int visual, int newVisual)
{
     verticalHeader()->moveSection(newVisual, visual);
}
*/

void KTFramesTable::emitRequestSelectFrame(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
     if (k->frameIndex != currentColumn || k->layerIndex != currentRow) {
         k->frameIndex = currentColumn;
         k->layerIndex = currentRow;
         emit emitSelection(currentRow, currentColumn);
     }
}

