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

#include "ktgctable.h"

#include <QHeaderView>
#include <QMenu>

#include "kglobal.h"
#include "kdebug.h"
#include "kapplication.h"

/**
 * This class provides some methods for the KTLibraryWidget.
 * Here is where folder methods are defined for the Library widget.
 * @author David Cuadrado <krawek@toonka.com>
*/

//KTGCTable::KTGCTable(QWidget *parent) : QTreeWidget(parent), m_currentFolder(0)

KTGCTable::KTGCTable(QWidget *parent) : KTreeListWidget(parent), m_currentFolder(0)
{
    setHeaderLabels(QStringList() << "" << "");

    header()->setResizeMode(QHeaderView::ResizeToContents);
    setItemDelegate(new KTTreeDelegate(this));
    setColumnCount(3);

    setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true); 
    setDragDropMode(QAbstractItemView::InternalMove);

    foldersTotal = 1;
}

KTGCTable::~KTGCTable()
{
}

void KTGCTable::createFolder(const QString &name)
{
    if (name.isNull())
        folderName = tr("New folder %1").arg(foldersTotal);
    else
        folderName = name;

    QTreeWidgetItem *newFolder = new QTreeWidgetItem(this);
    newFolder->setFlags(newFolder->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
    newFolder->setIcon(0, QIcon(THEME_DIR + "icons/open.png"));
    newFolder->setText(1, folderName);
    newFolder->setText(2, "");
    newFolder->setFlags(newFolder->flags() | Qt::ItemIsEditable);

    foldersTotal++;

    m_currentFolder = newFolder;
    setCurrentItem(m_currentFolder);

    if (name.isNull())
        editItem(m_currentFolder, 1);
}

QTreeWidgetItem *KTGCTable::getFolder(const QString &folderName)
{
    QList<QTreeWidgetItem *> nodes = findItems(folderName, Qt::MatchExactly, 1);
    for (int i = 0; i < nodes.size(); ++i) {
         QTreeWidgetItem *node = nodes.at(i);
         if ((node->text(1).compare(folderName) == 0) && (node->text(2).length() == 0))
             return node; 
    }

    return 0;
}

QString KTGCTable::oldFolder()
{
    return folderName;
}

QTreeWidgetItem *KTGCTable::currentFolder()
{
    return m_currentFolder;
}

void KTGCTable::setCurrentFolder(QTreeWidgetItem *cf)
{
    if (cf)
        m_currentFolder = cf;
}

void KTGCTable::removeCurrentFolder()
{
    if (m_currentFolder) {
        int index = indexOfTopLevelItem(m_currentFolder) - 1;

        delete m_currentFolder;

        m_currentFolder = topLevelItem (index);
        setCurrentItem(m_currentFolder);
    }
}

void KTGCTable::callRename() 
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit itemRenamed(item);
}

void KTGCTable::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
        callRename();
}

bool KTGCTable::isFolder(QTreeWidgetItem *item) 
{
    if (item->text(2).length() == 0)
        return true;
   
    return false;
}

int  KTGCTable::indexOf(QTreeWidgetItem *item)
{
    return indexOfTopLevelItem(item) + 1;
}

void KTGCTable::mousePressEvent(QMouseEvent *event)
{
    parentNode = "";

    QTreeWidgetItem *item = itemAt(event->pos());

    if (item) {
        setCurrentItem(item);
        emit itemSelected(item);

        if (event->buttons() == Qt::RightButton) {

            QAction *rename = new QAction(tr("Rename"), this);
            connect(rename, SIGNAL(triggered()), this, SLOT(callRename()));

            QAction *remove = new QAction(tr("Delete"), this);
            // remove->setShortcut(QKeySequence(Qt::Key_Delete));
            connect(remove, SIGNAL(triggered()), this, SIGNAL(itemRemoved()));

            QAction *edit;

            if (item->text(2).compare("SVG")==0) {
                edit = new QAction(tr("Edit with Inkscape"), this);
                connect(edit, SIGNAL(triggered()), this, SLOT(callInkscape()));
            } else if (item->text(2).compare("OBJ")!=0) {
                edit = new QAction(tr("Edit with Gimp"), this);
                connect(edit, SIGNAL(triggered()), this, SLOT(callGimp()));
            }

            QMenu *menu = new QMenu(tr("Options"));
            menu->addAction(rename);
            menu->addAction(remove);
            if (item->text(2).compare("OBJ")!=0) {
                edit->setDisabled(true);
                menu->addAction(edit);
            }

            menu->exec(event->globalPos());

        } else if (event->buttons() == Qt::LeftButton) {

            // If the node has a parent, get the parent's name
            QTreeWidgetItem *top = item->parent(); 
            if (top)
                parentNode = top->text(1);

            // For directories, get the children
            nodeChildren.clear();
            if (item->text(2).length()==0 && item->childCount() > 0) {
                for (int i=0;i<item->childCount();i++) {
                     QTreeWidgetItem *node = item->child(i);
                     nodeChildren << node;
                }
            } 

            QPixmap pixmap = item->icon(0).pixmap(15, 15);

            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << pixmap << item->text(1) << item->text(2) << item->text(3);

            QMimeData *mimeData = new QMimeData;
            mimeData->setData("application/x-dnditemdata", itemData);

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setPixmap(pixmap);

            if (drag->start(Qt::MoveAction) == Qt::MoveAction)
                delete takeTopLevelItem(indexOfTopLevelItem(item));
        }
    }
}

void KTGCTable::dropEvent(QDropEvent *event)
{
     bool eventAccept = false;

     if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
         QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
         QDataStream dataStream(&itemData, QIODevice::ReadOnly);

         QPixmap pixmap;
         QString label;
         QString extension;
         QString key;
         dataStream >> pixmap >> label >> extension >> key;

         QTreeWidgetItem *item;
         QTreeWidgetItem *parent = itemAt(event->pos().x(), event->pos().y());

         if (parent) {
             // If both Target and Origin are directories
             if (parentNode.length() > 0) {
                 if ((parent->text(2).length() == 0) && (extension.length() == 0))
                      return;
             }

             // Ensure Target is a directory
             if (parent->text(2).length() > 0)
                 return;

             // Target is going to the same parent
             int childrenTotal = parent->childCount(); 
             for (int i=0;i < childrenTotal; i++) {
                  QTreeWidgetItem *child = parent->child(i);
                  QString name = child->text(1);
                  if (name.compare(label) == 0 && child->text(2).compare(extension) == 0)
                      return;
             }

             // Make sure node and target are not the same
             if (extension.length() > 0) {
                 if (parentNode.length() > 0) {
                     QList<QTreeWidgetItem *> nodes = findItems(parentNode, Qt::MatchExactly, 1);
                     for (int i = 0; i < nodes.size(); ++i) {
                          QTreeWidgetItem *node = nodes.at(i);
                          if (node->text(1).compare(parentNode) == 0) {
                              int childrenTotal = node->childCount();
                              for (int i=0;i < childrenTotal; i++) {
                                   QTreeWidgetItem *child = node->child(i);
                                   if (child->text(1).compare(label) == 0 && child->text(2).compare(extension) == 0) {
                                       node->removeChild(child);
                                       break;
                                   }
                              }
                              break;
                          }
                     }
                 }

                 item = new QTreeWidgetItem(parent);
                 item->setIcon(0, QIcon(pixmap));
                 item->setText(1, label);
                 item->setText(2, extension);
                 item->setText(3, key);
                 item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
                 setCurrentItem(item);

                 if (key.length() > 0)
                     emit itemMoved(key, parent->text(1));

                 eventAccept = true;

             } else {
                 bool flag = true;
                 if (parent->isExpanded())
                     flag = false;
                 parent->setExpanded(flag);
             }

         } else {

             if (parentNode.length() > 0) {
                 QList<QTreeWidgetItem *> nodes = findItems(parentNode, Qt::MatchExactly, 1);
                 for (int i = 0; i < nodes.size(); ++i) {
                      QTreeWidgetItem *node = nodes.at(i);
                      if (node->text(1).compare(parentNode) == 0) {
                          int childrenTotal = node->childCount();
                          for (int i=0;i < childrenTotal; i++) {
                               QTreeWidgetItem *child = node->child(i);
                               if (child->text(1).compare(label) == 0 && child->text(2).compare(extension) == 0) {
                                   node->removeChild(child); 
                                   break;
                               }
                          }
                          break;
                      }
                 }
             } else {
                 if (extension.length() == 0 && nodeChildren.size() > 0)
                     return;
             }

             item = new QTreeWidgetItem(this);
             item->setIcon(0, QIcon(pixmap));
             item->setText(1, label);
             item->setText(2, extension);
             item->setText(3, key);
             item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
             setCurrentItem(item);

             emit itemMoved(key, "");
             eventAccept = true;
         }

     } else {
         event->ignore();
         return;
     }

     if (eventAccept) {
         if (event->source() == this) {
             event->setDropAction(Qt::MoveAction);
             event->accept();
         } else {
             event->acceptProposedAction();
         }
     }
}

void KTGCTable::dragEnterEvent(QDragEnterEvent *event)
{
     if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
         if (event->source() == this) {
             event->setDropAction(Qt::MoveAction);
             event->accept();
         } else {
             event->acceptProposedAction();
         }
     } else {
         event->ignore();
     }
}

void KTGCTable::dragMoveEvent(QDragMoveEvent *event)
{
     if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
         if (event->source() == this) {
             event->setDropAction(Qt::MoveAction);
             event->accept();
         } else {
             event->acceptProposedAction();
         }
     } else {
         event->ignore();
     }
}

/*
void KTGCTable::dragLeaveEvent(QDragLeaveEvent *event)
{
    kFatal() <<  "KTGCTable::dragLeaveEvent - Just tracing...";
}
*/

void KTGCTable::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Up) {
        QTreeWidgetItem *current = currentItem();
        if (current) {
            QTreeWidgetItem *up = itemAbove(current);
            if (up) {
                setCurrentItem(up);
                emit itemSelected(up);
            }
        }
        return;
    }

    if (event->key() == Qt::Key_Down) {
        QTreeWidgetItem *current = currentItem();
        if (current) {
            QTreeWidgetItem *down = itemBelow(current);
            if (down) {
                setCurrentItem(down);
                emit itemSelected(down);
            }
        }
        return;
    }

    if (event->key() == Qt::Key_Return) {
        QTreeWidgetItem *current = currentItem();
        if (current) {
            if (current->text(2).length() == 0) {
                if (current->childCount() > 0) {
                    if (current->isExpanded())
                        current->setExpanded(false);
                    else
                        current->setExpanded(true);
                }
            }
        }
        return;
    }
}

void KTGCTable::callInkscape()
{
    // execute /usr/bin/inkscape
}

void KTGCTable::callGimp()
{
    // execute /usr/bin/gimp   
}
