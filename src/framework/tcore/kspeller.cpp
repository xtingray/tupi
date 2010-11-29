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


#include "kspeller.h"
#include "kaspellchecker.h"

#include <QtDebug>

KSpeller::KSpeller(QObject *parent) : QObject(parent), m_speller(0)
{
#if defined(HAVE_ASPELL)
    m_speller = new KAspellChecker;
#else
#warning NO SPELL MACROS DEFINED!
#endif
}

KSpeller::~KSpeller()
{
    if (m_speller) 
        delete m_speller;
}

bool KSpeller::checkWord(const QString &word)
{
    if (m_speller)
        return m_speller->checkWord(removeExtraCharacters(word));

    return false;
}

QStringList KSpeller::suggestions(const QString &word)
{
    if (m_speller)
        return m_speller->suggestions(removeExtraCharacters(word));
    
    return QStringList();
}

QString KSpeller::removeExtraCharacters(const QString &str)
{
    QString result = str;
    
    if (!result[0].isLetterOrNumber())
        result.remove(0,1);
    
    if (!result[result.length()-1].isLetterOrNumber())
        result.remove(result.length()-1,1);
    
    return result;
}
