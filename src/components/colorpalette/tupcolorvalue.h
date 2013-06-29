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

#ifndef TUPVALUECOLOR_H
#define TUPVALUECOLOR_H

#include <QFrame>

/**
 * @author Jorge Cuadrado
**/

class TupItemColorValue : public QFrame
{
    Q_OBJECT

    public:
        TupItemColorValue( const QString &text = QString::null, QWidget *parent = 0);
        ~TupItemColorValue();
        int  value();
        void setMax(int);
        void setRange(int minimum, int maximum);
        void setSuffix(const QString &suffix);

    private:
        struct Private;
        Private *const k;

    public slots:
        void setValue(int val);

    signals:
        void valueChanged(int i);
        void valueChanged(const QString &text);
        void editingFinished();
};

class TupColorValue : public QFrame
{
    Q_OBJECT

    public:
        TupColorValue(QWidget *parent = 0);
        ~TupColorValue();
       int hue();
       int saturation();
       int value();
       int alpha();

    private:
       struct Private;
       Private *const k;

    private:
       void setupForm();

    public slots:
       void setColor(const QBrush & brush);

    private slots:
       void syncValuesRgb(int value = 0);

    signals:
       void brushChanged(const QBrush &);
       void hueChanged(int);
       void saturationChanged(int);
       void valueChanged(int);
};

#endif
