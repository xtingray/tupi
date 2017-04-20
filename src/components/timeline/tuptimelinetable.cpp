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

#include "tuptimelinetable.h"
#include "tconfig.h"

////////// TupTimeLineTableItemDelegate ///////////

class TupTimeLineTableItemDelegate : public QItemDelegate
{
    public:
        TupTimeLineTableItemDelegate(QObject * parent = 0);
        ~TupTimeLineTableItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

    private:
        struct Private;
        Private *const k;
};

struct TupTimeLineTableItemDelegate::Private
{
    QString themeName;
};

TupTimeLineTableItemDelegate::TupTimeLineTableItemDelegate(QObject *parent) : QItemDelegate(parent), k(new Private) 
{
    TCONFIG->beginGroup("General");
    k->themeName = TCONFIG->value("Theme", "Light").toString();
}

TupTimeLineTableItemDelegate::~TupTimeLineTableItemDelegate()
{
}

void TupTimeLineTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_ASSERT(index.isValid());

    QItemDelegate::paint(painter, option, index);
    TupTimeLineTable *table = qobject_cast<TupTimeLineTable *>(index.model()->parent());
    TupTimeLineTableItem *item = dynamic_cast<TupTimeLineTableItem *>(table->itemFromIndex(index));
    
    // draw the background color
    QVariant value = index.data(Qt::BackgroundColorRole);
    
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
        } 
        
        painter->restore();
    }
    
    // Selection!
    if (option.showDecorationSelected && (option.state & QStyle::State_Selected)) {
        painter->save();

        QColor color(0, 136, 0, 180);
        if (k->themeName.compare("Dark") == 0)
            color = QColor(80, 80, 80, 180);

        painter->fillRect(option.rect, color);
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
                if (item->isLocked()) {
                    painter->setPen(QPen(Qt::red, 1, Qt::SolidLine));
                    painter->setBrush(Qt::red);
                } 
                painter->drawEllipse(option.rect.x() + ((option.rect.width() - offset)/2), 
                                     option.rect.y() + ((option.rect.height() + offset)/2), 
                                     offset, offset);
            } else {
                painter->setBrush(QColor(0, 136, 0));
                painter->drawRect(option.rect.x() + ((option.rect.width() - offset)/2), 
                                  option.rect.y() + ((option.rect.height() + offset)/2), 
                                  offset, offset);
            }
            
            painter->restore();
        }
    }
}

////////// TupTimeLineTableItem ////////

TupTimeLineTableItem::TupTimeLineTableItem()
{
}

TupTimeLineTableItem::~TupTimeLineTableItem()
{
}

bool TupTimeLineTableItem::isUsed()
{
    return data(IsUsed).toBool();
}

bool TupTimeLineTableItem::isLocked()
{
    return data(IsLocked).toBool();
}

bool TupTimeLineTableItem::isSound()
{
    QVariant data = this->data(IsSound);
    
    if (data.canConvert<bool>())
        return data.toBool();

    return false;
}

//// TupTimeLineTable

struct TupTimeLineTable::Private
{
    int rectWidth;
    int rectHeight;

    int sceneIndex;
    int layerIndex;
    int frameIndex;

    TupTimeLineRuler *ruler;
    TupTimeLineHeader *layersColumn;

    bool removingLayer;
    bool removingFrame;
    bool isLocalRequest;
};

TupTimeLineTable::TupTimeLineTable(int sceneIndex, QWidget *parent) : QTableWidget(0, 200, parent), k(new Private)
{
    k->isLocalRequest = false;
    k->sceneIndex = sceneIndex;
    k->frameIndex = 0;
    k->layerIndex = 0;

    k->ruler = new TupTimeLineRuler;
    connect(k->ruler, SIGNAL(headerSelectionChanged(int)), this, SLOT(frameSelectionFromRuler(int)));

    k->removingLayer = false;
    k->removingFrame = false;

    k->layersColumn = new TupTimeLineHeader;
    connect(k->layersColumn, SIGNAL(nameChanged(int, const QString &)), this, SIGNAL(layerNameChanged(int, const QString &)));
    connect(k->layersColumn, SIGNAL(headerSelectionChanged(int)), this, SLOT(frameSelectionFromLayerHeader(int)));
    connect(k->layersColumn, SIGNAL(visibilityChanged(int, bool)), this, SIGNAL(visibilityChanged(int, bool)));
    connect(k->layersColumn, SIGNAL(sectionMoved(int, int, int)), this, SLOT(requestLayerMove(int, int, int)));

    setup();
}

TupTimeLineTable::~TupTimeLineTable()
{
    delete k;
}

void TupTimeLineTable::setup()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLineTable::setup()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    setItemDelegate(new TupTimeLineTableItemDelegate(this));
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(requestFrameSelection(int, int, int, int)));

    setHorizontalHeader(k->ruler);
    setVerticalHeader(k->layersColumn);

    setItemSize(10, 25);
    
    k->ruler->setSectionResizeMode(QHeaderView::Custom);
    k->layersColumn->setSectionResizeMode(QHeaderView::Custom);
}

void TupTimeLineTable::frameSelectionFromRuler(int frameIndex)
{
    emit frameSelected(0, frameIndex);
}

void TupTimeLineTable::frameSelectionFromLayerHeader(int layerIndex)
{
    if (k->layersColumn->currentSectionIndex() != layerIndex)
        k->layersColumn->updateSelection(layerIndex);

    emit frameSelected(layerIndex, currentColumn());
}

void TupTimeLineTable::requestLayerMove(int logicalIndex, int oldLayerIndex, int newLayerIndex)
{
    Q_UNUSED(logicalIndex);

    if (!k->layersColumn->sectionIsMoving()) {
        int distance = newLayerIndex - oldLayerIndex;
        if (distance > 0) {
            if (distance > 1)
                newLayerIndex = oldLayerIndex + 1;
        } else {
            distance = abs(distance);
            if (distance > 1)
                newLayerIndex = oldLayerIndex - 1;
        }

        k->isLocalRequest = true;
        emit layerMoved(oldLayerIndex, newLayerIndex);
    }
}

void TupTimeLineTable::setItemSize(int w, int h)
{
    k->rectHeight = h;
    k->rectWidth = w;
    
    fixSize();
}

bool TupTimeLineTable::isSoundLayer(int layerIndex)
{
    if (layerIndex < 0 && layerIndex >= rowCount())
        return false;
    
    return k->layersColumn->isSound(layerIndex);
}

void TupTimeLineTable::insertLayer(int layerIndex, const QString &name)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLineTable::insertLayer()]";
        #else
            T_FUNCINFO << "name -> " << name << " - layerIndex -> " << layerIndex;
        #endif
    #endif

    insertRow(layerIndex);
    k->layersColumn->insertSection(layerIndex, name);
    fixSize();
}

void TupTimeLineTable::insertSoundLayer(int layerIndex, const QString &name)
{
    insertRow(layerIndex);
    k->layersColumn->insertSection(layerIndex, name);
    fixSize();
}

void TupTimeLineTable::removeLayer(int layerIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLineTable::removeLayer()]";
        #else
            T_FUNCINFO << "layerIndex -> " << layerIndex;
        #endif
    #endif

    k->removingLayer = true;
    removeRow(layerIndex);
    k->layersColumn->removeSection(layerIndex);
}

void TupTimeLineTable::moveLayer(int oldLayerIndex, int newLayerIndex)
{
    if (oldLayerIndex < 0 || oldLayerIndex >= rowCount() || newLayerIndex < 0 || newLayerIndex >= rowCount()) 
        return;

    k->layersColumn->moveHeaderSection(oldLayerIndex, newLayerIndex, k->isLocalRequest);
    if (k->isLocalRequest)
        k->isLocalRequest = false;

    int framesTotal = k->layersColumn->lastFrame(oldLayerIndex); 
    for (int frameIndex = 0; frameIndex <= framesTotal; frameIndex++)
         exchangeFrame(frameIndex, oldLayerIndex, frameIndex, newLayerIndex);

    blockSignals(true);
    selectFrame(newLayerIndex, currentColumn());
    blockSignals(false);
}

void TupTimeLineTable::exchangeFrame(int currentFrameIndex, int currentLayerIndex, int newFrameIndex, int newLayerIndex)
{
    QTableWidgetItem *oldItem = takeItem(currentLayerIndex, currentFrameIndex);
    QTableWidgetItem *newItem = takeItem(newLayerIndex, newFrameIndex);

    setItem(newLayerIndex, newFrameIndex, oldItem);
    setItem(currentLayerIndex, currentFrameIndex, newItem);
}

void TupTimeLineTable::setLayerVisibility(int layerIndex, bool isVisible)
{
    if (k->layersColumn)
        k->layersColumn->setSectionVisibility(layerIndex, isVisible);
}

void TupTimeLineTable::setLayerName(int layerIndex, const QString &name)
{
    if (k->layersColumn)
        k->layersColumn->setSectionTitle(layerIndex, name);
}

int TupTimeLineTable::currentLayer()
{
    return currentRow();
}

int TupTimeLineTable::layersCount()
{
    return rowCount();
}

int TupTimeLineTable::lastFrameByLayer(int layerIndex)
{
    if (layerIndex < 0 || layerIndex >= rowCount())
        return -1;

    if (k->layersColumn)
        return k->layersColumn->lastFrame(layerIndex);

    return -1;
}

// FRAMES

void TupTimeLineTable::insertFrame(int layerIndex)
{
    if (layerIndex < 0 || layerIndex >= rowCount())
        return;

    k->layersColumn->updateLastFrame(layerIndex, true);
  
    int lastFrame = k->layersColumn->lastFrame(layerIndex); 

    setAttribute(layerIndex, lastFrame, TupTimeLineTableItem::IsUsed, true);
    setAttribute(layerIndex, lastFrame, TupTimeLineTableItem::IsSound, false);
}

void TupTimeLineTable::restoreFrameSelection(int layerIndex, int frameIndex, const QString &selection)
{
    if (layerIndex < 0 || layerIndex >= rowCount())
        return;

    QStringList params = selection.split(",");
    int layers = params.at(0).toInt();
    int frames = params.at(1).toInt();

    generateFrames(layerIndex, layers, frames);

    blockSignals(true);
    setCurrentItem(item(layerIndex, frameIndex));
    blockSignals(false);

    viewport()->update();
}

void TupTimeLineTable::pasteFrameSelection(int layerIndex, int frameIndex, int layers, int frames)
{
    if (layerIndex < 0 || layerIndex >= rowCount())
        return;

    generateFrames(layerIndex, layers, frames);

    blockSignals(true);
    setCurrentItem(item(layerIndex, frameIndex + (frames - 1)));
    blockSignals(false);

    viewport()->update();
}

void TupTimeLineTable::generateFrames(int layerIndex, int layers, int frames)
{
    int layersTotal = layerIndex + layers;
    for (int i=layerIndex; i<layersTotal; i++) {
         int initFrame = k->layersColumn->lastFrame(i) + 1;
         int framesTotal = initFrame + frames;
         for (int j=initFrame; j<framesTotal; j++) {
              setAttribute(i, j, TupTimeLineTableItem::IsUsed, true);
              k->layersColumn->updateLastFrame(i, true);
         }
    }
}

void TupTimeLineTable::updateLayerHeader(int layerIndex)
{
    k->layersColumn->updateSelection(layerIndex); 
}

void TupTimeLineTable::selectFrame(int frameIndex)
{
    setCurrentItem(item(currentRow(), frameIndex));
}

void TupTimeLineTable::removeFrame(int layerIndex, int frameIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLineTable::removeFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    Q_UNUSED(frameIndex);

    if (layerIndex < 0 || layerIndex >= rowCount())
        return;

    k->removingFrame = true;
    setAttribute(layerIndex, k->layersColumn->lastFrame(layerIndex), TupTimeLineTableItem::IsUsed, false);
    k->layersColumn->updateLastFrame(layerIndex, false);

    viewport()->update();
}

void TupTimeLineTable::removeFrameSelection(int layerIndex, int frameIndex, int layers, int frames)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupTimeLineTable::removeFrameSelection()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (layerIndex < 0 || layerIndex >= rowCount())
        return;

    int layersTotal = layerIndex + layers;
    for (int i=layerIndex; i<layersTotal; i++) {
         int framesTotal = frameIndex + frames;
         if (frames == (k->layersColumn->lastFrame(i) + 1))
             frameIndex = 1;
         for (int j=frameIndex; j<framesTotal; j++) {
              setAttribute(i, k->layersColumn->lastFrame(i), TupTimeLineTableItem::IsUsed, false);
              k->layersColumn->updateLastFrame(i, false);
         }
    }

    int lastIndex = k->layersColumn->lastFrame(layerIndex);
    if (lastIndex < frameIndex)
        frameIndex = lastIndex;

    /*
    blockSignals(true);
    setCurrentItem(item(layerIndex, frameIndex));
    blockSignals(false);
    */

    viewport()->update();
}

/*
void TupTimeLineTable::lockFrame(int layerIndex, int frameIndex, bool lock)
{
    if (layerIndex < 0 || layerIndex >= rowCount())
        return;
    
    setAttribute(layerIndex, frameIndex, TupTimeLineTableItem::IsLocked, lock);
    viewport()->update();
}

bool TupTimeLineTable::frameIsLocked(int layerIndex, int frameIndex)
{
    TupTimeLineTableItem *frame = dynamic_cast<TupTimeLineTableItem *>(item(layerIndex, frameIndex));

    if (frame) {
        return frame->isLocked(); 
    } else {
        #ifdef K_DEBUG
            QString msg = "TupTimeLineTable::frameIsLocked() - Layer: " + QString::number(layerIndex) + QString(", Frame: ") + QString::number(frameIndex) + QString(" doesn't exist");
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }

    return false;
}
*/

void TupTimeLineTable::setAttribute(int layerIndex, int frameIndex, TupTimeLineTableItem::Attributes att, bool value)
{
    QTableWidgetItem *item = this->item(layerIndex, frameIndex);
    if (!item) {
        item = new TupTimeLineTableItem;
        setItem(layerIndex, frameIndex, item);
    }
    
    item->setData(att, value);
}

void TupTimeLineTable::fixSize()
{
    int framesTotal = columnCount();
    for (int column = 0; column < framesTotal; column++)
         horizontalHeader()->resizeSection(column, k->rectWidth);

    int layerTotal = rowCount();
    for (int row = 0; row < layerTotal; row++)
         k->layersColumn->resizeSection(row, k->rectHeight);
}

void TupTimeLineTable::requestFrameSelection(int currentLayerIndex, int currentFrameIndex, int previousLayerIndex, int previousFrameIndex)
{
    if (!k->removingLayer) {
        if (k->removingFrame) {
            k->removingFrame = false;
            emit frameSelected(currentRow(), currentColumn());
            return;
        } else {
            if (previousFrameIndex != currentFrameIndex || previousLayerIndex != currentLayerIndex) 
                emit frameSelected(currentRow(), currentColumn());
        }
    } else { // A layer is being removed
        k->removingLayer = false;

        if (previousLayerIndex != 0) {
            if (previousLayerIndex != rowCount() - 1) {
                blockSignals(true);
                setCurrentItem(item(previousLayerIndex - 1, currentColumn())); 
                k->layersColumn->updateSelection(previousLayerIndex - 1);
                blockSignals(false);
            }
        }
    }
}

void TupTimeLineTable::mousePressEvent(QMouseEvent *event)
{
    int frameIndex = columnAt(event->x());

    int total = columnCount();
    if ((frameIndex >= total - 11) && (frameIndex <= total - 1)) {
        int newTotal = total + 100;
        for (int i=total; i < newTotal; i++)
             insertColumn(i);
        fixSize();
    }

    QTableWidget::mousePressEvent(event);
}

void TupTimeLineTable::keyPressEvent(QKeyEvent *event)
{
    // tError() << "TupTimeLineTable::keyPressEvent() - event->key() -> " << event->key();
    // tError() << "TupTimeLineTable::keyPressEvent() - event->modifiers() -> " << event->modifiers();

    // SQA: Check if this piece of code is obsolete 
    // Fn + Left/Right arrow
    if (event->key() == 16777232 || event->key() == 16777233)
        return;

    if (event->key() == Qt::Key_Backspace) {
        emit frameRemoved();
        return;
    }

    if (event->key() == Qt::Key_Return) {
        emit newPerspective(4);
        return;
    }

    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_PageDown) {
        int limit = columnCount()-1;
        int next = currentColumn()+1;
        if (next <= limit) { 
            if (event->modifiers() == Qt::ControlModifier)
                emit frameCopied(currentRow(), currentColumn());
            else
                setCurrentCell(currentRow(), next);
        }
        return;
    }    

    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_PageUp) {
        int next = currentColumn()-1;
        if (next >= 0) { 
            if (event->modifiers() == Qt::ControlModifier)
                emit frameRemoved();
            else
                setCurrentCell(currentRow(), next);
        } else {
            if (next == -1 && event->modifiers() == Qt::ControlModifier)
                emit frameRemoved();
        }
        return;
    }

    if (event->key() == Qt::Key_Up) {
        int next = currentRow()-1;
        if (next >= 0) 
            setCurrentCell(next, currentColumn());
        return;
    }

    if (event->key() == Qt::Key_Down) {
        int limit = rowCount() - 1;
        int next = currentRow() + 1;
        if (next <= limit)
            setCurrentCell(next, currentColumn());
        return;
    }

    QTableWidget::keyPressEvent(event);
}

void TupTimeLineTable::enterEvent(QEvent *event)
{
    if (!hasFocus())
        setFocus();

    QTableWidget::enterEvent(event);
}

void TupTimeLineTable::leaveEvent(QEvent *event)
{
    if (hasFocus())
        clearFocus();

    QTableWidget::leaveEvent(event);
}

void TupTimeLineTable::selectFrame(int layerIndex, int frameIndex)
{
    blockSignals(true);
    setCurrentCell(layerIndex, frameIndex);
    updateLayerHeader(layerIndex);
    blockSignals(false);
}

void TupTimeLineTable::selectFrame(int layerIndex, int frameIndex, const QString &selection)
{
    blockSignals(true);
    setCurrentCell(layerIndex, frameIndex);
    updateLayerHeader(layerIndex);
    blockSignals(false);

    QStringList coords = selection.split(","); 
    if (coords.count() == 4) {
        int initLayer = coords.at(0).toInt();
        int lastLayer = coords.at(1).toInt(); 
        int initFrame = coords.at(2).toInt();
        int lastFrame = coords.at(3).toInt();

        selectionModel()->clearSelection();

        QModelIndexList indexes;
        for (int i=initLayer; i<=lastLayer; i++) {
            for (int j=initFrame; j<=lastFrame; j++)
                selectionModel()->select(model()->index(i, j), QItemSelectionModel::Select);
        }
    }
}

QList<int> TupTimeLineTable::currentSelection()
{
    QModelIndexList selection = selectedIndexes();
    QList<int> coords;
    QList<int> layers;
    QList<int> frames;

    if (!selection.isEmpty()) {
        foreach (QModelIndex cell, selection) {
            int layer = cell.column();
            int frame = cell.row();

            if (!layers.contains(layer))
                layers << layer;
            if (!frames.contains(frame))
                frames << frame;
        }
        coords << layers.first() << layers.last() << frames.first() << frames.last();
    }

    return coords;
}
