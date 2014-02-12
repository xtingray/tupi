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

#include "tcirclebutton.h"

#include <QPainter>
#include <QRegion>
#include <QRect>
#include <QPaintEvent>
#include <QDebug>
#include <QMoveEvent>
#include <QApplication>
#include <QBitmap>
#include <QPainterPath>
#include <QTimer>
#include <QPaintEngine>

static const char * new_xpm[] = {
"12 16 6 1",
"     c #040404",
".    c None",
"X    c white",
"o    c #808304",
"O    c black",
"+    c #f3f7f3",
"       .....",
" XXXXX  ....",
" XXXXX X ...",
" XXXXX XX ..",
" XooXX    O.",
" X+XXX+XXXO.",
" XXXXXXXXXO.",
" XoooXooXXO.",
" XXXXXXXXXO.",
" XXXXXXXXXO.",
" XoXXoooXXO.",
" XXXXXXXXXO.",
"OXXXXXXXXXO.",
"OXXXXXXXXXO.",
"OOOOOOOOOOO.",
"............"};

class TCircleButton::Animator
{
    public:
        Animator() : aStep(0), aBegin(false), m_interval(50) 
        {
            aTimer = new QTimer;
        }
        ~Animator() {};
        
        void begin()
        {
            aTimer->start(m_interval);
        }

        void end()
        {
            aTimer->stop();
        }
        
        QTimer *aTimer;
        int aStep;
        bool aBegin;
        
    private:
        int m_interval;
};

TCircleButton::TCircleButton(int diameter, bool animate, QWidget *parent) : QPushButton(parent), m_diameter(diameter)
{
    show();

    m_pix = QPixmap(new_xpm);
    
    setIcon(m_pix);
    
    paintMask();
    
    setMaximumSize(m_diameter, m_diameter);
    
    m_animator = new Animator;
    
    if (animate)
        connect(m_animator->aTimer, SIGNAL(timeout()), this, SLOT(animate()));
}

TCircleButton::~TCircleButton()
{
    delete m_animator;
}

QSize TCircleButton::sizeHint() const
{
    ensurePolished();

    int w = 0, h = 0;

    QStyleOptionButton opt = styleOption();

    // calculate contents size...
    if (!icon().isNull()) {
        int ih = opt.iconSize.height();
        int iw = opt.iconSize.width() + 4;
        w += iw;
        h = qMax(h, ih);
    }

    if (menu())
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
    
    QString s(text());
    bool empty = s.isEmpty();

    if (empty)
        s = QString::fromLatin1("XXXX");
    
    QFontMetrics fm = fontMetrics();
    QSize sz = fm.size(Qt::TextShowMnemonic, s);

    if (!empty || !w)
        w += sz.width();

    if (!empty || !h)
        h = qMax(h, sz.height());

    return (style()->sizeFromContents(QStyle::CT_PushButton, &opt, QSize(w, h), this).expandedTo(QApplication::globalStrut()));
}

QStyleOptionButton TCircleButton::styleOption() const
{
    QStyleOptionButton opt;
    opt.init(this);

    if (isEnabled())
        opt.state |= QStyle::State_Enabled;

    if (hasFocus())
        opt.state |= QStyle::State_HasFocus;

    if (isDown())
        opt.state |= QStyle::State_Sunken;

    if (! isFlat() && ! isDown())
        opt.state |= QStyle::State_Raised;

    opt.features = isDefault() ? QStyleOptionButton::DefaultButton : QStyleOptionButton::None;
    opt.text = text();
    opt.icon = icon();
    opt.iconSize = QSize(m_diameter,m_diameter);

    QRect r = rect();

    opt.rect = QRect(r.x(),r.y(), m_diameter, m_diameter);
    
    return opt;
}

void TCircleButton::paintMask()
{
    m_mask = QPixmap(m_diameter, m_diameter);
    m_mask.fill(Qt::transparent);
    
    QPainter paintMask(&m_mask);
    paintMask.setRenderHint(QPainter::Antialiasing);
    paintMask.setPen(QPen(palette().color(QPalette::Foreground ), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    paintMask.setBrush(QBrush(palette().color(QPalette::Foreground )));
    
    paintMask.drawEllipse(QRect(rect().x(),rect().y(),m_diameter, m_diameter));
    
    setMask(m_mask.mask());
    paintMask.end();
}

void TCircleButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QStyleOptionButton option = styleOption();
    
    style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
    
    painter.save();
    
    QPainterPath path;
    int internalRadio = m_diameter;
    
    // pen
    painter.setPen(QPen(palette().color(QPalette::Foreground ), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    path.addEllipse( rect().x(), rect().y(), m_diameter, m_diameter);
    painter.drawPath(path);
    
    path = QPainterPath();
    
    internalRadio /= 10;
    
    // pen
    painter.setPen(QPen(palette().color(QPalette::ButtonText), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    
    path.addEllipse( rect().x()+internalRadio, rect().y()+internalRadio, m_diameter-internalRadio*2, m_diameter-internalRadio*2);
    
    painter.drawPath(path);
    
    path = QPainterPath();
    
    // pen
    painter.setPen(QPen(palette().color(QPalette::Foreground ), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    internalRadio += 2;
    
    path.addEllipse( rect().x()+internalRadio, rect().y()+internalRadio, m_diameter-internalRadio*2, m_diameter-internalRadio*2);
    painter.drawPath(path);
    
    painter.restore();
}

void TCircleButton::animate()
{
    if (m_animator->aBegin)
        m_animator->aStep += 2;
    else
        m_animator->aStep -= 2;
    
    if (m_animator->aStep <= m_diameter - 5)
        m_animator->aBegin = true;
    
    if (m_animator->aBegin && m_animator->aStep >= m_diameter + 5)
        m_animator->aBegin = false;
    
    setMinimumSize(m_diameter, m_animator->aStep);
}

void TCircleButton::enterEvent(QEvent *)
{
    m_animator->aStep = m_diameter;
    
    m_animator->begin();
    m_animator->aBegin = true;
    
    update();
}

void TCircleButton::leaveEvent(QEvent *) 
{
    m_animator->aBegin = false;
    m_animator->aStep = 0;
    
    m_animator->end();
    
    setMaximumSize(m_diameter, m_diameter);
    setMinimumSize(m_diameter, m_diameter);
    setIconSize(QSize(m_diameter,m_diameter));
}
