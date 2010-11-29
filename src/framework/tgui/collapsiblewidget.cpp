/*
   This file is part of the KDE libraries
   Copyright (C) 2005 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2006 David Cuadrado <krawek@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 3 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QtGui>
#include "collapsiblewidget.h"

/******************************************************************
 * Helper classes
 *****************************************************************/

class ClickableLabel : public QLabel
{
    Q_OBJECT
    public:
        ClickableLabel(QWidget* parent = 0);
        ~ClickableLabel();

    protected:
        void paintEvent(QPaintEvent *e);
        void enterEvent(QEvent * e);
        void leaveEvent(QEvent *e);
        void mousePressEvent(QMouseEvent *e);

    signals:
        void clicked();

    private:
        bool m_isEnter;
};

ClickableLabel::ClickableLabel(QWidget* parent)
        : QLabel(parent), m_isEnter(false)
{
}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    emit clicked();
}

void ClickableLabel::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    if (m_isEnter) {
        QRect r = rect();
        double h = r.height();
        double h2 = r.height() / 2.0;
        QPainterPath path;
        path.addRect(r.x() + h2, r.y() + 0, r.width() - h2 * 2, r.height());
        path.addEllipse(r.x(), r.y(), h, h);
        path.addEllipse(r.x() + r.width() - h, r.y(), h, h);
        path.setFillRule(Qt::WindingFill);
        painter.setPen(Qt::NoPen);
        painter.setBrush( palette().brush(QPalette::Highlight));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawPath(path);
    }

    painter.end();
    QLabel::paintEvent(e);
}

void ClickableLabel::enterEvent(QEvent * e)
{
    m_isEnter = true;
    update();
    QLabel::enterEvent(e);
}

void ClickableLabel::leaveEvent(QEvent *e)
{
    m_isEnter = false;
    update();
    QLabel::leaveEvent(e);
}

class ArrowButton : public QAbstractButton
{
    public:
        ArrowButton(QWidget *parent = 0);
        ~ArrowButton();

         QSize sizeHint() const { return QSize(16, 16); }

    protected:
         void paintEvent(QPaintEvent*);
};

ArrowButton::ArrowButton(QWidget *parent)
    : QAbstractButton(parent)
{
}

ArrowButton::~ArrowButton()
{
}

void ArrowButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    int h = sizeHint().height();
    opt.rect = QRect(0,(height()- h )/2, h, h);
    opt.palette = palette();

    if (isEnabled()) 
        opt.state |= QStyle::State_Enabled;

    if (isChecked())
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &p);
    else
        style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &opt, &p);

    p.end();
}

/******************************************************************
 * Private classes
 *****************************************************************/

class CollapsibleWidget::Private
{
    public:
        QGridLayout *gridLayout;
        QWidget *innerWidget;
        ClickableLabel *label;
        ArrowButton *colButton;
};

class SettingsContainer::Private
{
    public:
        QVBoxLayout *layout;
};

/******************************************************************
 * Implementation
 *****************************************************************/

SettingsContainer::SettingsContainer(QWidget *parent) : QScrollArea(parent), k(new SettingsContainer::Private)
{
    QWidget *w = new QWidget;
    QVBoxLayout *helperLay = new QVBoxLayout(w);
    k->layout = new QVBoxLayout;
    helperLay->addLayout(k->layout);
    helperLay->addStretch(2);
    setWidget(w);
    setWidgetResizable(true);
}

SettingsContainer::~SettingsContainer()
{
    delete k;
}

CollapsibleWidget::CollapsibleWidget(QWidget *parent)
        : QWidget(parent), k(new CollapsibleWidget::Private)
{
    init();
}

CollapsibleWidget::CollapsibleWidget(const QString& caption, QWidget *parent) : QWidget(parent), k(new CollapsibleWidget::Private)
{
    init();
    setCaption(caption);
}

CollapsibleWidget* SettingsContainer::insertWidget(QWidget *w, const QString& name)
{
    if (w && w->layout()) {
        QLayout *lay = w->layout();
        lay->setMargin(2);
        lay->setSpacing(0);
    }

    CollapsibleWidget *cw = new CollapsibleWidget(name);
    k->layout->addWidget(cw);
    cw->setInnerWidget(w);

    return cw;
}

void CollapsibleWidget::init()
{
    k->innerWidget = 0;
    k->gridLayout = new QGridLayout(this);
    k->gridLayout->setMargin(0);

    k->colButton = new ArrowButton;
    k->colButton->setCheckable(true);

    k->label = new ClickableLabel;
    k->label->setSizePolicy(QSizePolicy::MinimumExpanding, 
                            QSizePolicy::Preferred);

    k->gridLayout->addWidget(k->colButton, 1, 1);
    k->gridLayout->addWidget(k->label, 1, 2);

    connect(k->label, SIGNAL(clicked()), 
            k->colButton, SLOT(click()));

    connect(k->colButton, SIGNAL(toggled(bool)), 
            SLOT(setExpanded(bool)));

    setExpanded(false);
    setEnabled(false);
}

CollapsibleWidget::~CollapsibleWidget()
{
    delete k;
}

QWidget* CollapsibleWidget::innerWidget() const
{
    return k->innerWidget;
}

void CollapsibleWidget::setInnerWidget(QWidget *w)
{
    if (!w) 
        return;

    QGroupBox *container = new QGroupBox(this);
    w->setParent(container);
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    k->innerWidget = w;

    containerLayout->addWidget(w);

    k->gridLayout->addWidget(container, 2, 2);
    k->gridLayout->setRowStretch(2, 1);
    setEnabled(true);
    setExpanded(isExpanded());
}

void CollapsibleWidget::setCaption(const QString& caption)
{
    k->label->setText(QString("<b>%1</b>").arg(caption));
}

QString CollapsibleWidget::caption() const
{
    return k->label->text();
}

void CollapsibleWidget::setExpanded(bool expanded)
{
    if (k->innerWidget) {
        k->innerWidget->parentWidget()->setVisible(expanded);
        k->innerWidget->setVisible(expanded);
    }
}

bool CollapsibleWidget::isExpanded() const
{
    return k->colButton->isChecked();
}

#include "collapsiblewidget.moc"
