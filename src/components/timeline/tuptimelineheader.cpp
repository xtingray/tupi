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

#include "tuptimelineheader.h"

struct TupTimeLineHeader::Private
{
    QPixmap lockIcon;
    QPixmap viewIconOn;
    QPixmap viewIconOff;
    int currentLayer;
    QList<TimeLineLayerItem> layers;
    QLineEdit *editor;
    int editorSection;
    bool sectionOnMotion;
};

TupTimeLineHeader::TupTimeLineHeader(QWidget * parent) : QHeaderView(Qt::Vertical, parent), k(new Private)
{
    setSectionsClickable(true);
    setSectionsMovable(true);

    setFixedWidth(115);
    // SQA: This code will be disabled until the "Lock layer" feature is implemented
    // setFixedWidth(140);
    // k->lockIcon = QPixmap(THEME_DIR + "icons/padlock.png");
    k->viewIconOn = QPixmap(THEME_DIR + "icons/show_layer.png");
    k->viewIconOff = QPixmap(THEME_DIR + "icons/hide_layer.png");

    connect(this, SIGNAL(sectionDoubleClicked(int)), this, SLOT(showTitleEditor(int)));

    k->editorSection = -1;

    k->editor = new QLineEdit(this);
    k->editor->setFocusPolicy(Qt::ClickFocus);
    k->editor->setInputMask("");
    connect(k->editor, SIGNAL(editingFinished()), this, SLOT(hideTitleEditor()));
    k->editor->hide();
}

TupTimeLineHeader::~TupTimeLineHeader()
{
}

void TupTimeLineHeader::paintSection(QPainter * painter, const QRect & rect, int index) const
{
    if (!model() || !rect.isValid())
        return;

    painter->save();

    QStyleOptionHeader headerOption;
    headerOption.rect = rect;
    headerOption.orientation = Qt::Vertical;
    headerOption.position = QStyleOptionHeader::Middle;
    headerOption.text = "";

    style()->drawControl(QStyle::CE_HeaderSection, &headerOption, painter);

    if (k->currentLayer == index) {
        QColor color(0, 136, 0, 40);
        painter->fillRect(rect, color);
    }

    QFont font = this->font();
    font.setPointSize(7);
    // QFont font("Arial", 7, QFont::Normal, false);
    QFontMetrics fm(font);

    int y = rect.normalized().bottomLeft().y() - (1 + (rect.normalized().height() - fm.height())/2);
    painter->setFont(font);
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter->drawText(10, y, k->layers[index].title);

    y = rect.y();

    // SQA: This code will be disabled until the "Lock layer" feature is implemented
    /* 
    QRectF lockRect = QRectF(0, 0, 11, 12);
    int lockY = (rect.height() - lockRect.height())/2;
    painter->drawPixmap(QPointF(rect.x() + 90, lockY + y), k->lockIcon, lockRect);
    */

    QRectF viewRect = QRectF(0, 0, 13, 7); 
    int viewY = (rect.height() - viewRect.height())/2;
    if (k->layers[index].isVisible)
        painter->drawPixmap(QPointF(rect.x() + 90, viewY + y), k->viewIconOn, viewRect);
    else
        painter->drawPixmap(QPointF(rect.x() + 90, viewY + y), k->viewIconOff, viewRect);

    painter->restore();
}

void TupTimeLineHeader::mousePressEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    int section = logicalIndexAt(point);

    if (section != k->currentLayer)
        emit headerSelectionChanged(section);

    int y = sectionViewportPosition(section);
    QRect rect(90, y, 20, sectionSize(section));
    if (rect.contains(point))
        emit visibilityChanged(section, !k->layers[section].isVisible);

    QHeaderView::mousePressEvent(event);
}

void TupTimeLineHeader::updateSelection(int index)
{
    if (k->currentLayer != index) {
        k->currentLayer = index;
        updateSection(index);
    }
}

void TupTimeLineHeader::insertSection(int index, const QString &name)
{
    TimeLineLayerItem layer;
    layer.title = name;
    layer.lastFrame = -1;
    layer.isVisible = true;
    layer.isLocked = false;
    layer.isSound = false;

    k->layers.insert(index, layer);
}

void TupTimeLineHeader::setSectionVisibility(int index, bool visibility)
{
    if (index >= 0 && index < k->layers.count()) {
        k->layers[index].isVisible = visibility;
        updateSection(index);
    }
}

void TupTimeLineHeader::setSectionTitle(int index, const QString &name)
{
    k->layers[index].title = name;
    updateSection(index);
}

void TupTimeLineHeader::showTitleEditor(int index)
{
    if (index >= 0) {
        QFont font = this->font();
        font.setPointSize(7);
        // QFont font("Arial", 7, QFont::Normal, false);
        k->editor->setFont(font);
        int x = sectionViewportPosition(index);
        k->editor->setGeometry(0, x, width(), sectionSize(index));
        k->editorSection = index;
        k->editor->setText(k->layers[index].title);
        k->editor->show();
        k->editor->setFocus();
    }
}

void TupTimeLineHeader::hideTitleEditor()
{
    k->editor->hide();

    if (k->editorSection != -1 && k->editor->isModified())
        emit nameChanged(k->editorSection, k->editor->text());

    k->editorSection = -1;
}

void TupTimeLineHeader::removeSection(int index)
{
    k->layers.removeAt(index);
}

int TupTimeLineHeader::lastFrame(int index)
{
    if (index > -1 && index < k->layers.count())
        return k->layers[index].lastFrame;
    return -1;
}

void TupTimeLineHeader::updateLastFrame(int index, bool addition)
{
    if (addition)
        k->layers[index].lastFrame++;
    else
        k->layers[index].lastFrame--;
}

void TupTimeLineHeader::resetLastFrame(int index)
{
    k->layers[index].lastFrame = -1;
}

bool TupTimeLineHeader::isSound(int index)
{
    return k->layers[index].isSound;
}

void TupTimeLineHeader::setSoundFlag(int index, bool flag)
{
    k->layers[index].isSound = flag;
}

int TupTimeLineHeader::currentSectionIndex()
{
    return k->currentLayer;
}

void TupTimeLineHeader::moveHeaderSection(int position, int newPosition, bool isLocalRequest)
{
    if (isLocalRequest) {
        k->sectionOnMotion = true;
        moveSection(visualIndex(position), visualIndex(newPosition));
        k->layers.swap(position, newPosition);
        k->sectionOnMotion = false;
    } else {
        k->layers.swap(position, newPosition);
    }
}

bool TupTimeLineHeader::sectionIsMoving()
{
    return k->sectionOnMotion;
}


