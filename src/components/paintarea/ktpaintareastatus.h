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

#ifndef KTPAINTAREASTATUS_H
#define KTPAINTAREASTATUS_H

#include <QStatusBar>

class KTViewDocument;

/**
 * @author David Cuadrado
*/

class KTPaintAreaStatus : public QStatusBar
{
    Q_OBJECT

    public:
        KTPaintAreaStatus(KTViewDocument *parent);
        ~KTPaintAreaStatus();
        void updateTool(const QString &label, const QPixmap &pixmap);
        void setZoomFactor(const QString &text);
        void updateZoomFactor(double factor);
        qreal currentZoomFactor();
        void setBgColor(QColor color);

    public slots:
        void applyZoom(const QString &text);
        void setPen(const QPen &pen);

    private slots:
        void selectAntialiasingHint();
        void selectRenderer(int id);
        void applyRotation(const QString &text);
        void updateFrameIndex(int index);
        void updateFramePointer();

    signals:
        void colorRequested();
        void colorUpdated(const QColor color);
        void newFramePointer(int index);

    private:
        void updateZoomField(const QString &text);
        struct Private;
        Private *const k;
};

#endif
