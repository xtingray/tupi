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

#include "txmlparserbase.h"

struct TXmlParserBase::Private
{
    QString currentTag;
    QString root;
    bool isParsing;
    bool readText;
    bool ignore;
    QString document;
};

TXmlParserBase::TXmlParserBase() : QXmlDefaultHandler(), k(new Private)
{
}

TXmlParserBase::~TXmlParserBase()
{
    delete k;
}

void TXmlParserBase::initialize()
{
}

bool TXmlParserBase::startDocument()
{
    k->isParsing = true;
    k->currentTag = QString();
    k->root = QString();
    k->readText = false;
    k->ignore = false;
    
    initialize();

    return true;
}

bool TXmlParserBase::endDocument()
{
    k->isParsing = false;
    return true;
}

bool TXmlParserBase::startElement(const QString& , const QString& , const QString& qname, const QXmlAttributes& atts)
{
    if (k->ignore) 
        return true;
    
    if (k->root.isEmpty())
        k->root = qname;
    
    bool r = startTag(qname, atts);
    
    k->currentTag = qname;
    
    return r;
}


bool TXmlParserBase::endElement(const QString&, const QString& , const QString& qname)
{
    return endTag(qname);
}


bool TXmlParserBase::characters(const QString & ch)
{
    if (k->ignore) 
        return true;
    
    if (k->readText) {
        text(ch.simplified());
        k->readText = false;
    }
    
    return true;
}

bool TXmlParserBase::error(const QXmlParseException & exception)
{
#ifdef K_DEBUG	
    #ifdef Q_OS_WIN
        qWarning() << exception.lineNumber() << "x" << exception.columnNumber() << ": " << exception.message();
        qWarning() << __PRETTY_FUNCTION__ << " Document: " << k->document;
    #else
        tWarning() << exception.lineNumber() << "x" << exception.columnNumber() << ": " << exception.message();
        tWarning() << __PRETTY_FUNCTION__ << " Document: " << k->document;
    #endif
#else
     Q_UNUSED(exception);
#endif

    return true;
}

bool TXmlParserBase::fatalError(const QXmlParseException & exception)
{
#ifdef K_DEBUG
    #ifdef Q_OS_WIN
        qWarning() << exception.lineNumber() << "x" << exception.columnNumber() << ": " << exception.message();
        qWarning() << __PRETTY_FUNCTION__ << " Document: " << k->document;
    #else
        tWarning() << exception.lineNumber() << "x" << exception.columnNumber() << ": " << exception.message();
        tWarning() << __PRETTY_FUNCTION__ << " Document: " << k->document;
    #endif
#else
     Q_UNUSED(exception);
#endif

    return true;
}

void TXmlParserBase::setReadText(bool read)
{
    k->readText = read;
}

void TXmlParserBase::setIgnore(bool ignore)
{
    k->ignore = ignore;
}

QString TXmlParserBase::currentTag() const
{
    return k->currentTag;
}

QString TXmlParserBase::root() const
{
    return k->root;
}

bool TXmlParserBase::parse(const QString &doc)
{
    QXmlSimpleReader reader;
    
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    
    QXmlInputSource xmlsource;
    xmlsource.setData(doc);
    
    k->document = doc;
    
    return reader.parse(&xmlsource);
}

bool TXmlParserBase::parse(QFile *file)
{
    if (!file->isOpen()) {
        if (! file->open(QIODevice::ReadOnly | QIODevice::Text)) {
#ifdef K_DEBUG
            QString msg = "TXmlParserBase::parse() - Error: Cannot open file -> " + file->fileName();
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
#endif

            return false;
        }
    }
    
    return parse(QString::fromLocal8Bit(file->readAll()));
}
