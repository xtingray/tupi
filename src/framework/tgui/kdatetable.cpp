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


#include "kdatetable.h"

#include <QHeaderView>
#include <QPainter>

#include <QtDebug>

class Day : public QHeaderView
{
	public:
		Day(QWidget *parent = 0);
		~Day();
		
	protected:
		void paintSection ( QPainter * painter, const QRect & rect, int logicalIndex ) const;
		QSize sectionSizeFromContents ( int logicalIndex ) const;
};

Day::Day(QWidget *parent): QHeaderView(Qt::Horizontal, parent)
{
}

Day::~Day()
{
}

void Day::paintSection ( QPainter *painter, const QRect &rect, int logicalIndex) const
{
	QFont font = painter->font();
	QPalette pal = palette();
	
	painter->save();
	
	font.setBold( true );
	painter->setFont(font);
	
	QString dayText;
	
	switch(logicalIndex)
	{
		case 0: // mon
		{
			dayText = tr("Mon");
		}
		break;
		case 1: // tue
		{
			dayText = tr("Tue");
		}
		break;
		case 2: // Wed
		{
			dayText = tr("Wed");
		}
		break;
		case 3: // Thu
		{
			dayText = tr("Thu");
		}
		break;
		case 4: // Fri
		{
			dayText = tr("Fri");
		}
		break;
		
		case 5: // Sat
		{
			dayText = tr("Sat");
			
			painter->setBrush(pal.text());
			painter->setPen( QPen( QBrush(pal.base()), 3 ));
			
			painter->fillRect(rect, pal.text());
		}
		break;
		case 6: // Sun
		{
			dayText = tr("Sun");
			
			painter->setBrush(pal.text());
			painter->setPen( QPen( QBrush(pal.base()), 3 ));
			
			painter->fillRect(rect, pal.text());
		}
		break;
	}
	
	painter->drawText(rect, Qt::AlignHCenter, dayText);
	
	painter->restore();
	
}

QSize Day::sectionSizeFromContents( int logicalIndex ) const
{
	return QSize(sectionSize(logicalIndex),15);
}

//////////////////////

KDateTable::KDateTable(QWidget *parent) : QTableWidget( 5, 7, parent)
{
	m_date = QDate::currentDate();
	
	setMonth(m_date.month());
	
	verticalHeader()->hide();
	
	setHorizontalHeader(new Day(this));
	
	setCellSize( 30, 15 );
	
	horizontalHeader()->setResizeMode(QHeaderView::Custom);
	verticalHeader()->setResizeMode(QHeaderView::Custom);
	
	setSelectionMode (QAbstractItemView::SingleSelection);
	
	connect(this, SIGNAL(itemClicked(QTableWidgetItem *)), this, SLOT(dateFromItem(QTableWidgetItem *)));
	
	connect(this, SIGNAL( itemActivated (QTableWidgetItem * )), this, SLOT(dateFromItem(QTableWidgetItem *)));
	
	connect(this, SIGNAL( itemEntered (QTableWidgetItem * )), this, SLOT(dateFromItem(QTableWidgetItem *)));
}

KDateTable::~KDateTable()
{
}

void KDateTable::setDate(const QDate &date)
{
	if ( date != m_date)
	{
		emit dateChanged( date );
		m_date = date;
	}
	
	setMonth( date.month() );
}

void KDateTable::setCellSize(int width, int height)
{
	m_cellWidth = width;
	m_cellHeight = height;
	for(int column = 0; column < columnCount(); column++)
	{
		horizontalHeader()->resizeSection(column, width);
	}
	for( int row = 0; row < rowCount(); row++)
	{
		verticalHeader()->resizeSection(row, height);
	}
}

void KDateTable::setMonth(int month)
{
	m_date.setYMD(m_date.year(), month, m_date.day());
	
	for(int row = 0; row < rowCount(); row++)
	{
		for(int col = 0; col < columnCount(); col++)
		{
			int pos= 7*row+col;
			
			QDate cellDate = dateFromPosition( pos );
			
			QString text = QString::number(cellDate.day());
			
			QTableWidgetItem *newItem = new QTableWidgetItem(text);
			newItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
			newItem->setTextAlignment ( Qt::AlignCenter );
			
			setItem(row, col, newItem);
			
			if ( cellDate.month() != m_date.month() )
			{
				newItem->setTextColor( palette().mid().color() );
			}
			else if ( cellDate.day() == m_date.day() )
			{
				setCurrentItem(newItem);
			}
		}
	}
}

QDate KDateTable::dateFromPosition(int position)
{
	QDate date;
	date.setYMD( m_date.year(), m_date.month(), 1);
	
	int firstday = date.dayOfWeek();
	int offset = (firstday - 8) % 7;
	
	if ( offset < 1 ) offset += 7;
	
	date = date.addDays( position - offset);
	
	return date;
}

QDate KDateTable::dateFromItem(QTableWidgetItem *item)
{
	if ( !item ) return QDate();
	
	QDate date;
	date.setYMD( m_date.year(), m_date.month(), item->text().toInt());
		
	setDate(date);
	
	return date;
}

QDate KDateTable::date() const
{
	return m_date;
}

int KDateTable::cellWidth() const
{
	return m_cellWidth;
}


int KDateTable::cellHeight() const
{
	return m_cellHeight;
}

void KDateTable::paintEvent(QPaintEvent *e)
{
	setCellSize( viewport()->width() / 7, viewport()->height() / 5);
	
	QTableWidget::paintEvent(e);
}

