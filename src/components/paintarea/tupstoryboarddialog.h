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

#ifndef TUPSTORYBOARDDIALOG_H
#define TUPSTORYBOARDDIALOG_H

#include "tupscene.h"
#include "tupstoryboard.h"
#include "tupexportinterface.h"

#include <QDialog>
#include <QColor>
#include <QSize>
#include <QIcon>
#include <QListWidgetItem>

class TupStoryBoardDialog : public QDialog
{
    Q_OBJECT

    public:
        TupStoryBoardDialog(bool isNetworked, TupExportInterface *imagePlugin, const QColor &color, 
                            const QSize &size, TupScene *scene, QWidget *parent);
        ~TupStoryBoardDialog();

    private slots:
        void updateForm(QListWidgetItem *current, QListWidgetItem *previous);
        void saveStoryBoard();
        void exportStoryBoard();
        void postStoryBoard();
        void closeDialog();

    signals:
        void saveStoryboard(TupStoryboard *);
        void postStoryboard(TupStoryboard *);

    private:
        void setListComponent();
        void setPreviewScreen();
        void setStoryForm();
        void setSceneForm();

        void thumbnailGenerator();
        void addScene(const QString &label, const QIcon &icon);
 
        struct Private;
        Private *const k;
};

#endif
