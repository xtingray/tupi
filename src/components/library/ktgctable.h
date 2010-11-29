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

#ifndef KTGCTABLE_H
#define KTGCTABLE_H

#include "kttreedelegate.h"

#include <QKeyEvent>
//#include <QTreeWidget>

#include "ktreelistwidget.h"

/**
 * @author David Cuadrado <krawek@toonka.com>
*/

//class KTGCTable : public QTreeWidget

class KTGCTable : public KTreeListWidget
{
    Q_OBJECT

    public:
        KTGCTable(QWidget *parent = 0);
        ~KTGCTable();
        QTreeWidgetItem *currentFolder();
        void setCurrentFolder(QTreeWidgetItem *cf);
        void removeCurrentFolder();
        void mousePressEvent(QMouseEvent *event);
        void mouseDoubleClickEvent(QMouseEvent *event);
        bool isFolder(QTreeWidgetItem *item);
        int indexOf(QTreeWidgetItem *item);
        QString oldFolder();
        QTreeWidgetItem *getFolder(const QString &folderName);

        enum ObjectType 
        {
            Item = 0,
            Folder
        };

    signals:
        void itemSelected(QTreeWidgetItem *);
        void itemRemoved();
        void itemRenamed(QTreeWidgetItem *);
        void itemMoved(QString node, QString target);

    public slots:
        void createFolder(const QString &name = QString());

    private slots:
        void callRename();
        void callInkscape();
        void callGimp();

    protected:
        void dropEvent(QDropEvent *event);
        void dragEnterEvent(QDragEnterEvent *event);
        //void dragLeaveEvent(QDragLeaveEvent *event);
        void dragMoveEvent(QDragMoveEvent *event);
        void keyPressEvent(QKeyEvent * event);

    private:
        QTreeWidgetItem *m_currentFolder;
        int foldersTotal;
        QString folderName; 
        QString parentNode;
        QList<QTreeWidgetItem *> nodeChildren;
        typedef QList<QTreeWidgetItem *> Lists;
        // QHash<int, Lists> deepChildren;
};

#endif
