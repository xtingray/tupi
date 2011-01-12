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

#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QFrame>

class QGraphicsPathItem;
/**
 * @author Jorge Cuadrado <kuadrosx@toonka.com>
*/
class Configurator : public QFrame
{
    Q_OBJECT
    public:
        Configurator(QWidget *parent = 0);
        ~Configurator();

        void initCombo(int framesTotal, int currentIndex);
        void setComboIndex(int currentIndex);
        void updateSteps(const QGraphicsPathItem *path);
        QString tweenToXml(int currentFrame, QString path);
        int totalSteps();
        void activatePathMode();
        void activateSelectionMode();
        void cleanData();
        QString currentTweenName() const;
        
    private slots:
        void emitOptionChanged(int option);
        void addTween();
        
    signals:
        void clickedCreatePath();
        void clickedSelect();
        void clickedResetTweener();
        void clickedApplyTweener();
        
    private:
        struct Private;
        Private *const k;
};

#endif
