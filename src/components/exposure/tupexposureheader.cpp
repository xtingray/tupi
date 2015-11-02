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

#include "tupexposureheader.h"

/**
 * @author Jorge Cuadrado
 */

TupExposureHeader::TupExposureHeader(QWidget * parent) : QHeaderView(Qt::Horizontal, parent), m_editedSection(-1), m_sectionOnMotion(false)
{
    setSectionsClickable(true);
    setSectionsMovable(true);

    connect(this, SIGNAL(sectionDoubleClicked(int)), this, SLOT(showTitleEditor(int)));

    m_editor = new QLineEdit(this);
    m_editor->setFocusPolicy(Qt::ClickFocus);
    m_editor->setInputMask("");
    connect(m_editor, SIGNAL(editingFinished()), this, SLOT(hideTitleEditor()));
    m_editor->hide();
}

TupExposureHeader::~TupExposureHeader()
{
}

void TupExposureHeader::notifyVisibilityChange(int section)
{
   emit visibilityChanged(visualIndex(section), !m_sections[section].isVisible);
}

void TupExposureHeader::setSectionVisibility(int section, bool visibility)
{
    // Q_UNUSED(visibility);
    // FIXME: in tupexpousertable.cpp visibility or !m_sections[logicalndex].isVisible
    // m_sections[logicalndex].isVisible = !m_sections[logicalndex].isVisible;

    m_sections[section].isVisible = visibility;
    updateSection(section);
}

void TupExposureHeader::showTitleEditor(int section)
{
    if (section >= 0) {
        QFont font = this->font();
        font.setPointSize(8);
        m_editor->setFont(font);
        // QFont font("Arial", 8, QFont::Normal, false);

        int x = sectionViewportPosition(section);
        m_editor->setGeometry(x, 0, sectionSize(section), height());
        m_editedSection = section;
        m_editor->setText(m_sections[section].title);
        m_editor->show();
        m_editor->setFocus();
    }
}

void TupExposureHeader::hideTitleEditor()
{
    m_editor->hide();

    if (m_editedSection != -1 && m_editor->isModified())
        emit nameChanged(m_editedSection, m_editor->text());

    m_editedSection = -1;
}

void TupExposureHeader::insertSection(int section, const QString &text)
{
    ExposureLayerItem layer;
    layer.title = text;
    layer.lastFrame = 0;
    layer.isVisible = true;
    layer.isLocked = false;
    m_sections.insert(section, layer);
}

void TupExposureHeader::setSectionTitle(int section, const QString &text)
{
    m_sections[section].title = text;
    updateSection(section);
}

bool TupExposureHeader::sectionIsMoving()
{
    return m_sectionOnMotion;
}

void TupExposureHeader::setLockFlag(int logicalndex, bool lock)
{
    m_sections[logicalndex].isLocked = lock;
    updateSection(logicalndex);
}

void TupExposureHeader::moveHeaderSection(int position, int newPosition, bool isLocalRequest)
{
    if (isLocalRequest) {
        m_sectionOnMotion = true;
        moveSection(visualIndex(position), visualIndex(newPosition));
        m_sections.swap(position, newPosition);
        m_sectionOnMotion = false;
    } else {
        m_sections.swap(position, newPosition);
    }
}

int TupExposureHeader::lastFrame(int section)
{
    return m_sections[section].lastFrame;
}

void TupExposureHeader::removeSection(int section)
{
    m_sections.removeAt(section);
}

void TupExposureHeader::setLastFrame(int section, int num)
{
    m_sections[section].lastFrame = num;
}

void TupExposureHeader::mousePressEvent(QMouseEvent *event)
{
    int section = logicalIndexAt(event->pos());
    if (section > -1 && section < count()) {
        int x = sectionViewportPosition(section) + 3;
        QFont font = this->font();
        font.setPointSize(8);
        QFontMetrics fm(font);
        QString text = m_sections[section].title;
        int w = fm.width(text);
        int limit = sectionSize(section)/2 - w/2;

        QRect rect(x + limit - 12, 3, 12, height()-3);
        if (rect.contains(event->pos())) {
            notifyVisibilityChange(section);
        } else {
            if (m_currentSection != section)
                emit headerSelectionChanged(section);

            QHeaderView::mousePressEvent(event);
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureHeader::mousePressEvent() - Fatal Error: Section index is invalid -> " + QString::number(section);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
        #endif
    }
}

void TupExposureHeader::paintSection(QPainter *painter, const QRect & rect, int section) const
{
    if (!rect.isValid()) 
        return;

    QStyleOptionHeader headerOption;
    headerOption.rect = rect;
    headerOption.orientation = Qt::Horizontal;
    headerOption.position = QStyleOptionHeader::Middle;
    headerOption.text = "";

    QStyle::State state = QStyle::State_None;

    if (isEnabled())
        state |= QStyle::State_Enabled;

    if (window()->isActiveWindow())
        state |= QStyle::State_Active;

    style()->drawControl(QStyle::CE_HeaderSection, &headerOption, painter);

    QString text = m_sections[section].title;
    QFont font = this->font();
    font.setPointSize(8);
    // QFont font("Arial", 8, QFont::Normal, false);
    QFontMetrics fm(font);

    if (((section == m_currentSection) || (m_sections.size() == 1)) && m_sections[section].isVisible) { // Header selected
        QColor color(0, 136, 0, 40);
        painter->fillRect(rect.normalized().adjusted(0, 0, 0, -1), color);
    }

    QStyleOptionButton buttonOption;

    if (m_sections[section].isVisible) {
        buttonOption.palette.setBrush(QPalette::Button, QColor(0, 136, 0, 255));
    } else {
        buttonOption.palette.setBrush(QPalette::Button, Qt::red);
        buttonOption.state |= QStyle::State_Sunken;
        QColor color(255, 0, 0, 40);
        painter->fillRect(rect.normalized().adjusted(0, 0, 0, -1), color);
    }

    int buttonWidth = 12;
    int width = (rect.normalized().width() - (fm.width(text) + buttonWidth) + 4)/ 2;
    int x = rect.normalized().x() + width + buttonWidth;
    int y = rect.normalized().bottomLeft().y() - (1 + (rect.normalized().height() - fm.height())/2);

    painter->setFont(font);
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter->drawText(x, y, text);

    buttonOption.rect = QRect(rect.x() + width - 4, rect.y() + ((rect.normalized().height()-buttonWidth)/2) + 1, buttonWidth, buttonWidth);

    style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
}

void TupExposureHeader::updateSelection(int section)
{
    m_currentSection = section;
    updateSection(section);
}

int TupExposureHeader::sectionsTotal()
{
    return m_sections.size();
}

int TupExposureHeader::currentSectionIndex()
{
    return m_currentSection;
}

//#include "tupexposuretable.moc"
