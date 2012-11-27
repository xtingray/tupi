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

#ifndef TupVIEWCOLORCELLS_H
#define TupVIEWCOLORCELLS_H

#include <QFrame>
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QBrush>

#include "tupcellscolor.h"
#include "tuppaletteparser.h"
#include "tapplicationproperties.h"

/**
    @author Jorge Cuadrado
**/

class TupViewColorCells : public QFrame
{
    Q_OBJECT

    public:
        TupViewColorCells(QWidget *parent = 0);
        virtual ~TupViewColorCells();
        void readPaletteFile(const QString &file);
        void setColor(const QBrush & b);

    private:
        struct Private;
        Private *const k;

    private:
        void setupForm();
        void setupButtons();
        void fillDefaultColors();
        void addDefaultColor(int i, int j, const QColor &);
        void fillNamedColor();
        void readPalettes(const QString &paletteDir);
        void addPalette(TupCellsColor *palette);

    protected:

    signals:
        void selectColor(const QBrush &);

    public slots:
        virtual void addCurrentColor();
        virtual void removeCurrentColor();
        virtual void addPalette(const QString & name, const QList<QBrush> & brushes, bool editable);
        void changeColor(QTableWidgetItem*);
};

#endif
