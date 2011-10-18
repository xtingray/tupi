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

#ifndef KTGRADIENTSELECTOR_H
#define KTGRADIENTSELECTOR_H

#include "ktglobal.h"

#include <QAbstractSlider>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QLinearGradient>
#include <QPainterPath>

#include <QList>

/**
 * @if english
 * @short This class provides a simple graphical interface to generate "GradientStops"
 * @elseif spanish
 * @short Esta clase provee de una simple interfaz grafica para generar "GradientStops".
 * @endif
 * @author Jorge Cuadrado
 */

class TUPI_EXPORT KTGradientSelector : public QAbstractSlider
{
   Q_OBJECT
   
   private:

      class KGradientArrow : public QObject
      {
         public:

            KGradientArrow(QPoint pos, const QColor& color, QObject *parent = 0) : QObject(parent), m_color(color)
            {
               QPolygon array(6);
               array.setPoint(0, pos.x() + 0, pos.y() + 0);
               array.setPoint(1, pos.x() + 5, pos.y() + 5);
               array.setPoint(2, pos.x() + 5, pos.y() + 9);
               array.setPoint(3, pos.x() - 5, pos.y() + 9);
               array.setPoint(4, pos.x() - 5, pos.y() + 5);
               array.setPoint(5, pos.x() + 0, pos.y() + 0);

               m_form.addPolygon(array);
            }

            ~KGradientArrow(){}

            double position() 
            {
               return m_form.currentPosition().x();
            }

            bool contains(const QPoint &pt)
            {
               return m_form.contains(pt);
            }

            void moveArrow(const QPoint &pos)
            {   
               QMatrix matrix;
               matrix.translate(pos.x() - m_form.currentPosition().x(), 0);
   
               m_form = matrix.map(m_form);
            }

            QPainterPath form()
            {
               return m_form;
            }

            QColor color() const
            {
               return m_color;
            }

            void setColor(const QColor &color)
            {
               m_color = color;
            }

            void moveVertical(const QPoint &pos)
            {
               QMatrix matrix;
               matrix.translate(0, pos.y() - m_form.currentPosition().y());
   
               m_form = matrix.map(m_form);
            }

            QPainterPath m_form;
            QColor m_color;
      };
      
   public:

      /**
       * @if english
       * Translate
       * @elseif spanish
       * Constructor por defecto.
       * @endif
       */
      KTGradientSelector(QWidget *parent=0);
      
      /**
       * @if english
       * Translate
       * @elseif spanish
       * Crea un KTGradientSelector, con una orientacion definida.
       * @endif
       * @see Qt::Orientation
       */
      KTGradientSelector(Qt::Orientation o, QWidget *parent = 0);
      /**
       * Destructor
       */
      ~KTGradientSelector();
      /**
       * @if english
       * Translate
       * @elseif spanish
       * Pone un GradienStops, para ser editado.
       * @endif
       */
      void setStops(const QGradientStops &);
      
      Qt::Orientation orientation() const
      {   
         return _orientation; 
      }

      QRect contentsRect() const;

      void setValue(int value)
      {
         QAbstractSlider::setValue(value);
      }

      int value() const
      {
         return QAbstractSlider
            ::value();
      }

      /**
       * @if english
       * Translate
       * @elseif spanish
       * Pone el numero maximo de flechas, que definen el "GradienStops".
       * @endif
       */
      void setMaxArrows(int value);

      /**
       * @if english
       * Translate
       * @elseif spanish
       * Devuelve el "GradienStops" actual.
       * @endif
       */
      QGradientStops  gradientStops() const
      {
         return m_gradient.stops();
      }

      /**
       * @if english
       * Translate
       * @elseif spanish
       * Crea el "GradienStops" actual apartir de las flechas acutales.
       * @endif
       */
      void createGradient();
      
      /**
       * @if english
       * Translate
       * @elseif spanish
       * Cambia el color de la flecha actualmente seleccionada.
       * @endif
       */
      void setCurrentColor(const QColor& color);

   signals:
      void newValue(int value);
      void gradientChanged(const QGradientStops&);
      void arrowAdded();
      
   public slots:
      void addArrow(QPoint position, QColor color);
      virtual void valueChange(int value);

   protected:
      virtual void drawContents(QPainter *);
      
      virtual void paintEvent(QPaintEvent *);
      virtual void mousePressEvent(QMouseEvent *event);
      virtual void mouseMoveEvent(QMouseEvent *event);
      virtual void wheelEvent(QWheelEvent *event);

      virtual QSize minimumSize() const
      { 
         return sizeHint(); 
      }

      virtual void resizeEvent(QResizeEvent * event);

      virtual QSize sizeHint() const
      {
         return QSize(width(), 35);
      }
      
   private:
      QPoint calcArrowPos(int val);
      void moveArrow(const QPoint &pos);
      double valueToGradient(int _value) const;
      void init();

   private:
      Qt::Orientation _orientation;
      int m_currentArrowIndex;
      QLinearGradient m_gradient;
      QList<KGradientArrow*> m_arrows;
      bool m_update;
      int m_maxArrows;
      QColor m_currentColor;
};

#endif
