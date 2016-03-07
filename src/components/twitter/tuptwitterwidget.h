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

#ifndef TUPTWITTERWIDGET_H
#define TUPTWITTERWIDGET_H

#include "tglobal.h"
#include "tuptwitter.h"

#include <QWidget>
#include <QSplitter>
#include <QTextBrowser>
#include <QTextDocument>
#include <QBoxLayout>
#include <QIcon>
#include <QMenu>
#include <QMouseEvent>

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupTwitterWidget : public QWidget
{
    Q_OBJECT

    public:
        TupTwitterWidget(QWidget *parent);
        ~TupTwitterWidget();

    signals:
        void newPerspective(int index);

    public slots:
        void setSource(const QString &filePath);

    protected:
        void keyPressEvent(QKeyEvent *event); 

    private:
        struct Private;
        Private *const k;
};

#endif
