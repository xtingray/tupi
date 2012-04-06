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

#include "tcollapsiblewidget.h"
#include "tdebug.h"

#include <QTextDocument>
#include <QMimeData>
#include <QPainter>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QPainterPath>
#include <QStyleOption>
#include <QGroupBox>
#include <QFontMetrics>

/******************************************************************
 * Helper classes
 *****************************************************************/

class CollapsibleMimeData : public QMimeData
{
    public:
        CollapsibleMimeData();
        ~CollapsibleMimeData();
        
        void setCollapsibleWidget(TCollapsibleWidget *w);
        TCollapsibleWidget *collapsibleWidget() const;
        
    private:
        TCollapsibleWidget *m_cw;
};

CollapsibleMimeData::CollapsibleMimeData() : QMimeData(), m_cw(0)
{
}

CollapsibleMimeData::~CollapsibleMimeData()
{
}

void CollapsibleMimeData::setCollapsibleWidget(TCollapsibleWidget *w)
{
    m_cw = w;
}

TCollapsibleWidget *CollapsibleMimeData::collapsibleWidget() const
{
    return m_cw;
}

TClickableLabel::TClickableLabel(QWidget* parent)
    : QWidget( parent ), m_isEnter(false), m_isDragging(false), m_checked(false)
{
    setMouseTracking(false);
    
    m_text = new QTextDocument(this);
}

TClickableLabel::~TClickableLabel()
{
}

QSize TClickableLabel::sizeHint() const
{
    return m_text->size().toSize();
}

void TClickableLabel::setText(const QString &text)
{
    m_text->setHtml(text);
}

QString TClickableLabel::text() const
{
    return m_text->toPlainText();
}

void TClickableLabel::setChecked(bool c)
{
    m_checked = c;
}

bool TClickableLabel::isChecked() const
{
    return m_checked;
}

void TClickableLabel::mousePressEvent(QMouseEvent *e)
{
    m_isDragging = false;
    
    m_position = e->pos();
    QWidget::mousePressEvent(e);
}

void TClickableLabel::mouseReleaseEvent(QMouseEvent *e)
{
    if (! m_isDragging) {
        m_checked = !m_checked;
        emit clicked();
    }
}

void TClickableLabel::mouseMoveEvent(QMouseEvent* e)
{
    QWidget::mouseMoveEvent(e);

    if ((e->pos() - m_position).manhattanLength() <  QApplication::startDragDistance())
        return;
    
    QDrag *drag = new QDrag(this);
    CollapsibleMimeData *mimeData = new CollapsibleMimeData;
    TCollapsibleWidget *parent = dynamic_cast<TCollapsibleWidget *>(parentWidget());
    
    if (! parent) 
        return;

    QWidget *inner = parent->innerWidget();

    if (! inner) 
        return;
    
    mimeData->setCollapsibleWidget(parent);
    
    QPixmap wpx = QPixmap::grabWidget(parent);
    QPainter p(&wpx);
    
    p.drawRoundRect(wpx.rect(), 10, 10);
    
    drag->setPixmap(wpx);
    drag->setMimeData(mimeData);
    
    m_isDragging = true;
}

void TClickableLabel::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    if (m_isEnter) {
        painter.save();
        
        QRect r = rect();
    
        double h = r.height();
        double h2 = r.height() / 2.0;
        QPainterPath path;
        path.addRect(r.x() + h2, r.y() + 0, r.width() - h2 * 2, r.height());
        path.addEllipse(r.x(), r.y(), h, h);
        path.addEllipse(r.x() + r.width() - h, r.y(), h, h);
        path.setFillRule(Qt::WindingFill);
        painter.setPen(Qt::NoPen);
        painter.setBrush( palette().brush(QPalette::Highlight) );
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawPath(path);
        
        painter.restore();
    }
    
    QRect r = rect();
    r.setX((int) (r.x() + m_text->textWidth())/2);
    
    m_text->drawContents(&painter, r);
    
    painter.end();
    QWidget::paintEvent(e);
}

void TClickableLabel::enterEvent(QEvent * e)
{
    m_isEnter = true;
    update();
    QWidget::enterEvent(e);
}

void TClickableLabel::leaveEvent(QEvent *e)
{
    m_isEnter = false;
    update();
    QWidget::leaveEvent(e);
}

/******************************************************************
 * Private classes
 *****************************************************************/

class TCollapsibleWidget::Private
{
    public:
        QGridLayout    *cwlayout;
        QWidget        *innerWidget;
        TClickableLabel *label;
};

class KSettingsContainer::Private
{
    public:
        QVBoxLayout *layout;
        QList<TCollapsibleWidget *> collapsibles;
};

/******************************************************************
 * Implementation
 *****************************************************************/

KSettingsContainer::KSettingsContainer(QWidget *parent)
    : QScrollArea(parent), k(new KSettingsContainer::Private)
{
    QWidget *w = new QWidget;
    QVBoxLayout *helperLay = new QVBoxLayout(w);
    k->layout = new QVBoxLayout;
    helperLay->addLayout(k->layout);
    helperLay->addStretch(2);
    setWidget(w);
    setWidgetResizable(true);
    
    setAcceptDrops(true);
}

KSettingsContainer::~KSettingsContainer()
{
    delete k;
}

TCollapsibleWidget* KSettingsContainer::insertWidget(QWidget *w, const QString& name)
{
    if (w && w->layout()) {
        QLayout *lay = w->layout();
        lay->setMargin(2);
        lay->setSpacing(0);
    }

    TCollapsibleWidget *cw = new TCollapsibleWidget(name);
    
    k->layout->addWidget(cw);
    cw->setInnerWidget(w);
    
    k->collapsibles << cw;
    
    cw->show();
    
    return cw;
}

void KSettingsContainer::removeWidget(QWidget *w)
{
    foreach (TCollapsibleWidget *cw, k->collapsibles) {
             if (cw->innerWidget() == w) {
                 k->collapsibles.removeAll(cw );
                 k->layout->removeWidget(cw);
            
                 break;
             }
    }
}

void KSettingsContainer::dragEnterEvent(QDragEnterEvent * event)
{
    setFocus();
    
    if (dynamic_cast<const CollapsibleMimeData *>(event->mimeData())) {
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

void KSettingsContainer::dragMoveEvent(QDragMoveEvent* event)
{
    if (dynamic_cast<const CollapsibleMimeData *>(event->mimeData()))
        event->acceptProposedAction();
    else
        event->ignore();
}

void KSettingsContainer::dropEvent(QDropEvent* event)
{
    if (const CollapsibleMimeData *mimeData = dynamic_cast<const CollapsibleMimeData *>(event->mimeData())) {
        k->layout->removeWidget( mimeData->collapsibleWidget());
        
        QWidget *child = childAt(event->pos());
        
        if (child) {
            if (TCollapsibleWidget *prev = dynamic_cast< TCollapsibleWidget *>(child)) {
                k->layout->insertWidget( k->layout->indexOf(prev)+1, mimeData->collapsibleWidget());
            } else if (TCollapsibleWidget *prev = dynamic_cast< TCollapsibleWidget *>(child->parentWidget())) {
                k->layout->insertWidget( k->layout->indexOf(prev)+1, mimeData->collapsibleWidget());
            } else {
                k->layout->addWidget(mimeData->collapsibleWidget());
            }
        } else {
            k->layout->addWidget(mimeData->collapsibleWidget());
        }
        
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

TCollapsibleWidget::TCollapsibleWidget(QWidget *parent)
    : QWidget(parent), k(new TCollapsibleWidget::Private)
{
    init();
}

TCollapsibleWidget::TCollapsibleWidget(const QString& caption, QWidget *parent)
    : QWidget(parent), k(new TCollapsibleWidget::Private)
{
    init();
    setCaption(caption);
}

void TCollapsibleWidget::init()
{
    k->innerWidget = 0;
    k->cwlayout = new QGridLayout(this);
    k->cwlayout->setMargin(0);
    
    k->label = new TClickableLabel;
    k->label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    
    k->cwlayout->addWidget(k->label, 1, 1);
    
    connect(k->label, SIGNAL(clicked()), this, SLOT(toggleExpanded()));
    
    setExpanded(false);
    setEnabled(false);
}

TCollapsibleWidget::~TCollapsibleWidget()
{
    delete k;
}

QWidget* TCollapsibleWidget::innerWidget() const
{
    return k->innerWidget;
}

void TCollapsibleWidget::setInnerWidget(QWidget *w)
{
    if (!w) 
        return;
    
    QGroupBox *container = new QGroupBox(this);
    w->setParent(container);
    
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    k->innerWidget = w;
    
    containerLayout->addWidget(w);

    k->cwlayout->addWidget(container, 2, 1);
    k->cwlayout->setRowStretch(2, 1);
    
    setEnabled(true);
    setExpanded(isExpanded());
}

void TCollapsibleWidget::setCaption(const QString& caption)
{
    k->label->setText(QString("<b>%1</b>").arg(caption));
}

QString TCollapsibleWidget::caption() const
{
    return k->label->text();
}

void TCollapsibleWidget::setExpanded(bool expanded)
{
    if (k->innerWidget) {

        setUpdatesEnabled(false);

        k->innerWidget->parentWidget()->setVisible(expanded);
        k->innerWidget->setVisible(expanded);
        k->label->setChecked(expanded);
        
        setUpdatesEnabled(true);
    }
}

bool TCollapsibleWidget::isExpanded() const
{
    return k->label->isChecked();
}

void TCollapsibleWidget::toggleExpanded()
{
    setExpanded(k->label->isChecked());
}
