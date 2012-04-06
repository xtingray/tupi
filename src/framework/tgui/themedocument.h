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
 *   TooN's versions:                                                     * 
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

#ifndef THEMEDOCUMENT_H
#define THEMEDOCUMENT_H

#include "tglobal.h"

#include <QMap>
#include <QStringList>
#include <qdom.h>

typedef QMap<QString, QString> ThemeKey;

/**
 * @author David Cuadrado
 * <?xml version = '1.0' encoding = 'UTF-8'?>
 * @verbatim
 * <Theme name="" version="" >
 * <General>
 * <Text color="#000000" />
 *     <Base color="#EFF0FF" />
 *     <Foreground color="#3e3e45" />
 *     <Background color="#556202" />
 *     <Button color="#B7B6AB" />
 *     <ButtonText color="#3e3e45" />
 * </General>
 * <Effects>
 *     <Light color="" /> <!--QColorGroup::Light - lighter than Button color. -->
 *     <Midlight color="#070707" /> <!-- QColorGroup::Midlight - between Button and Light.--> 
 *     <Dark color="" />
 *     <Mid color="" />
 * </Effects>
 * <Selections>
 *     <Highlight color="#3B6886" />
 *     <HighlightedText color="#EFEDDF" />
 * </Selections>
 * <TextEffects>
 *     <BrightText color="" />
 *     <Link color="" />
 *     <LinkVisited color="" />
 * </TextEffects>
 * </Theme>
 * @endverbatim
*/

class T_GUI_EXPORT ThemeDocument : public QDomDocument
{
    public:
        ThemeDocument();
        ThemeDocument(const QString &name, const QString &version);
        ~ThemeDocument();
        void addGeneralSection(ThemeKey tk);
        void addEffectsSection(ThemeKey tk);
        void addSelections(ThemeKey tk);
        void addTextEffect(ThemeKey tk);
};

#endif
