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

#include "tupitemmanager.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tapplication.h"

#include <QHeaderView>
#include <QMenu>

/**
 * This class provides some methods for the TupLibraryWidget.
 * Here is where folder methods are defined for the Library widget.
 * @author David Cuadrado
*/

TupItemManager::TupItemManager(QWidget *parent) : TreeListWidget(parent), m_currentFolder(0)
{
    currentSelection = "";
    setHeaderLabels(QStringList() << "" << "");

    header()->setResizeMode(QHeaderView::ResizeToContents);
    setItemDelegate(new TupTreeDelegate(this));
    setColumnCount(3);

    setAcceptDrops(true);
    setDragEnabled(true);
    setDropIndicatorShown(true); 
    setDragDropMode(QAbstractItemView::InternalMove);

    foldersTotal = 1;
}

TupItemManager::~TupItemManager()
{
}

void TupItemManager::createFolder(const QString &name)
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

    // SQA: Check if this instruction is really necessary
    newFolder->setFlags(newFolder->flags() | Qt::ItemIsEditable);

    foldersTotal++;

    m_currentFolder = newFolder;
    setCurrentItem(m_currentFolder);

    if (name.isNull()) {
        editItem(m_currentFolder, 1);
        emit itemCreated(newFolder);
    } 
}

QTreeWidgetItem *TupItemManager::getFolder(const QString &folderName)
{
    QList<QTreeWidgetItem *> nodes = findItems(folderName, Qt::MatchExactly, 1);
    for (int i = 0; i < nodes.size(); ++i) {
         QTreeWidgetItem *node = nodes.at(i);
         if ((node->text(1).compare(folderName) == 0) && (node->text(2).length() == 0))
             return node; 
    }

    return 0;
}

QString TupItemManager::oldFolder()
{
    return folderName;
}

QTreeWidgetItem *TupItemManager::currentFolder()
{
    return m_currentFolder;
}

void TupItemManager::setCurrentFolder(QTreeWidgetItem *cf)
{
    if (cf)
        m_currentFolder = cf;
}

void TupItemManager::removeCurrentFolder()
{
    if (m_currentFolder) {
        int index = indexOfTopLevelItem(m_currentFolder) - 1;

        delete m_currentFolder;

        m_currentFolder = topLevelItem (index);
        setCurrentItem(m_currentFolder);
    }
}

void TupItemManager::renameItem() 
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit itemRenamed(item);
}

void TupItemManager::cloneItem()
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit itemCloned(item);
}

void TupItemManager::callInkscapeToEdit()
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit inkscapeEditCall(item);
}

void TupItemManager::callGimpToEdit()
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit gimpEditCall(item);
}

void TupItemManager::callKritaToEdit()
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit kritaEditCall(item);
}

void TupItemManager::callMyPaintToEdit()
{
    QTreeWidgetItem *item = currentItem();
    if (item)
        emit myPaintEditCall(item);
}

void TupItemManager::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
        renameItem();
}

bool TupItemManager::isFolder(QTreeWidgetItem *item) 
{
    if (item->text(2).length() == 0)
        return true;
   
    return false;
}

int  TupItemManager::indexOf(QTreeWidgetItem *item)
{
    return indexOfTopLevelItem(item) + 1;
}

void TupItemManager::mousePressEvent(QMouseEvent *event)
{
    parentNode = "";

    QTreeWidgetItem *item = itemAt(event->pos());

    if (item) {
        setCurrentItem(item);
        emit itemSelected(item);

        if (event->buttons() == Qt::RightButton) {

            QMenu *menu = new QMenu(tr("Options"));

            if (item->text(2).compare("SVG")==0) {
                QAction *edit = new QAction(tr("Edit with Inkscape"), this);
                connect(edit, SIGNAL(triggered()), this, SLOT(callInkscapeToEdit()));
#ifdef Q_OS_UNIX
                if (!QFile::exists("/usr/bin/inkscape"))
                    edit->setDisabled(true);
#else
                edit->setDisabled(true);
#endif
                menu->addAction(edit);
            } else if (item->text(2).compare("OBJ")!=0) {
                QAction *gimpEdit = new QAction(tr("Edit with Gimp"), this);
                connect(gimpEdit, SIGNAL(triggered()), this, SLOT(callGimpToEdit()));
#ifdef Q_OS_UNIX
                if (!QFile::exists("/usr/bin/gimp"))
                    gimpEdit->setDisabled(true);
#else
                gimpEdit->setDisabled(true);
#endif
                menu->addAction(gimpEdit);

                QAction *kritaEdit = new QAction(tr("Edit with Krita"), this);
                connect(kritaEdit, SIGNAL(triggered()), this, SLOT(callKritaToEdit()));
#ifdef Q_OS_UNIX
                if (!QFile::exists("/usr/bin/krita"))
                    kritaEdit->setDisabled(true);
#else
                kritaEdit->setDisabled(true);
#endif
                menu->addAction(kritaEdit);

                QAction *myPaintEdit = new QAction(tr("Edit with MyPaint"), this);
                connect(myPaintEdit, SIGNAL(triggered()), this, SLOT(callMyPaintToEdit()));
#ifdef Q_OS_UNIX
                if (!QFile::exists("/usr/bin/mypaint"))
                    myPaintEdit->setDisabled(true);
#else
                myPaintEdit->setDisabled(true);
#endif
                menu->addAction(myPaintEdit);
            }

            QAction *rename = new QAction(tr("Rename"), this);
            connect(rename, SIGNAL(triggered()), this, SLOT(renameItem()));

            QAction *clone = new QAction(tr("Clone"), this);
            connect(clone, SIGNAL(triggered()), this, SLOT(cloneItem()));

            QAction *remove = new QAction(tr("Delete"), this);
            connect(remove, SIGNAL(triggered()), this, SIGNAL(itemRemoved()));

            menu->addAction(clone);
            menu->addAction(rename);
            menu->addAction(remove);

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

void TupItemManager::dropEvent(QDropEvent *event)
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

void TupItemManager::dragEnterEvent(QDragEnterEvent *event)
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

void TupItemManager::dragMoveEvent(QDragMoveEvent *event)
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

void TupItemManager::keyPressEvent(QKeyEvent * event)
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

void TupItemManager::cleanUI()
{
    clear();
    foldersTotal = 1;
}
