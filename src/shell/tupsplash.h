/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#ifndef TUPSPLASH_H
#define TUPSPLASH_H

/**
 * @author Fernando Roldan - David Cuadrado
 * @file splash.h
 * @brief Include this file if you need the class TupSplash
 */

#include "tapplicationproperties.h"

#ifdef K_DEBUG

#ifdef Q_OS_WIN
#include <QDebug>
#else
#include "tdebug.h"
#endif

#endif

#include <QPainter>
#include <QSplashScreen>
#include <QTimer>

/**
 * @brief Class that handles the application's splash screen
 *
 * <b>Date of Creation: July 28 - 2004.</b>\n
 * This is a widget that it is shown at the application's startup.
*/
class TupSplash : public QSplashScreen
{
    Q_OBJECT

    public:
        /**
         * @brief Default Constructor
         *
         * Constructs a TupSplash object.
         */
         TupSplash();
         /**
          * @brief Default Destructor
          *
          * Destroys the TupSplash object.
          */
         ~TupSplash();

         void setMessage(const QString &msg);

    private slots:
         void animate();

    private:
         QString message;
         QString version;
         int state;
         int counter;
         int pos[3];
         bool flag;
         //QTimer *timer;

    protected:
         void drawContents(QPainter * painter);
};

#endif
