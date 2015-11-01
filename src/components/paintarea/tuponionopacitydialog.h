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

#ifndef TUPONIONOPACITYDIALOG_H
#define TUPONIONOPACITYDIALOG_H

#include "tglobal.h"
#include "tapplicationproperties.h"
#include "tseparator.h"
#include "tuppenthicknesswidget.h"
#include "timagebutton.h"

#include <QDialog>
#include <QColor>
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <cmath>

class TUPI_EXPORT TupOnionOpacityDialog : public QDialog
{
    Q_OBJECT

    public:
        TupOnionOpacityDialog(const QColor &color, double opacity, QWidget *parent);
        ~TupOnionOpacityDialog();

        QSize sizeHint() const;

    signals:
        void updateOpacity(double);

    private slots:
        void fivePointsLess();
        void onePointLess();
        void onePointMore();
        void fivePointsMore();

    private:
        void setOpacityCanvas();
        void setButtonsPanel();
        void modifySize(double value);

        struct Private;
        Private *const k;
};

#endif
