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

#include "tcellview.h"
#include "tdebug.h"

#include <QPainter>
#include <QPaintEvent>
#include <QItemSelectionModel>
#include <QPainterPath>
#include <QScrollBar>
#include <QHeaderView>
#include <QTimer>

////////// TCellViewItemDelegate ///////////

class TCellViewItemDelegate : public QAbstractItemDelegate
{
    public:
        TCellViewItemDelegate(QObject * parent = 0);
        ~TCellViewItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
        virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

TCellViewItemDelegate::TCellViewItemDelegate(QObject * parent) :  QAbstractItemDelegate(parent)
{
}

TCellViewItemDelegate::~TCellViewItemDelegate()
{
}

void TCellViewItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    Q_ASSERT(index.isValid());
    const QAbstractItemModel *model = index.model();
    Q_ASSERT(model);

    QVariant value;

    QStyleOptionViewItem opt = option;

    // do layout
    QImage img = qvariant_cast<QImage>(model->data(index,Qt::DisplayRole));

    if (! img.isNull())
        painter->drawImage(opt.rect, img);

    // draw the background color
    value = model->data(index, Qt::BackgroundColorRole);

    if (value.isValid()) {
        QBrush brush = qvariant_cast<QBrush>(value);
        if (brush.gradient()) {
            QMatrix m;
            m.translate(option.rect.topLeft().x(), option.rect.topLeft().y());
            m.scale((float)(option.rect.width())/100.0 , (float)(option.rect.height())/100.0);
            brush.setMatrix(m);
            painter->fillRect(option.rect, brush);
        } else {
            painter->fillRect(option.rect, brush);
        }
    }

    // Selection!
    if (option.showDecorationSelected && (option.state & QStyle::State_Selected)) {
        QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
        painter->save();
        painter->setPen(QPen(option.palette.brush(cg, QPalette::Highlight), 3));
        painter->drawRect(option.rect.adjusted(1,1,-2,-2));
        painter->restore();
    }
}

QSize TCellViewItemDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
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

    //QFontMetrics fontMetrics(fnt);

    return (pixmapRect).size();
}

////////// TCellViewItem ////////
TCellViewItem::TCellViewItem() : QTableWidgetItem(UserType)
{
     setFlags(Qt::ItemIsEditable
             |Qt::ItemIsSelectable
             |Qt::ItemIsUserCheckable
             |Qt::ItemIsEnabled
             |Qt::ItemIsDragEnabled
             |Qt::ItemIsDropEnabled);
}

TCellViewItem::~TCellViewItem()
{
}

QImage TCellViewItem::image() const
{
    return qvariant_cast<QImage>(data(Qt::DisplayRole));
}

QBrush TCellViewItem::background() const
{
    return qvariant_cast<QBrush>(data(Qt::BackgroundColorRole));
}
    
////////// TCellView  ///////////
TCellView::TCellView(int MAX_COLUMNS, QWidget *parent) : QTableWidget(parent), m_countColor(0),  m_col(0), m_row(0), MAX_COLUMNS(MAX_COLUMNS)
{
    setup();
}

TCellView::TCellView(int rows, int columns, int MAX_COLUMNS, QWidget *parent)
          : QTableWidget(rows, columns, parent), m_countColor(0),  m_col(0), m_row(0), MAX_COLUMNS(MAX_COLUMNS)
{
    setup();
}

void TCellView::setup()
{
    setItemDelegate(new TCellViewItemDelegate(this));

    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

    verticalHeader()->hide();
    horizontalHeader()->hide();

    setItemSize(18, 18);

    // horizontalHeader()->setResizeMode(QHeaderView::Custom);
    // verticalHeader()->setResizeMode(QHeaderView::Custom);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Custom);
    verticalHeader()->setSectionResizeMode(QHeaderView::Custom);
}

void TCellView::setItemSize(int w, int h)
{
    m_rectHeight = h;
    m_rectWidth = w;
    QTimer::singleShot(0, this, SLOT(fixSize()));
}
    
void TCellView::fixSize()
{
    setUpdatesEnabled(false);

    for (int column = 0; column < columnCount(); column++)
         horizontalHeader()->resizeSection(column, m_rectWidth);

    for (int row = 0; row < rowCount(); row++)
         verticalHeader()->resizeSection(row, m_rectHeight);

    setUpdatesEnabled(true);
}


TCellView::~TCellView()
{
}

QStyleOptionViewItem TCellView::viewOptions() const
{
    QStyleOptionViewItem option = QAbstractItemView::viewOptions();
    option.showDecorationSelected = true;
    option.decorationSize = QSize(22, 22);
    option.decorationPosition = QStyleOptionViewItem::Right;

    return option;
}

void TCellView::addItem(TCellViewItem *item)
{
    if (columnCount() < MAX_COLUMNS)
        insertColumn(columnCount());

    if (m_countColor % MAX_COLUMNS == 0) {
        insertRow(rowCount());
        m_row++;
        m_col = 0;
    } else {
        m_col++;
    }

    m_countColor++;
    setItem(m_row-1 , m_col , item);

    fixSize();
}

void TCellView::addItem(const QBrush& b)
{
    TCellViewItem *item = new TCellViewItem;
    item->setBackground(b);

    addItem(item);
}

void TCellView::addItem(const QImage &i)
{
    TCellViewItem *item = new TCellViewItem;
    item->setData(Qt::DisplayRole, i);

    addItem(item);
}

void TCellView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier) {
        SHOW_VAR(event->delta());
    } else {
        QTableWidget::wheelEvent(event);
    }
}
