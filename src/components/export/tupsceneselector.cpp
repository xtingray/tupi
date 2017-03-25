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

#include "tupsceneselector.h"

TupSceneSelector::TupSceneSelector(const TupExportWidget *widget) : TupExportWizardPage(tr("Select Scenes"))
{
    setTag("SCENE");
    m_selector = new TItemSelector;

    connect(m_selector, SIGNAL(changed()), this, SLOT(updateState()));
    connect(widget, SIGNAL(updateScenes()), this, SLOT(updateScenesList()));

    setWidget(m_selector);
}

TupSceneSelector::~TupSceneSelector()
{
}

bool TupSceneSelector::isComplete() const
{
    return m_selector->selectedItems().count() > 0;
}

void TupSceneSelector::reset()
{
}

void TupSceneSelector::setScenes(const QList<TupScene *> &scenes)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupSceneSelector::setScenes()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    m_selector->clear();

    if (scenes.count() > 1) {
        int pos = 1;
        foreach (TupScene *scene, scenes) {
                 #ifdef K_DEBUG
                     QString msg = "TupSceneSelector::setScenes() - Adding " + scene->sceneName();
                     #ifdef Q_OS_WIN
                         qWarning() << msg;
                     #else
                         tWarning("export") << msg;
                     #endif
                 #endif

                 m_selector->addItem(QString("%1: ").arg(pos) + scene->sceneName());
                 pos++;
        }

        #ifdef K_DEBUG
            QString msg = "TupSceneSelector::setScenes() - Available Scenes: " + QString::number(pos - 1);
            #ifdef Q_OS_WIN
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif

        m_selector->selectFirstItem();
    } else {
        TupScene *scene = scenes.first();
        m_selector->addItem(QString("1: ") + scene->sceneName());
        m_selector->addSelectedItem(QString("1: ") + scene->sceneName());
    }
}

void TupSceneSelector::aboutToNextPage()
{
    emit selectedScenes(m_selector->selectedIndexes());
}

void TupSceneSelector::updateState()
{
    emit completed();
}

void TupSceneSelector::updateScenesList()
{
    // SQA: Pending code right over here
}

