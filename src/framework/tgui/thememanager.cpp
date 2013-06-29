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

#include "thememanager.h"
#include "themedocument.h"
#include "tdebug.h"
#include "tglobal.h"
#include "tapplication.h"

#include <QApplication>

ThemeManager::ThemeManager() : QXmlDefaultHandler()
{
    m_palette = QApplication::palette();
}

ThemeManager::~ThemeManager()
{
}

bool ThemeManager::applyTheme(const QString &file)
{
    bool ok = false;
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    QFile f(file);
    QXmlInputSource xmlsource(&f);

    if (reader.parse(&xmlsource)) {
        ok = true;
    } else {
        tError() <<  QObject::tr("I can't analize the theme file: %1").arg(file) << endl;
        ok = false;
    }
    
    return ok;
}

bool ThemeManager::applyTheme(const ThemeDocument &kd)
{
    // tDebug() << "Applying theme" << endl;

    bool ok = false;
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    QXmlInputSource xmlsource;
    xmlsource.setData(kd.toString());

    if (reader.parse(&xmlsource)) {
        ok = true;
    } else {
        tDebug() << QObject::tr("I can't analize the theme document") << endl;
        ok = false;
    }
    
    return ok;
}

bool ThemeManager::startElement(const QString& , const QString& , const QString& qname, const QXmlAttributes& atts)
{
    m_qname = qname;

    if (m_root.isNull()) {
        m_root = qname;
    } else if (m_root == "Theme") {
               if (qname == "Text") {
                   QColor c = getColor(atts);
                   if (c.isValid())
                       m_palette.setColor( QPalette::Text, c);
               } else if (qname == "Base") {
                          QColor c = getColor(atts);
                          if (c.isValid())
                              m_palette.setColor(QPalette::Base, c);
               } else if (qname == "Foreground") {
                          QColor c = getColor(atts);
                          if (! c.isValid())
                              c = 0xd7d7ef;
                          m_palette.setColor(QPalette::Foreground, c);
               } else if (qname == "Background") {
                          QColor c = getColor(atts);
                          if (c.isValid())
                              m_palette.setColor(QPalette::Background, c);
               } else if (qname == "Button") {
                          QColor c = getColor(atts);
                          if (c.isValid())
                              m_palette.setColor(QPalette::Button, c);
               } else if (qname == "ButtonText") {
                          QColor c = getColor(atts);
                          if (!c.isValid())
                              c = 0xd7d7ef;
                          m_palette.setColor(QPalette::ButtonText, c);
               } else if (qname == "Light") {
                          QColor c = getColor(atts);
                          if (c.isValid())
                              m_palette.setColor(QPalette::Light, c);
               } else if (qname == "Midlight") {
                          QColor c = getColor(atts);
                          if (c.isValid())
                              m_palette.setColor(QPalette::Midlight, c);
                } else if (qname == "Dark") {
                           QColor c = getColor(atts);
                           if (c.isValid())
                               m_palette.setColor(QPalette::Dark, c);
                } else if (qname == "Mid") {
                           QColor c = getColor(atts);
                           if (c.isValid())
                               m_palette.setColor(QPalette::Mid, c);
                } else if (qname == "Highlight") {
                           QColor c = getColor(atts);
                           if (c.isValid())
                               m_palette.setColor(QPalette::Highlight, c);
                } else if (qname == "HighlightedText") {
                           QColor c = getColor(atts);
                           if (c.isValid())
                               m_palette.setColor(QPalette::HighlightedText, c);
                } else if (qname == "BrightText") { 
                           QColor c = getColor(atts);
                           if (c.isValid())
                               m_palette.setColor(QPalette::HighlightedText, c);
                } else if (qname == "Link") {
                           QColor c = getColor(atts);
                           if (c.isValid())
                               m_palette.setColor(QPalette::Link, c);
                } else if (qname == "LinkVisited") {
                           QColor c = getColor(atts);
                           if (c.isValid())
                               m_palette.setColor(QPalette::LinkVisited, c);
                }
    }

    return true;
}

bool ThemeManager::endElement(const QString&, const QString&, const QString& qname)
{
    if (qname == "Theme")
        kApp->applyPalette(m_palette);
    
    return true;
}

bool ThemeManager::characters(const QString &)
{
    return true;
}

bool ThemeManager::error(const QXmlParseException & exception)
{
    tError() << "Error analizing theme: " << exception.message() << endl;

    return false;
}

bool ThemeManager::fatalError(const QXmlParseException & exception)
{
    tError() << "FATAL Error analizing theme: " << endl;
    tError() << "Line: " << exception.lineNumber() << " Column: " << exception.columnNumber() << " " << exception.message() << endl;

    return false;
}

QColor ThemeManager::getColor(const QXmlAttributes& atts)
{
    QColor color(atts.value("color"));

    return color;
}
