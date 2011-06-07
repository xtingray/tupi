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

#ifndef KTIP_H
#define KTIP_H

#include <QString>
#include <QList>
#include <QDialog>

#include "tglobal.h"
#include "ktipdatabase.h"

class QCheckBox;
class QTextBrowser;

/**
 * @class KTipDialog
 * @author David Cuadrado <krawek@gmail.com>
 */

class K_GUI_EXPORT KTipDialog : public QDialog
{
    Q_OBJECT;
    public:
        KTipDialog(QStringList &labels, const QString &file, QWidget *parent = 0);
        KTipDialog(QStringList &labels, KTipDatabase *database, QWidget *parent = 0);
        ~KTipDialog();
        
    private:
        void setupGUI();
        
    private slots:
        void showPrevTip();
        void showNextTip();
        
        void setShowOnStart();
        
    private:
        QTextBrowser *textBrowser;
        QCheckBox *m_showOnStart;
        KTipDatabase *m_database;
        QStringList tags;
};

#endif
