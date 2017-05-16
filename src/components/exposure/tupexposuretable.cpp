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

#include "tupexposuretable.h"
#include "tconfig.h"

/**
 * @author Jorge Cuadrado
 */

/////////////////
class TUPI_EXPORT TupExposureVerticalHeader : public QHeaderView
{
    public:
        TupExposureVerticalHeader(QWidget * parent = 0);
        ~TupExposureVerticalHeader();
        void paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const;
};

TupExposureVerticalHeader::TupExposureVerticalHeader(QWidget * parent) : QHeaderView(Qt::Vertical, parent)
{
    setFixedWidth(25);
}

TupExposureVerticalHeader::~TupExposureVerticalHeader()
{
}

void TupExposureVerticalHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
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
    QFont font = this->font();
    font.setPointSize(7);
    QFontMetrics fm(font);

    int x = rect.normalized().x() + ((rect.normalized().width() - fm.width(text))/2);
    int y = rect.normalized().bottomLeft().y() - (1 + (rect.normalized().height() - fm.height())/2);

    painter->setFont(font);
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter->drawText(x, y, text);
}

/////////////////

class TupExposureItemDelegate : public QItemDelegate
{
    public:
        TupExposureItemDelegate(QObject * parent = 0);
        ~TupExposureItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

    private:
        struct Private;
        Private *const k;
};

struct TupExposureItemDelegate::Private
{
    QString themeName;
};

TupExposureItemDelegate::TupExposureItemDelegate(QObject * parent) :  QItemDelegate(parent), k(new Private)
{
    TCONFIG->beginGroup("General");
    k->themeName = TCONFIG->value("Theme", "Light").toString();
}

TupExposureItemDelegate::~TupExposureItemDelegate()
{
}

void TupExposureItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QItemDelegate::paint(painter, option, index);
    TupExposureTable *table = qobject_cast<TupExposureTable *>(index.model()->parent());
    QTableWidgetItem *item = table->itemFromIndex(index);

    if (item) {
        /* SQA: Useful code
        if (item->data(TupExposureTable::IsLocked).toBool()) {
             QPixmap pixmap(THEME_DIR + "icons/padlock.png");
             painter->drawPixmap(1, 5, pixmap);
        } else 
        */

        int x = option.rect.topLeft().x() + 2;
        int y = option.rect.topLeft().y() + 2;
        int w = option.rect.bottomRight().x() - x - 2;
        int h = option.rect.bottomRight().y() - y - 2;

        if ((item->data(TupExposureTable::IsEmpty).toInt() == TupExposureTable::Empty)) {
            QColor color(100, 100, 100, 30);
            if (k->themeName.compare("Dark") == 0)
                color = QColor(120, 120, 120);

            QPen pen(color);
            pen.setStyle(Qt::DashLine);
            painter->setPen(pen);
            painter->drawRect(x, y, w, h);
        }

        if ((item->data(TupExposureTable::IsEmpty).toInt() == TupExposureTable::Used)) {
            QPainterPath path(QPointF(x, y));
            path.lineTo(x, y + h - 3);
            path.lineTo(x + 3, y + h);
            path.lineTo(x + w, y + h);
            path.lineTo(x + w, y + 4);
            path.lineTo(x + w - 8, y); 

            QColor color(0, 102, 255, 80);
            if (k->themeName.compare("Dark") == 0)
                color = QColor(0, 0, 0, 60);

            painter->fillPath(path, QBrush(color));

            if (k->themeName.compare("Dark") == 0) {
                QPen pen(QColor(100, 100, 100));
                painter->setPen(pen);
                painter->drawPath(path);
            }
        }
    }
}

////////////////////////

struct TupExposureTable::Private
{
    TupExposureHeader *header;
    QMenu *singleMenu;
    // QMenu *multipleMenu;
    bool removingLayer;
    bool isLocalRequest;
    QString themeName;
};

TupExposureTable::TupExposureTable(QWidget * parent) : QTableWidget(parent), k(new Private)
{
    TCONFIG->beginGroup("General");
    k->themeName = TCONFIG->value("Theme", "Light").toString();

    k->isLocalRequest = false;

    TupExposureVerticalHeader *verticalHeader = new TupExposureVerticalHeader(this);
    setVerticalHeader(verticalHeader);

    setItemDelegate(new TupExposureItemDelegate(this));
    k->removingLayer = false;

    QTableWidgetItem *prototype = new QTableWidgetItem();
    prototype->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable);
    prototype->setTextAlignment(Qt::AlignCenter);
    prototype->setData(IsEmpty, Unset);
    setItemPrototype(prototype);

    setRowCount(100);

    for (int i=0; i < 100; i++)
        setRowHeight(i, 20);

    k->header = new TupExposureHeader(this);
    connect(k->header, SIGNAL(visibilityChanged(int, bool)), this, SIGNAL(layerVisibilityChanged(int, bool)));
    connect(k->header, SIGNAL(nameChanged(int, const QString &)), this, SIGNAL(layerNameChanged(int, const QString & )));
    connect(k->header, SIGNAL(sectionMoved(int, int, int)), this, SLOT(requestLayerMove(int, int, int)));
    connect(k->header, SIGNAL(headerSelectionChanged(int)), this, SLOT(updateLayerSelection(int)));
    setHorizontalHeader(k->header);

    connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(markUsedFrames(int, int)));
    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(requestFrameSelection(int, int, int, int)));

    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    k->singleMenu = 0;
}

void TupExposureTable::requestFrameRenaming(QTableWidgetItem *item)
{
    QModelIndex index = indexFromItem(item);
    emit frameRenamed(index.column(), index.row(), item->text());
}

void TupExposureTable::requestFrameSelection(int currentSelectedRow, int currentColumn, int previousRow, int previousColumn)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureTable::requestFrameSelection()]";
        #else
            T_FUNCINFO << "current: " << currentSelectedRow << ", " << currentColumn << " - previous: " << previousRow << ", " << previousColumn;
        #endif
    #endif

    if (!k->removingLayer) { 
        QTableWidgetItem *frame = item(currentSelectedRow, currentColumn);
        if (frame) {
            if (previousColumn != currentColumn || previousRow != currentSelectedRow)
                emit frameSelected(currentLayer(), currentRow());

            if ((previousColumn != currentColumn) || (columnCount() == 1))
                k->header->updateSelection(currentColumn);
        }
    } else { // A layer is being removed
        k->removingLayer = false;
        selectFrame(currentColumn, currentSelectedRow);

        if (previousColumn == 0)
            k->header->updateSelection(0);
        else
            k->header->updateSelection(currentColumn);
    }
}

void TupExposureTable::requestLayerMove(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    if (!k->header->sectionIsMoving()) {
        int distance = newVisualIndex - oldVisualIndex;
        if (distance > 0) {
            if (distance > 1)
                newVisualIndex = oldVisualIndex + 1;
        } else {
            distance = abs(distance);
            if (distance > 1)
                newVisualIndex = oldVisualIndex - 1;
        }

        k->isLocalRequest = true;
        emit layerMoved(oldVisualIndex, newVisualIndex);
    }
}

void TupExposureTable::updateLayerSelection(int layerIndex)
{
    selectFrame(layerIndex, currentRow());
}

TupExposureTable::~TupExposureTable()
{
    delete k;
}

QString TupExposureTable::frameName(int layerIndex, int frameIndex)
{
    QTableWidgetItem *frame = item(frameIndex, layerIndex);
    if (frame)
        return frame->text();

    return "";
}

void TupExposureTable::setFrameName(int layerIndex, int frameIndex, const QString &name)
{
    QTableWidgetItem *frame = item(frameIndex, layerIndex);
    if (frame) { 
        QFont font = this->font();
        font.setPointSize(7);
        frame->setFont(font);
        if (frame->text() != name)
            frame->setText(name);
    }
}

void TupExposureTable::setLayerName(int layerIndex, const QString & name)
{
    k->header->setSectionTitle(k->header->logicalIndex(layerIndex), name);
}

bool TupExposureTable::layerIndexIsValid(int layerIndex)
{
    if (layerIndex >= 0 && layerIndex < columnCount())
        return true;

    return false; 
}

bool TupExposureTable::frameIndexIsValid(int frameIndex)
{
    if (frameIndex >= 0 && frameIndex < rowCount())
        return true;

    return false;  
}

TupExposureTable::FrameType TupExposureTable::frameState(int layerIndex, int frameIndex)
{ 
    TupExposureTable::FrameType type = TupExposureTable::Unset;
    if (layerIndexIsValid(layerIndex) && frameIndexIsValid(frameIndex)) {
        QTableWidgetItem *frame = item(frameIndex, layerIndex);
        if (frame)
            type = TupExposureTable::FrameType(frame->data(TupExposureTable::IsEmpty).toInt()); 
    }

    return type;
}

void TupExposureTable::updateFrameState(int layerIndex, int frameIndex, TupExposureTable::FrameType value)
{	
    if (layerIndex < 0 || frameIndex < 0)
        return;

    QTableWidgetItem *frame = item(frameIndex, layerIndex);
    if (frame) {
        frame->setData(IsEmpty, value);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureTable::updateFrameState() - Error: No frame at [" + QString::number(layerIndex) + ", " + QString::number(frameIndex) + "]";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupExposureTable::selectFrame(int layerIndex, int frameIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureTable::selectFrame()]";
        #else
            T_FUNCINFO << "layerIndex, frameIndex -> " << layerIndex << ", " << frameIndex;
        #endif
    #endif

    if (k->header->currentSectionIndex() != layerIndex)
        k->header->updateSelection(layerIndex);

    setCurrentCell(frameIndex, layerIndex);
}

void TupExposureTable::selectFrame(int layerIndex, int frameIndex, const QString &selection)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureTable::selectFrame()]";
        #else
            T_FUNCINFO;
            tWarning() << "selection -> " << selection;
        #endif
    #endif

    if (k->header->currentSectionIndex() != layerIndex)
        k->header->updateSelection(layerIndex);

    QStringList coords = selection.split(",");
    if (coords.count() == 4) {
        int initLayer = coords.at(0).toInt();
        int lastLayer = coords.at(1).toInt();
        int initFrame = coords.at(2).toInt();
        int lastFrame = coords.at(3).toInt();

        selectionModel()->clearSelection();
        setCurrentCell(frameIndex, layerIndex);

        for (int i=initLayer; i<=lastLayer; i++) {
            for (int j=initFrame; j<=lastFrame; j++)
                selectionModel()->select(model()->index(j, i), QItemSelectionModel::Select);
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureTable::selectFrame() - Selection area parameter is misconfigured!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupExposureTable::updateSelection(int layerIndex, int frameIndex)
{
    selectionModel()->clearSelection();
    selectionModel()->select(model()->index(frameIndex, layerIndex), QItemSelectionModel::Select);
}

void TupExposureTable::setSinglePopUpMenu(QMenu *single)
{
    k->singleMenu = single;
    // k->multipleMenu = multiple;
}

int TupExposureTable::currentLayer() const
{
    return k->header->visualIndex(currentColumn());
}

int TupExposureTable::currentFrame() const
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureTable::currentFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QTableWidgetItem *frame = currentItem();

    if (frame) {
        if (frame->data(TupExposureTable::IsEmpty).toInt() != Unset)
            return currentRow();
    }

    return 0;
}

void TupExposureTable::insertLayer(int index, const QString & name)
{
    insertColumn(index);
    setColumnWidth(index, 70);
    k->header->insertSection(index, name);
}

void TupExposureTable::insertFrame(int layerIndex, int frameIndex, const QString &name, bool external)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureTable::insertFrame()]";
        #else
            T_FUNCINFO << "layerIndex, frameIndex -> " << layerIndex << ", " << frameIndex;
        #endif
    #endif

    QTableWidgetItem *frame = new QTableWidgetItem;

    QColor color = Qt::transparent;
    if (k->themeName.compare("Dark") == 0)
        frame->setForeground(Qt::black);
    frame->setBackgroundColor(color);

    QFont font = this->font();
    font.setPointSize(7);
    frame->setFont(font);
    frame->setSizeHint(QSize(65, 10));
    frame->setText(name);
    frame->setData(IsEmpty, Empty);
    frame->setTextAlignment(Qt::AlignCenter);

    int logicalIndex = k->header->logicalIndex(layerIndex);
    k->header->setLastFrame(logicalIndex, k->header->lastFrame(logicalIndex) + 1);
    setItem(k->header->lastFrame(logicalIndex)-1, logicalIndex, frame);

    int init = k->header->lastFrame(logicalIndex)-1;
    for (int i=init; i>frameIndex; i--)
        exchangeFrame(layerIndex, i, layerIndex, i-1, external);

    if (k->header->lastFrame(logicalIndex) == rowCount()) {
        setRowCount(k->header->lastFrame(logicalIndex) + 100);
        int last = k->header->lastFrame(logicalIndex);
        for (int i=last; i < last + 100; i++)
             setRowHeight(i, 20);
    }
}

/*
void TupExposureTable::setLockLayer(int layerIndex, bool locked)
{
    k->header->setLockFlag(layerIndex, locked);
}
*/

void TupExposureTable::setLayerVisibility(int visualIndex, bool visibility)
{
    k->header->setSectionVisibility(k->header->logicalIndex(visualIndex), visibility);
}

void TupExposureTable::removeLayer(int layerIndex)
{
    setUpdatesEnabled(false);

    k->removingLayer = true;
    k->header->removeSection(layerIndex);
    removeColumn(layerIndex);

    setUpdatesEnabled(true);
}

void TupExposureTable::removeFrame(int layerIndex, int frameIndex)
{
    QTableWidgetItem *item = takeItem(frameIndex, layerIndex);

    if (item) {
        k->header->setLastFrame(layerIndex, k->header->lastFrame(layerIndex)-1);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureTable::removeFrame() - No item available at [" + QString::number(layerIndex) + ", " + QString::number(frameIndex) + "]";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupExposureTable::exchangeFrame(int oldPosLayer, int oldPosFrame, int newPosLayer, int newPosFrame, bool external)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureTable::exchangeFrame()]";
        #else
            T_FUNCINFO << "frameIndex 1, frameIndex 2 -> " << oldPosFrame << ", " << newPosFrame;
        #endif
    #endif

    QTableWidgetItem *frame1 = item(oldPosFrame, oldPosLayer);
    TupExposureTable::FrameType type1 = TupExposureTable::FrameType(frame1->data(TupExposureTable::IsEmpty).toInt());
    QString name1 = frame1->text();

    QTableWidgetItem *frame2 = item(newPosFrame, newPosLayer);
    TupExposureTable::FrameType type2 = TupExposureTable::FrameType(frame2->data(TupExposureTable::IsEmpty).toInt());
    QString name2 = frame2->text();

    frame1->setText(name2);
    frame1->setData(IsEmpty, type2);
    frame2->setText(name1);
    frame2->setData(IsEmpty, type1);

    if (!external)
        setCurrentItem(frame2);
}

void TupExposureTable::moveLayer(int oldPosLayer, int newPosLayer)
{
    k->header->moveHeaderSection(oldPosLayer, newPosLayer, k->isLocalRequest);
    if (k->isLocalRequest)
        k->isLocalRequest = false;

    for (int frameIndex = 0; frameIndex < k->header->lastFrame(oldPosLayer); frameIndex++)
         exchangeFrame(oldPosLayer, frameIndex, newPosLayer, frameIndex, true);

    blockSignals(true);
    selectFrame(newPosLayer, currentRow());
    blockSignals(false);
}

void TupExposureTable::markUsedFrames(int frameIndex, int layerIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureTable::markUsedFrames()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int lastFrame = k->header->lastFrame(layerIndex); 

    if (frameIndex >= lastFrame) {
        int used = usedFrames(layerIndex);
        if (lastFrame >= used) {
            for (int frame=used; frame <= frameIndex; frame++)
                emit frameUsed(layerIndex, frame);
        }
        emit frameSelected(layerIndex, frameIndex);
    } 
}

void TupExposureTable::markNextFrame(int frameIndex, int layerIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExposureTable::markNextFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int lastFrame = k->header->lastFrame(layerIndex);

    if (frameIndex >= lastFrame) {
        int used = usedFrames(layerIndex);
        if (frameIndex == used) {
            emit frameUsed(layerIndex, frameIndex);
            emit frameSelected(layerIndex, frameIndex);
        }
    }
}

int TupExposureTable::usedFrames(int column) const
{
    return k->header->lastFrame(column);
}

bool TupExposureTable::edit(const QModelIndex & index, EditTrigger trigger, QEvent *event)
{
    QTableWidgetItem *item = itemFromIndex(index);

    if (item) {
        if (item->data(TupExposureTable::IsEmpty).toInt() != Unset)
            QTableWidget::edit(index, trigger, event);
        else
            return false;
    }

    return false;
}

void TupExposureTable::mousePressEvent(QMouseEvent *event)
{
    int frameIndex = rowAt(event->y());
    int layerIndex = currentLayer();
    if (event->button() == Qt::RightButton) {
        if (frameIndex <= k->header->lastFrame(layerIndex)) {
            int frames = selectedItems().count();
            if (frames == 1) {
                if (k->singleMenu)
                    k->singleMenu->exec(event->globalPos());
            }
            /* SQA: PopUp menu for multiple selection
            } else if (frames > 1) {
                if (k->multipleMenu) {
                    // clearFocus();
                    k->multipleMenu->exec(event->globalPos());
                }
            }
            */
        } else {
            return;
        }
    }

    QTableWidget::mousePressEvent(event);
}

void TupExposureTable::mouseMoveEvent(QMouseEvent *event)
{
    int layerIndex = currentLayer();
    int frameIndex = rowAt(event->y()); 
    QList<int> layers = currentSelection();

    if (!layers.isEmpty()) {
        for (int j=layers.at(0); j<=layerIndex; j++) {
            int top = k->header->lastFrame(j);
            if (frameIndex >= top) {
                for (int i=top; i<=frameIndex; i++)
                    emit frameUsed(j, i);
            }
        }
    }

    QTableWidget::mouseMoveEvent(event);
}

void TupExposureTable::commitData(QWidget *editor)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    QTableWidget::commitData(0); // Don't rename

    if (lineEdit)
        emit frameRenamed(currentLayer(), currentFrame(), lineEdit->text());
}

int TupExposureTable::layersCount()
{
    return k->header->sectionsTotal();
}

int TupExposureTable::framesCount()
{
    return k->header->lastFrame(0);
}

int TupExposureTable::framesCountAtCurrentLayer()
{
    return k->header->lastFrame(currentLayer());
}

int TupExposureTable::framesCountAtLayer(int layer)
{
    return k->header->lastFrame(layer);
}

void TupExposureTable::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        emit selectionRemoved();
        return;
    }

    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_PageUp) {
        int row = currentRow()-1;
        if (row > -1) {
            if (event->modifiers() == Qt::ControlModifier)
                emit selectionRemoved();
            else
                setCurrentCell(row, currentColumn());
        } else {
            if (row == -1 && event->modifiers() == Qt::ControlModifier)
                emit selectionRemoved();
        }
        return;
    }

    if (event->key() == Qt::Key_Down || event->key() == Qt::Key_PageDown) {
        int framesCount = k->header->lastFrame(currentLayer());
        int next = currentRow() + 1;
        if (event->modifiers() == Qt::ControlModifier) {
            emit frameExtended(currentLayer(), currentFrame());
        } else {
            if (next >= framesCount)
                markNextFrame(next, currentColumn());
            else
                setCurrentCell(next, currentColumn());
        }
        return;
    }

    if (event->key() == Qt::Key_C) {
        if (event->modifiers() == Qt::ControlModifier)
            emit selectionCopied();
        return;
    }

    if (event->key() == Qt::Key_V) {
        if (event->modifiers() == Qt::ControlModifier)
            emit selectionPasted();
        return;
    }

    if (event->key() == Qt::Key_X) {
        if (event->modifiers() == Qt::ControlModifier) {
            emit selectionCopied();
            emit selectionRemoved();
        }
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
}

// SQA : Verify if this method is required
void TupExposureTable::notifyCellClicked(int frame, int layer) 
{
    emit cellClicked(frame, layer);
}

void TupExposureTable::enterEvent(QEvent *event)
{
    if (!hasFocus())
        setFocus();

    QTableWidget::enterEvent(event);
}

void TupExposureTable::leaveEvent(QEvent *event)
{
    if (hasFocus())
        clearFocus();

    QTableWidget::leaveEvent(event);
}

void TupExposureTable::reset()
{
    int cols = columnCount();
    if (cols > 1) {
        for (int i=1; i < cols; i++)
             removeLayer(i);
    }

    int rows = rowCount();
    for (int i=1; i < rows;  i++)
         takeItem(i, 0);

    k->header->setLastFrame(0, 1);
}

QList<int> TupExposureTable::currentSelection()
{
    QList<int> coords;
    QList<int> layers;
    QList<int> frames;
    QModelIndexList selection = selectedIndexes();

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

void TupExposureTable::updateSceneView(int layerIndex, int frameIndex)
{
    scrollToItem(item(frameIndex, layerIndex));
}
