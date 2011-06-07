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

#ifndef KSQUEEZELABEL_H
#define KSQUEEZELABEL_H

#include <qlabel.h>
#include "tglobal.h"

/**
 * @if english
 * Class inspired in KSqueezedTextLabel from kde libraries
 * 
 * from kde documentation:
 * 
 * A label class that squeezes its text into the label
 * If the text is too long to fit into the label it is divided into remaining left and right parts which are separated by three dots.
 * 
 * @elseif spanish
 * Esta clase esta inspirada en KSqueezedTextLabel de las librerias KDE
 * 
 * De la documentacion de kde:
 * 
 * Una clase label que corta su texto en el label
 * Si el texto es muy largo para ser llenado en el label es dividido en el espacio restante que son separadas por tres puntos.
 * 
 * @endif
 * @author David Cuadrado <krawek@gmail.com>
*/

class K_GUI_EXPORT KSqueezeLabel : public QLabel
{
   public:
      KSqueezeLabel(QWidget *parent, const char *name=0);
      KSqueezeLabel(const QString &text, QWidget *parent, const char *name=0);
      ~KSqueezeLabel();
      
      QSize sizeHint() const;
      QSize minimumSizeHint() const;
      void setText(const QString &text);
      void setAlignment(Qt::Alignment alignment);
      QString completeText() const;
      
   protected:
      virtual void squeezeText();
      void resizeEvent(QResizeEvent *);
      
   private:
      QString squeezer(const QString &s, const QFontMetrics& fm, uint width);
      QString m_text;
};

#endif
