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

#ifndef TIPDIALOG_H
#define TIPDIALOG_H

#include "tglobal.h"
#include "tipdatabase.h"
#include "tseparator.h"
#include "tconfig.h"
#include "tapplicationproperties.h"

#include <QTabWidget>
#include <QString>
#include <QList>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QTextFrame>
#include <QTextFrameFormat>
#include <QCheckBox>
#include <QPushButton>
#include <QDomDocument>
#include <QFile>
#include <QDir>

class QCheckBox;
class QTextBrowser;

/**
 * @class TipDialog
 * @author David Cuadrado
 */

class T_GUI_EXPORT TipDialog : public QDialog
{
    Q_OBJECT

    public:
        enum Tab { Animation = 0, Tip };
        TipDialog(QStringList &labels, const QString &videos, const QString &tips, QWidget *parent = 0);
        ~TipDialog();
        
    private:
        void setupGUI();

        void previousVideo();
        void previousTip();
        void nextVideo();
        void nextTip();
        
    private slots:
        void updateTabIndex(int index);
        void showPreviousItem();
        void showNextItem();
        
    private:
        QTabWidget *tabWidget;
        QTextBrowser *videoBrowser;
        QTextBrowser *textBrowser;
        TipDatabase *recordsDatabase;
        QStringList tags;
        int tabIndex;
};

#endif
