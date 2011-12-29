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

#include "ktexposuretable.h"
#include "ktexposureheader.h"
#include "tdebug.h"
#include "ktglobal.h"

#include <QPainter>
#include <QItemDelegate>
#include <QLineEdit>
#include <QMouseEvent>
#include <QMenu>
#include <QHeaderView>

/**
 * @author Jorge Cuadrado
 */

/////////////////
class KTExposureVerticalHeader : public QHeaderView
{
    //Q_OBJECT

    public:
        KTExposureVerticalHeader(QWidget * parent = 0);
        ~KTExposureVerticalHeader();
        void paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const;
};

KTExposureVerticalHeader::KTExposureVerticalHeader(QWidget * parent) : QHeaderView(Qt::Vertical, parent)
{
    //setClickable(true);
    setFixedWidth(25);
}

KTExposureVerticalHeader::~KTExposureVerticalHeader()
{
}

void KTExposureVerticalHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
    Q_UNUSED(logicalIndex);

    if (!rect.isValid())
        return;

    QStyleOptionHeader headerOption;
    headerOption.rect = rect;
    headerOption.orientation = Qt::Vertical;
    headerOption.position = QStyleOptionHeader::Middle;
    headerOption.text = "";

    QStyle::State state = QStyle::State_None;

    if (isEnabled())
        state |= QStyle::State_Enabled;

    if (window()->isActiveWindow())
        state |= QStyle::State_Active;
 
    style()->drawControl(QStyle::CE_HeaderSection, &headerOption, painter);

    QString text;
    text = text.setNum(logicalIndex + 1);
    QFont font("Arial", 7, QFont::Normal, false);
    QFontMetrics fm(font);

    int x = rect.normalized().x() + ((rect.normalized().width() - fm.width(text))/2);
    int y = rect.normalized().bottomLeft().y() - (1 + (rect.normalized().height() - fm.height())/2);

    painter->setFont(font);
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter->drawText(x, y, text);
}

/////////////////

class KTExposureItemDelegate : public QItemDelegate
{
    public:
        KTExposureItemDelegate(QObject * parent = 0);
        ~KTExposureItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

KTExposureItemDelegate::KTExposureItemDelegate(QObject * parent) :  QItemDelegate(parent)
{
}

KTExposureItemDelegate::~KTExposureItemDelegate()
{
}

void KTExposureItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QItemDelegate::paint(painter, option, index);

    KTExposureTable *table = qobject_cast<KTExposureTable *>(index.model()->parent());

    QTableWidgetItem *item = table->itemFromIndex(index);

    if (item) {

        /* Useful code
        if (item->data(KTExposureTable::IsLocked).toBool()) {
             QPixmap pixmap(THEME_DIR + "icons/padlock.png");
             painter->drawPixmap(1, 5, pixmap);
        } else 
        */

        int x = option.rect.topLeft().x() + 2;
        int y = option.rect.topLeft().y() + 2;
        int w = option.rect.bottomRight().x() - x - 2;
        int h = option.rect.bottomRight().y() - y - 2;

        if (item->data(KTExposureTable::IsLocked).toBool()) {
            painter->setPen(QColor(255, 255, 255, 200));
            painter->drawRect(x, y, w, h);
        }

        if ((item->data(KTExposureTable::IsEmpty).toInt() == KTExposureTable::Empty) && !item->data(KTExposureTable::IsLocked).toBool()) {
            QPen pen(QColor(100, 100, 100, 30));
            pen.setStyle(Qt::DashLine);
            painter->setPen(pen);
            painter->drawRect(x, y, w, h);
        }

        if ((item->data(KTExposureTable::IsEmpty).toInt() == KTExposureTable::Used) && !item->data(KTExposureTable::IsLocked).toBool()) {
            painter->setPen(QColor(0, 0, 0, 60));
            painter->drawRect(x, y, w, h);
        }
    }
}

////////////////////////

struct KTExposureTable::Private
{
    KTExposureHeader *header;
    QMenu *menu;
    bool removingLayer;
    bool removingFrame;
};

KTExposureTable::KTExposureTable(QWidget * parent) : QTableWidget(parent), k(new Private)
{
    KTExposureVerticalHeader *verticalHeader = new KTExposureVerticalHeader(this);
    setVerticalHeader(verticalHeader);

    setItemDelegate(new KTExposureItemDelegate(this));
    k->removingLayer = false;
    k->removingFrame = false;

    QTableWidgetItem *prototype = new QTableWidgetItem();
    prototype->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable);
    prototype->setTextAlignment(Qt::AlignCenter);
    prototype->setData(IsEmpty, Unset);

    setItemPrototype(prototype);

    setRowCount(100);

    for (int i=0; i < 100; i++)
         setRowHeight(i, 20);

    k->header = new KTExposureHeader(this);

    connect(k->header, SIGNAL(visibilityChanged(int, bool)), this, SIGNAL(requestChangeVisibilityLayer(int, bool)));
    connect(k->header, SIGNAL(changedName(int, const QString &)), this, SIGNAL(requestRenameLayer(int, 
                              const QString & )));
    connect(k->header, SIGNAL(sectionMoved(int, int, int)), this, SLOT(emitRequestMoveLayer(int, int, int)));
    connect(k->header, SIGNAL(selectionChanged(int)), this, SLOT(updateLayerSelection(int)));

    setHorizontalHeader(k->header);

    connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(emitRequestSetUsedFrame(int, int)));
    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(emitRequestSelectFrame(int, int, int, int)));

    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

    k->menu = 0;
}

void KTExposureTable::emitRequestRenameFrame(QTableWidgetItem * item)
{
    QModelIndex  index = indexFromItem(item);
    emit requestRenameFrame(index.column(), index.row(), item->text());
}

void KTExposureTable::emitRequestSelectFrame(int currentSelectedRow, int currentColumn, int previousRow, int previousColumn)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!k->removingLayer) { 

        if (k->removingFrame) {
            k->removingFrame = false;
            if ((previousColumn != currentColumn) || (columnCount() == 1))
                 k->header->updateSelection(currentColumn);
            return;
        }

        if (previousRow != currentSelectedRow || previousColumn != currentColumn)
            emit requestSelectFrame(currentLayer(), currentRow());

        if ((previousColumn != currentColumn) || (columnCount() == 1))
             k->header->updateSelection(currentColumn);

    } else { // A layer is being removed
        k->removingLayer = false;
        selectFrame(currentColumn, currentSelectedRow);

        if (previousColumn == 0)
            k->header->updateSelection(0);
        else
            k->header->updateSelection(currentColumn);
    }
}

void KTExposureTable::emitRequestMoveLayer(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    if (! k->header->signalMovedBlocked()) {
        k->header->moveLayer(newVisualIndex, oldVisualIndex);
        emit requestMoveLayer(oldVisualIndex, newVisualIndex);
    }
}

void KTExposureTable::updateLayerSelection(int layerIndex)
{
    selectFrame(layerIndex, currentRow());
}

KTExposureTable::~KTExposureTable()
{
    delete k;
}

QString KTExposureTable::frameName(int indexLayer, int indexFrame)
{
    QTableWidgetItem *frame = item(indexFrame , indexLayer);
    if (frame)
        return frame->text();

    return "";
}

void KTExposureTable::setFrameName(int indexLayer, int indexFrame, const QString & name)
{
    QTableWidgetItem *frame = item(indexFrame , indexLayer);
    frame->setFont(QFont("Arial", 7, QFont::Normal, false));

    if (frame) {
        if (frame->text() != name)
            frame->setText(name);
    }
}

void KTExposureTable::setLayerName(int indexLayer, const QString & name)
{
    k->header->setLayerName(k->header->logicalIndex(indexLayer), name);
}

bool KTExposureTable::frameIsLocked(int indexLayer, int indexFrame)
{
    QTableWidgetItem *frame = item(indexFrame, indexLayer);
    if (frame) {
        return frame->data(IsLocked).toBool();
    } else {
        #ifdef K_DEBUG
               tWarning() << "KTExposureTable::frameIsLocked() - Layer: " << indexLayer << ", Frame: " << indexFrame << " doesn't exist";
        #endif
    }
    return false;
}

KTExposureTable::FrameType KTExposureTable::frameState(int indexLayer, int indexFrame)
{ 
    KTExposureTable::FrameType type = KTExposureTable::Unset;
    QTableWidgetItem *frame = item(indexFrame, indexLayer);
    if (frame)
        type = KTExposureTable::FrameType(frame->data(KTExposureTable::IsEmpty).toInt()); 

    return type;
}

void KTExposureTable::updateFrameState(int indexLayer, int indexFrame, KTExposureTable::FrameType value)
{
    QTableWidgetItem *frame = item(indexFrame, indexLayer);
    frame->setData(IsEmpty, value);

    if (value == KTExposureTable::Used)
        frame->setBackgroundColor(QColor(0xf0f0f0));
}

void KTExposureTable::selectFrame(int indexLayer, int indexFrame)
{
    setCurrentCell(indexFrame, k->header->logicalIndex(indexLayer));
}

void KTExposureTable::setMenu(QMenu *menu)
{
    k->menu = menu;
}

int KTExposureTable::currentLayer() const
{
    return k->header->visualIndex(currentColumn());
}

int KTExposureTable::currentFrame() const
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    QTableWidgetItem *frame = currentItem();

    /* SQA: Old code. Remove it!
    if (frame) {
        if (frame->data(IsEmpty).toBool()) {
            return currentRow();
        } else {
            return currentRow()-1; //???
        }
    }
    */

    if (frame) {
        if (frame->data(KTExposureTable::IsEmpty).toInt() != Unset)
            return currentRow();
    }

    return 0;
}

void KTExposureTable::insertLayer(int index, const QString & name)
{
    insertColumn(index);
    setColumnWidth(index, 70);
    k->header->insertLayer(index, name);
}

void KTExposureTable::insertFrame(int indexLayer, int indexFrame, const QString & name, bool external)
{
    QTableWidgetItem *frame = new QTableWidgetItem;
    frame->setBackgroundColor(QColor(0xe6e6e6));
    frame->setFont(QFont("Arial", 7, QFont::Normal, false));
    frame->setSizeHint(QSize(65, 10));
    frame->setText(name);
    frame->setData(IsEmpty, Empty);
    frame->setTextAlignment(Qt::AlignCenter);

    int logicalIndex = k->header->logicalIndex(indexLayer);

    k->header->setLastFrame(logicalIndex, k->header->lastFrame(logicalIndex) + 1);

    setItem(k->header->lastFrame(logicalIndex)-1, logicalIndex, frame);

    for (int i = k->header->lastFrame(logicalIndex)-1; i > indexFrame; i--)
         exchangeFrame(indexLayer, i , indexLayer, i-1, external);

    if (k->header->lastFrame(logicalIndex) == rowCount()) {
        setRowCount(k->header->lastFrame(logicalIndex) + 100);
        int last = k->header->lastFrame(logicalIndex);
        for (int i=last; i < last + 100; i++)
             setRowHeight(i, 20);
    }
}

void KTExposureTable::setLockFrame(int indexLayer, int indexFrame, bool locked)
{
    int logicalIndex = k->header->logicalIndex(indexLayer);
    QTableWidgetItem * frame = item(indexFrame, logicalIndex);

    if (frame) {
        if (frame->data(KTExposureTable::IsEmpty).toInt() != Unset) {
            if (locked)
                frame->setBackgroundColor(QColor(250, 71, 53));
            else
                frame->setBackgroundColor(QColor(0xe6e6e6));

            frame->setData(IsLocked, locked);
        }
    }
}

void KTExposureTable::setLockLayer(int indexLayer, bool locked)
{
    k->header->setLockLayer(indexLayer, locked);
}

void KTExposureTable::setVisibilityChanged(int visualIndex, bool visibility)
{
    k->header->setVisibilityChanged(k->header->logicalIndex(visualIndex), visibility);
}

void KTExposureTable::removeLayer(int indexLayer)
{
    setUpdatesEnabled(false);
    k->removingLayer = true;

    k->header->removeLayer(indexLayer);
    removeColumn(indexLayer);

    setUpdatesEnabled(true);
}

void KTExposureTable::removeFrame(int indexLayer, int indexFrame, bool fromMenu)
{
    //setUpdatesEnabled(false);
    k->removingFrame = fromMenu;

    QTableWidgetItem *item  = takeItem(indexFrame, indexLayer);
    if (item) {
        for (int index = indexFrame + 1; index < k->header->lastFrame(indexLayer); index++) {
             QTableWidgetItem * idx  = takeItem(index, indexLayer);
             if (idx)
                 setItem(index - 1, indexLayer, idx);
        }
        k->header->setLastFrame(indexLayer, k->header->lastFrame(indexLayer)-1);
    }

    //setUpdatesEnabled(true);
}

void KTExposureTable::exchangeFrame(int oldPosLayer, int oldPosFrame, int newPosLayer, int newPosFrame, bool external)
{
    QTableWidgetItem * oldItem  = takeItem(oldPosFrame, oldPosLayer);
    QTableWidgetItem * newItem  = takeItem(newPosFrame, newPosLayer);

    setItem(newPosFrame, newPosLayer, oldItem);
    setItem(oldPosFrame, oldPosLayer, newItem);

    if (!external) 
        setCurrentItem(oldItem);
}

void KTExposureTable::moveLayer(int oldPosLayer, int newPosLayer)
{
    k->header->moveLayer(oldPosLayer, newPosLayer);
}

void KTExposureTable::emitRequestSetUsedFrame(int indexFrame, int indexLayer)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    int layer = k->header->visualIndex(indexLayer);
    int lastFrame = k->header->lastFrame(indexLayer); 

    if (indexFrame >= lastFrame) {
        for (int column=0; column<columnCount(); column++) {
             int used = usedFrames(column); 
             if (lastFrame >= used) {
                 for (int frame=used; frame <= indexFrame; frame++)
                      emit requestSetUsedFrame(column, frame);
             }
        }

        emit requestSelectFrame(layer, indexFrame);
    } 
}

int KTExposureTable::usedFrames(int column) const
{
    return k->header->lastFrame(column);
}

bool KTExposureTable::edit(const QModelIndex & index, EditTrigger trigger, QEvent *event)
{
    QTableWidgetItem *item = itemFromIndex(index);

    if (item) {
        if (item->data(KTExposureTable::IsEmpty).toInt() != Unset)
            QTableWidget::edit(index, trigger, event);
        else
            return false;
    }

    return false;
}

void KTExposureTable::mousePressEvent(QMouseEvent * event)
{
    int frame = rowAt(event->y());
    if (event->button() == Qt::RightButton) {
        if (k->header->lastFrame(currentLayer()) >= frame) {
            if (k->menu) {
                clearFocus();
                k->menu->exec(event->globalPos());
            }
        } else {
            return;
        }
    }

    QTableWidget::mousePressEvent(event);
}

void KTExposureTable::commitData(QWidget *editor)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    QTableWidget::commitData(0); // Don't rename

    if (lineEdit)
        emit requestRenameFrame(currentLayer(), currentFrame(), lineEdit->text());
}

int KTExposureTable::layersTotal()
{
    return k->header->layersTotal();
}

int KTExposureTable::framesTotal()
{
    return k->header->lastFrame(0);
}

int KTExposureTable::framesTotalAtCurrentLayer()
{
    return k->header->lastFrame(currentLayer());
}

void KTExposureTable::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Up) {
        int row = currentRow()-1;
        if (row > -1)
            setCurrentCell(row, currentColumn());
        return;
    }

    if (event->key() == Qt::Key_Down) {
        int limit = rowCount()-1;
        int next = currentRow()+1;
        if (next <= limit)
            setCurrentCell(next, currentColumn());
        return;
    }

    if (event->key() == Qt::Key_Right) {
        int limit = columnCount()-1;
        int next = currentColumn()+1;
        if (next <= limit)
            setCurrentCell(currentRow(), next);
        return;
    }

    if (event->key() == Qt::Key_Left) {
        int column = currentColumn()-1;
        if (column > -1)
            setCurrentCell(currentRow(), column);
        return;
    }   

    if (event->key() == Qt::Key_Return) {
        emitRequestSetUsedFrame(currentRow(), currentColumn());
        return;
    }
}

void KTExposureTable::emitCellClicked(int frame, int layer) 
{
    emit cellClicked(frame, layer);
}

void KTExposureTable::enterEvent(QEvent *event)
{
    if (!hasFocus())
        setFocus();

    QTableWidget::enterEvent(event);
}

void KTExposureTable::leaveEvent(QEvent *event)
{
    if (hasFocus())
        clearFocus();

    QTableWidget::leaveEvent(event);
}

void KTExposureTable::reset()
{
    int cols = columnCount();
    if (cols > 1) {
        for (int i=1; i < cols; i++)
             removeLayer(i);
    }

    int rows = rowCount();
    for (int i=1; i < rows;  i++) {
         QTableWidgetItem *item = takeItem(i, 0);
    }

    k->header->setLastFrame(0, 1);
}

