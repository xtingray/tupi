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

#include "tupbuttonitem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QApplication>

TupButtonItem::TupButtonItem(QGraphicsItem * parent, QGraphicsScene * scene)
	: QGraphicsItem(parent, scene)
{
	QGraphicsItem::setCursor(QCursor(Qt::PointingHandCursor ));
	setFlags (QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable  );
	
	m_iconSize = QSize(22,22);
}


TupButtonItem::~TupButtonItem()
{
}


void TupButtonItem::fromXml(const QString &xml)
{
}


QDomElement TupButtonItem::toXml(QDomDocument &doc) const
{
	QDomElement root = doc.createElement("button");
	
	return root;
}


void TupButtonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *w )
{
	QStyleOptionButton optionButton;
	
	optionButton.text = m_text;
	optionButton.rect = boundingRect().toRect();
	optionButton.icon = m_icon;
	optionButton.iconSize = m_iconSize;
	
	if (option->state & QStyle::State_Sunken)
	{
		optionButton.state = option->state;
	}
	else
	{
		
	}
	QApplication::style()->drawControl( QStyle::CE_PushButton, &optionButton ,painter, w );
}

QRectF TupButtonItem::boundingRect() const
{
	double width = 2;
	double height = 2;
	
	QFontMetricsF fm(m_font);
	
	width += fm.width( m_text );
	height += fm.height();
	
	width += m_iconSize.width();
	height += m_iconSize.height();
	
	return QRectF(0, 0, width, height);
}

void TupButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
	update();
}

void TupButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	update();
}

void TupButtonItem::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	QGraphicsItem::mouseMoveEvent(event);
	
	emit clicked();
	
	update();
}

void TupButtonItem::setIconSize(const QSize &size)
{
	m_iconSize = size;
	update();
}

void TupButtonItem::setIcon(const QIcon &icon)
{
	m_icon = icon;
	update();
}

void TupButtonItem::setText(const QString &text)
{
	m_text = text;
	update();
}

void TupButtonItem::setFont(const QFont &font)
{
	m_font = font;
	update();
}


