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

#ifndef KWIDGETLISTVIEW_H
#define KWIDGETLISTVIEW_H

#include <QTableWidget>
#include <QMap>
#include "kglobal.h"

/**
 * @if spanish
 *     La clase KWidgetListView provee una lista de widgets basada en items
 *     @brief Provee una lista de widgets
 * @endif
 * @author David Cuadrado <krawek@gmail.com>
*/
class K_GUI_EXPORT KWidgetListView : public QTableWidget
{
    Q_OBJECT
    public:
        /**
          * @if spanish
          * Constructor por defecto
          * @endif
          * @param parent 
          * @return 
          */
          KWidgetListView(QWidget * parent = 0);

        /**
          * @if spanish
          * Destructor
          * @endif
          * @return 
          */
          ~KWidgetListView();

        /**
          * @if spanish
          * Añade un widget a la lista, retorna el item contenedor por conveniencia
          * @endif
          * @param widget 
          * @return 
          */
          QTableWidgetItem *addWidget(QWidget *widget);

        /**
          * @if spanish
          * Inserta un widget en la lista en la posicicion dada
          * @endif
          * @param pos 
          * @param widget 
          * @return 
          */
          QTableWidgetItem *insertWidget(int pos, QWidget *widget);

        /**
          * @if spanish
          * Retorna el widget asociado al item
          * @endif
          * @param treeItem 
          * @return 
          */
          QWidget *widget(QTableWidgetItem *treeItem);

        /**
          * @if spanish
          * Retorna el item asociado al widget
          * @endif
          * @param widget 
          * @return 
          */
          QTableWidgetItem *item(QWidget *widget);

        /**
          * @if spanish
          * Mueve el item actual un posicion hacia arriba si es posible
          * @endif
          * @param index 
          */
          void moveItemUp(int index);

        /**
          * @if spanish
          * Mueve el item actual hacia abajo si es posible
          * @endif
          * @param index 
          */
          void moveItemDown(int index);

        /**
          * @if spanish
          * Retorna la posicicion visual de la fila actual
          * @endif
          * @return 
          */
          int currentVisualRow() const;

    protected:
        /**
          * @if spanish
          * Reimplementado de QWidget
          * @endif
          * @param e 
          */
          void resizeEvent(QResizeEvent *e);

    signals:
        /**
          * @if spanish
          * Este signal se emite cuando un item ha sido seleccionado
          * @endif
          * @param index 
          */
          void itemSelected(int index);

    private:
        QMap<QWidget *, QTableWidgetItem *> m_items;
};

#endif
