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

#include "kspellhighlighter.h"

#include <QtDebug>

KSpellHighlighter::KSpellHighlighter(QTextDocument * parent) : QSyntaxHighlighter(parent)
{
    m_resaltColor = Qt::darkMagenta;
    m_speller = new KSpeller;
}

KSpellHighlighter::~KSpellHighlighter()
{
    delete m_speller;
}

void KSpellHighlighter::setResaltColor(const QColor &color)
{
    m_resaltColor = color;
}

void KSpellHighlighter::highlightBlock(const QString & text)
{
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setForeground(m_resaltColor);
    
    QStringList words = text.split(' ');
    
    foreach (QString w, words) {
    	     if (!m_speller->checkWord(w))
                 setFormat( text.indexOf(w), w.length(), format);
    }
}
