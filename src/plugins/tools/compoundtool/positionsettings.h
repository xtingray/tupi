/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#ifndef POSITIONSETTINGS_H
#define POSITIONSETTINGS_H

#include <QWidget>

class QGraphicsPathItem;
class KTItemTweener;

/**
 * @author Gustav Gonzalez 
*/

class PositionSettings : public QWidget 
{
    Q_OBJECT

    public:
        enum Mode { Add = 1, Edit, View };
        enum EditMode { Selection = 0, Path, None };

        PositionSettings(QWidget *parent = 0);
        ~PositionSettings();

        void setParameters(const QString &name, int framesTotal, int startFrame);
        void setParameters(KTItemTweener *currentTween);
        void initStartCombo(int totalFrames, int currentIndex);
        void setStartFrame(int currentIndex);
        int startFrame();

        void updateSteps(const QGraphicsPathItem *path);
        QString tweenToXml(int currentFrame, QPointF point, QString &path);
        int totalSteps();
        void activatePathMode();
        void activateSelectionMode();
        void cleanData();
        void notifySelection(bool flag);
        int startComboSize();
        QString currentTweenName() const;
        
    private slots:
        void emitOptionChanged(int option);
        // void addTween();
        void applyTween();
        
    signals:
        void clickedCreatePath();
        void clickedSelect();
        void clickedResetTween();
        void clickedApplyTween();
        void startingPointChanged(int);
        
    private:
        void setInnerForm();
        void activeInnerForm(bool enable);
        void setEditMode();
        struct Private;
        Private *const k;
};

#endif
