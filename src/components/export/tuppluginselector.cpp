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

#include "tuppluginselector.h"

TupPluginSelector::TupPluginSelector() : TupExportWizardPage(tr("Select Plugin"))
{
    setTag("PLUGIN");
    QWidget *container = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(container);

    m_exporterList = new QListWidget;
    m_exporterList->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(m_exporterList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(selectedPluginItem(QListWidgetItem *)));
    layout->addWidget(m_exporterList);

    m_formatList = new QListWidget;
    connect(m_formatList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(selectedFormatItem(QListWidgetItem *)));
    layout->addWidget(m_formatList);

    setWidget(container);
    reset();
}

TupPluginSelector::~TupPluginSelector()
{
}

bool TupPluginSelector::isComplete() const
{
    return m_exporterList->selectedItems().count() > 0 && m_formatList->selectedItems().count() > 0;
}

void TupPluginSelector::reset()
{
    m_exporterList->clearSelection();
    m_formatList->clearSelection();
    m_formatList->clear();
}

void TupPluginSelector::addPlugin(const QString &plugin)
{
    #ifdef Q_OS_WIN
       if (QSysInfo::windowsVersion() != QSysInfo::WV_XP) {
           new QListWidgetItem(plugin, m_exporterList);
       } else {
       if (plugin.compare(tr("Video Formats")) != 0)
       new QListWidgetItem(plugin, m_exporterList);
   }
    #else
       new QListWidgetItem(plugin, m_exporterList);
    #endif
}

void TupPluginSelector::selectedPluginItem(QListWidgetItem *item)
{
    if (item) {
        emit selectedPlugin(item->text());
        emit completed();
    }
}

void TupPluginSelector::selectFirstItem()
{
    if (m_exporterList->count() > 0) {
        m_exporterList->item(0)->setSelected(true);
        if (m_exporterList->item(0)) {
            emit selectedPlugin(m_exporterList->item(0)->text());
            emit completed();
        }
    }
}

void TupPluginSelector::setFormats(TupExportInterface::Formats formats)
{
    m_formatList->clear();

#ifdef Q_OS_LINUX
    if (formats & TupExportInterface::OGV) {
        QListWidgetItem *format = new QListWidgetItem(tr("OGV Video"), m_formatList);
        format->setData(3124, TupExportInterface::OGV);
    }
#endif

#if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
    if (formats & TupExportInterface::MP4) {
        QListWidgetItem *format = new QListWidgetItem(tr("MP4 Video"), m_formatList);
        format->setData(3124, TupExportInterface::MP4);
    }

    if (formats & TupExportInterface::GIF) {
        QListWidgetItem *format = new QListWidgetItem(tr("Animated GIF"), m_formatList);
        format->setData(3124, TupExportInterface::GIF);
    }

    if (formats & TupExportInterface::MPEG) {
        QListWidgetItem *format = new QListWidgetItem(tr("MPEG Video"), m_formatList);
        format->setData(3124, TupExportInterface::MPEG);
    }

    if (formats & TupExportInterface::AVI) {
        QListWidgetItem *format = new QListWidgetItem(tr("AVI Video"), m_formatList);
        format->setData(3124, TupExportInterface::AVI);
    }

    if (formats & TupExportInterface::WEBM) {
        QListWidgetItem *format = new QListWidgetItem(tr("WEBM Video"), m_formatList);
        format->setData(3124, TupExportInterface::WEBM);
    }

    if (formats & TupExportInterface::SWF) {
        QListWidgetItem *format = new QListWidgetItem(tr("Macromedia Flash"), m_formatList);
        format->setData(3124, TupExportInterface::SWF);
    }

    if (formats & TupExportInterface::ASF) {
        QListWidgetItem *format = new QListWidgetItem(tr("ASF Video"), m_formatList);
        format->setData(3124, TupExportInterface::ASF);
    }
#endif

    if (formats & TupExportInterface::MOV) {
        QListWidgetItem *format = new QListWidgetItem(tr("QuickTime Video"), m_formatList);
        format->setData(3124, TupExportInterface::MOV);
    }

    if (formats & TupExportInterface::PNG) {
        QListWidgetItem *format = new QListWidgetItem(tr("PNG Image Sequence"), m_formatList);
        format->setData(3124, TupExportInterface::PNG);
    }

    if (formats & TupExportInterface::JPEG) {
        QListWidgetItem *format = new QListWidgetItem(tr("JPEG Image Sequence"), m_formatList);
        format->setData(3124, TupExportInterface::JPEG);
    }

    if (formats & TupExportInterface::SVG) {
        QListWidgetItem *format = new QListWidgetItem(tr("SVG Image Sequence"), m_formatList);
        format->setData(3124, TupExportInterface::SVG);
    }

    if (formats & TupExportInterface::APNG) {
        QListWidgetItem *format = new QListWidgetItem(tr("Animated PNG (APNG)"), m_formatList);
        format->setData(3124, TupExportInterface::APNG);
        format->setFlags(Qt::NoItemFlags);
    }

    if (formats & TupExportInterface::SMIL) {
        QListWidgetItem *format = new QListWidgetItem(tr("SMIL"), m_formatList);
        format->setData(3124, TupExportInterface::SMIL);
    }
}

char const* TupPluginSelector::getFormatExtension(const QString format) 
{ 
    if (format.compare(tr("WEBM Video")) == 0)
        return ".webm";

#ifdef Q_OS_UNIX
    if (format.compare(tr("OGV Video")) == 0)
        return ".ogv";
#endif

    if (format.compare(tr("MPEG Video")) == 0)
        return ".mpg";

    if (format.compare(tr("Macromedia Flash")) == 0)
        return ".swf";

    if (format.compare(tr("MP4 Video")) == 0)
        return ".mp4";

    if (format.compare(tr("AVI Video")) == 0)
        return ".avi";

    if (format.compare(tr("RealMedia Video")) == 0) 
        return ".rm";

    if (format.compare(tr("ASF Video")) == 0)
        return ".asf";

    if (format.compare(tr("QuickTime Video")) == 0)
        return ".mov";

    if (format.compare(tr("Animated GIF")) == 0)
        return ".gif";

    if (format.compare(tr("PNG Image Sequence")) == 0)
        return ".png";

    if (format.compare(tr("JPEG Image Sequence")) == 0)
        return ".jpeg";

    if (format.compare(tr("Animated PNG (APNG)")) == 0)
        return ".png";

    if (format.compare(tr("SVG Image Sequence")) == 0)
        return ".svg";

    if (format.compare(tr("SMIL")) == 0)
        return ".smil";

    return ".none";
}

void TupPluginSelector::selectedFormatItem(QListWidgetItem *item)
{
    if (item) {
        extension = getFormatExtension(item->text());
        QList<QListWidgetItem *> family = m_exporterList->selectedItems();
        QListWidgetItem *familyItem = (QListWidgetItem *) family.at(0); 

        QString familyLabel = familyItem->text(); 
        if (familyLabel.compare(tr("Animated Image")) == 0) {
            emit animatedImageFormatSelected(item->data(3124).toInt(), extension);
        } else if (familyLabel.compare(tr("Image Sequence")) == 0) {
                   emit imagesArrayFormatSelected(item->data(3124).toInt(), extension);
        } else { 
            emit animationFormatSelected(item->data(3124).toInt(), extension);
        }

        // emit formatSelected(item->data(3124).toInt(), extension);

        emit completed();
    }
}

const char* TupPluginSelector::getFileExtension()
{
    return extension;
}

