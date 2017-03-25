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

#ifndef TUPPAINTAREASTATUS_H
#define TUPPAINTAREASTATUS_H

#include "tglobal.h"
#include "tupdocumentview.h"
#include "tseparator.h"
#include "tupbrushmanager.h"
// #include "tupcolorwidget.h"
#include "tcolorcell.h"
#include "tupbrushstatus.h"
#include "tuptoolstatus.h"

#include <QStatusBar>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QObject>
#include <QLineEdit>
#include <QDir>

/**
 * @author David Cuadrado
*/

class TUPI_EXPORT TupPaintAreaStatus : public QStatusBar
{
    Q_OBJECT

    public:
        TupPaintAreaStatus(TupDocumentView *parent);
        ~TupPaintAreaStatus();
        void updateTool(const QString &label, const QPixmap &pixmap);
        void setZoomPercent(const QString &percent);
        void setRotationAngle(const QString &angle);
        void updateZoomFactor(double factor);
        qreal currentZoomFactor();
        void updateRotationAngle(int angle);
        void enableFullScreenFeature(bool flag);
        void updatePosition(const QString &position);

    public slots:
        void applyZoom(const QString &text);
        void setPen(const QPen &pen);
        void setBrush(const QBrush  &brush);
        void applyRotation(const QString &text);

    private slots:
        void selectAntialiasingHint();
        // void selectRenderer(int id);
        void updateFrameIndex(int index);
        void updateFramePointer();

    signals:
        void newFramePointer(int index);

    private:
        void updateZoomField(const QString &text);
        void updateRotationField(const QString &angle);
        struct Private;
        Private *const k;
};

#endif
