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

#ifndef TUPXMLPARSERBASE_H
#define TUPXMLPARSERBASE_H

#include <QXmlDefaultHandler>

/**
 * @author David Cuadrado
*/

class TupXmlParserBase : public QXmlDefaultHandler
{
    public:
        ~TupXmlParserBase();

    protected:
        TupXmlParserBase();

        bool startDocument();
        bool endDocument();
        bool startElement(const QString& , const QString& , const QString& qname, const QXmlAttributes& atts);
        bool characters(const QString & ch);
        bool endElement( const QString& ns, const QString& localname, const QString& qname);
        bool error(const QXmlParseException & exception);
        bool fatalError(const QXmlParseException & exception);

    protected:
        void setReadText(bool read);
        void setIgnore(bool ignore);

    public:
        virtual void initialize();
        virtual bool startTag(const QString &tag, const QXmlAttributes &atts) = 0;
        virtual bool endTag(const QString &tag) = 0;
        virtual void text(const QString &text) = 0;

    public:
        QString currentTag() const;
        QString root() const;
        bool parse(const QString &doc);
        bool parse(QFile *file);

    private:
        struct Private;
        Private *const k;
};

#endif
