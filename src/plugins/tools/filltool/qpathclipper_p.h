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

#ifndef QPATHCLIPPER_P_H
#define QPATHCLIPPER_P_H

#include <QtGui/qpainterpath.h>
#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_MODULE(Gui)

class PathVertex;

class Q_GUI_EXPORT QPathClipper
{
public:
    enum Operation {
        BoolAnd,
        BoolOr,
        BoolSub,
        BoolInSub
    };
public:
    QPathClipper();
    QPathClipper(const QPainterPath &subject,
                 const QPainterPath &clip);
    ~QPathClipper();

    void setSubjectPath(const QPainterPath &path);
    QPainterPath subjectPath() const;

    void setClipPath(const QPainterPath &path);
    QPainterPath clipPath() const;

    QPainterPath clip(Operation op=BoolAnd);
    bool intersect();
    bool contains();
private:
    Q_DISABLE_COPY(QPathClipper)
    class Private;
    Private *k;
};

QT_END_HEADER

#endif // QPATHCLIPPER_P_H
