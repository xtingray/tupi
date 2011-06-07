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

#ifndef KELABEL_H
#define KELABEL_H

#include "ksqueezelabel.h"
#include "tglobal.h"

#include <QLineEdit>
#include <QMouseEvent>

/**
 * @author David Cuadrado <krawek@gmail.com>
*/

class K_GUI_EXPORT KELabel : public KSqueezeLabel
{
    Q_OBJECT

    public:
        KELabel(const QString &text, QWidget *parent = 0);
        ~KELabel();
        void setValidator(const QValidator * v);
        
    public slots:
        void edit();
        void clearFocus();
        
    private slots:
        void applyText();
        
    signals:
        void edited(const QString &);
        
    protected:
        virtual void mouseDoubleClickEvent(QMouseEvent *mouse_event);
        
    private:
        QLineEdit *m_editor;
};

#endif
