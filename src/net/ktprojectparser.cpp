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

#include "ktprojectparser.h"
#include "kdebug.h"

struct KTProjectParser::Private
{
	QByteArray data;
};

KTProjectParser::KTProjectParser(): KTXmlParserBase() , k( new Private())
{
}


KTProjectParser::~KTProjectParser()
{
}


bool KTProjectParser::startTag(const QString &tag, const QXmlAttributes &atts)
{
	if(root() == "project")
	{
		if(tag == "data")
		{
			setReadText(true);
		}
	}
	return true;
}

bool KTProjectParser::endTag(const QString &tag)
{
	Q_UNUSED(tag);
	return true;
	
}

void KTProjectParser::text(const QString &text)
{
	if ( currentTag() == "data" )
	{
		k->data = QByteArray::fromBase64(text.toLocal8Bit());
	}
}

QByteArray KTProjectParser::data()
{
	return k->data;
}
