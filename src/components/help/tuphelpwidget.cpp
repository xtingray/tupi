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

#include "tuphelpwidget.h"
#include <qlayout.h>
#include <qtabwidget.h>
#include <qdom.h>
#include <qfile.h>
#include <qmap.h>

#include <QLocale>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

// Tupi Framework 
#include "tdebug.h"
#include "tglobal.h"

TupHelpWidget::TupHelpWidget(const QString &path, QWidget *parent) : TupModuleWidgetBase(parent)
{
    setWindowTitle(tr("Help"));
    setWindowIcon(QPixmap(THEME_DIR + "icons/help.png"));

    QString lang = QString(QLocale::system().name()).left(2);

    if (lang.length() > 0) {
        m_helpPath = new QDir(path + lang);
        if (!m_helpPath->exists())
            m_helpPath = new QDir(path + "en");
    } else {
        m_helpPath = new QDir(path + "en");
    }

    QTreeWidget *contentsListView = new QTreeWidget(this);
    contentsListView->setHeaderLabels(QStringList() << tr(""));
    contentsListView->header()->hide();

    //connect(contentsListView, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, 
    //                          SLOT(tryToLoadPage(QTreeWidgetItem *, int)));

    addChild(contentsListView);

    QDomDocument document;
    QFile file(m_helpPath->path() + "/help.xml");

    QTreeWidgetItem *first = new QTreeWidgetItem;

    if (file.open(QIODevice::ReadOnly)) {
        if (document.setContent(&file)) {
            QDomElement root = document.documentElement();
            QDomNode section = root.firstChild();

            while (!section.isNull()) {

                   QDomElement element = section.toElement();

                   if (!element.isNull()) {
                       if (element.tagName() == "Section") {

                           QTreeWidgetItem *item = new QTreeWidgetItem(contentsListView);
                           item->setText(0, element.attribute("title"));
                           m_files.insert(item, element.attribute("file"));

                           if (element.attribute("file").compare("cover.html") == 0)
                               first = item;

                           QDomNode subSection = element.firstChild();
                           while (! subSection.isNull()) {

                                  QDomElement element2 = subSection.toElement();
                                  if (!element2.isNull()) {
                                      if (element2.tagName() == "SubSection") {
                                          QTreeWidgetItem *subitem = new QTreeWidgetItem(item);
                                          subitem->setText(0, element2.attribute("title"));
                                          m_files.insert(subitem, element2.attribute("file"));
                                      }
                                  }
                                  subSection = subSection.nextSibling();
                           }
                       }
                   }
                   section = section.nextSibling();
            }
        } else {
                 qDebug("TupHelpWidget::Can't set contents");
        }
        file.close();
    } else {
        qDebug("TupHelpWidget::Can't open");
    }

    contentsListView->show();
    contentsListView->expandAll();
    if (first)
        contentsListView->setCurrentItem(first);

    connect(contentsListView, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this,
                              SLOT(tryToLoadPage(QTreeWidgetItem *, QTreeWidgetItem *)));
}

TupHelpWidget::~TupHelpWidget()
{
}

void TupHelpWidget::tryToLoadPage(QTreeWidgetItem *item, QTreeWidgetItem *preview)
{
    Q_UNUSED(preview);

    if (item) {
        QString fileName = m_files[item];
        if (! fileName.isNull())
            loadPage(m_helpPath->path()+"/"+ fileName);
    }
}

void TupHelpWidget::loadPage(const QString &filePath)
{
    emit pageLoaded(filePath);
}

QString TupHelpWidget::helpPath () const
{
    return m_helpPath->path();
}
