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

#ifndef KVALIDATES_H
#define KVALIDATES_H

#include <QWidget>
class QLineEdit;

#define K_GIVE_NAME(obj) obj->setObjectName(#obj);

/**
 * @author David Cuadrado
*/

class KFormValidator
{
    public:
        KFormValidator(QWidget *form);
        virtual ~KFormValidator();
        
        void setForm(QWidget *form);
        
        bool validatesNumerically(bool real);
        bool validatesRange(int i, int e);
        bool validatesRegExp( const QString &regexp);
        bool validatesLength(int max);
        bool validatesMask(const QString &mask);
        
        bool validatesMaskOf(const QString &mask, const QString &name);
        bool validatesLengthOf(int max, const QString &name);
        bool validatesNumericallyOf(bool real, const QString &name);
        bool validatesRangeOf(int i, int e, const QString &name);
        bool validatesRegExpOf( const QString &regexp, const QString &name);
        
        void validatesMaskOf(const QString &mask, QLineEdit *line);
        void validatesLengthOf(int max, QLineEdit *line);
        void validatesNumericallyOf(bool real, QLineEdit *line);
        void validatesRangeOf(int i, int e, QLineEdit *line);
        void validatesRegExpOf(const QString &regexp, QLineEdit *line);
        
    protected:
        virtual bool validate(QLineEdit *line);
        
    public slots:
        bool validate();
        
    private:
        QWidget *m_parent;
        QList<QLineEdit *> m_childs;
};

#endif

