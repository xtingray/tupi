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

#include "texttool.h"

#include <QPointF>
#include <QFontMetrics>
#include <QKeySequence>
#include <QGraphicsView>

#include "kglobal.h"
#include "ktscene.h"

#include "ktinputdeviceinformation.h"
#include "ktgraphicsscene.h"
#include "ktprojectrequest.h"
#include "ktprojectresponse.h"
#include "ktlibraryobject.h"

#include "ktrequestbuilder.h"

TextTool::TextTool()
{
    m_configurator = new TextConfigurator;
    setupActions();
}

TextTool::~TextTool()
{
}

QStringList TextTool::keys() const
{
    return QStringList() << tr("Text");
}

void TextTool::press(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(brushManager);

    QList<QGraphicsItem *> items = scene->items(input->pos());

    if (items.count() > 0) {
        QGraphicsItem *itemPress = items[0];
        if  (itemPressed(itemPress))
             return;
    }

    m_item = new KTTextItem;
    m_item->setPos(input->pos());
    m_item->setDefaultTextColor(brushManager->penColor());
}

void TextTool::doubleClick(const KTInputDeviceInformation *input, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(scene);
}

bool TextTool::itemPressed(QGraphicsItem *item)
{
    if (KTTextItem *text = qgraphicsitem_cast<KTTextItem *>(item)) {
        text->setEditable(true);
        text->setFocus();
        return true;
    }

    return false;
}

void TextTool::move(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(scene);
    Q_UNUSED(scene);
    Q_UNUSED(brushManager);
}

void TextTool::release(const KTInputDeviceInformation *input, KTBrushManager *brushManager, KTGraphicsScene *scene)
{
    Q_UNUSED(input);
    Q_UNUSED(brushManager);

    if (m_configurator->text().isEmpty()) {
        delete m_item;
        return;
    }

    if (m_configurator->isHtml())
        m_item->setHtml(m_configurator->text());
    else
        m_item->setPlainText(m_configurator->text());

    m_item->setFont(m_configurator->textFont());

    scene->includeObject(m_item);

    QDomDocument doc;
    doc.appendChild(m_item->toXml(doc));

    KTProjectRequest event = KTRequestBuilder::createItemRequest(scene->currentSceneIndex(), scene->currentLayerIndex(), 
                             scene->currentFrameIndex(), scene->currentFrame()->graphics().count(), QPointF(),
                             KTLibraryObject::Item, KTProjectRequest::Add, doc.toString()); // Adds to end

    emit requested(&event);
}

QMap<QString, KAction *> TextTool::actions() const
{
    return m_actions;
}

int TextTool::toolType() const
{
    return Brush;
}
		
QWidget *TextTool::configurator()
{
    return m_configurator;
}

void TextTool::aboutToChangeTool()
{
}

void TextTool::aboutToChangeScene(KTGraphicsScene *scene)
{
    Q_UNUSED(scene);
}

void TextTool::setupActions()
{
    KAction *text = new KAction(QIcon(THEME_DIR + "icons/text.png"), tr("Text"), this);
    text->setShortcut(QKeySequence(tr("T")));
    text->setCursor(QCursor(THEME_DIR + "cursors/text.png"));

    m_actions.insert(tr("Text"), text);
}

void TextTool::saveConfig()
{
}

Q_EXPORT_PLUGIN2(kt_textool, TextTool);
