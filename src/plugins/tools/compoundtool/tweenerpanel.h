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

#ifndef TWEENERPANEL_H
#define TWEENERPANEL_H

#include <QWidget>
#include <QGraphicsPathItem>

#include "ktitemtweener.h"

/**
 * @author Gustav Gonzalez 
*/

class TweenerPanel : public QWidget 
{
    Q_OBJECT

    public:

        enum TweenerType { 
             Position = 0,
             Rotation = 1,
             Scale = 2,
             Shear = 3,
             Opacity = 4,
             Coloring = 5,
             Compound = 6
        };

        enum Mode { Add = 1, Edit, View };
        enum EditMode { Selection = 0, TweenList, TweenProperties, None };

        TweenerPanel(QWidget *parent = 0);
        ~TweenerPanel();

        void setParameters(const QString &name, int framesTotal, int startFrame);
        void setParameters(KTItemTweener *currentTween);
        void activateMode(TweenerPanel::EditMode mode);
        void notifySelection(bool flag);
        void updateSteps(const QGraphicsPathItem *path, QPointF offset);
        void initStartCombo(int framesTotal, int currentFrame);
        int startComboSize();
        QString tweenToXml(int currentFrame, QPointF point);
        int totalSteps();
        QString currentTweenName() const;
        void cleanPositionData();

    private slots:
        void emitOptionChanged(int option);
        void showTweenSettings(int tweenType);
        void activateTweenersTable(TweenerPanel::TweenerType type, const QString &message);
        void updateTweenersTable(TweenerPanel::Mode mode);
        void applyTween();
        void enableApplyButton(bool flag);
        
    signals:
        void clickedSelect();
        void clickedTweenProperties();
        void clickedApplyTween();
        void clickedResetTween();
        void tweenPropertiesActivated(TweenerPanel::TweenerType flag);
        void startingPointChanged(int index);
        void loadPath(bool flag);
        
    private:
        void setOptionsPanel();
        void activeOptionsPanel(bool enable);
        void setTweenerTableForm();
        void activeTweenerTableForm(bool enable);
        void setButtonsPanel();
        void activeButtonsPanel(bool enable);
        void loadTweenComponents();
        void activeTweenComponent(int index, bool enable);
        void setEditMode();

        struct Private;
        Private *const k;
};

#endif
