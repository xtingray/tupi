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

#include "kosd.h"
#include "tdebug.h"

#include <QApplication>
#include <QBitmap>
#include <QTimer>
#include <QPainter>
#include <QDesktopWidget>
#include <QLinearGradient>

KOsd *KOsd::s_osd = 0;

KOsd::KOsd(QWidget * parent) : QWidget(parent), m_timer(0)
{
    setFocusPolicy(Qt::NoFocus);
    m_palette = palette();

    move(50, 50);
    resize(0, 0);
    hide();

    m_animator = new Animation;
    connect(&m_animator->timer, SIGNAL(timeout()), this, SLOT(animate()));

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), SLOT( hide()));

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::ToolTip);

    m_document = new QTextDocument(this);
}

KOsd::~KOsd()
{
    delete m_animator;
    delete m_timer;
}

void KOsd::display(const QString &title, const QString &message, Level level, int ms)
{
    if (message.isEmpty()) 
        return;

    QString htmlMessage = message;
    htmlMessage.replace('\n', "<br/>");
    QString tail = title + "</b></font><br><font style=\"font-size:11px\">" + htmlMessage + "</font>";

    //htmlMessage.replace('\n', "<br/>");

    QBrush background = palette().background();
    QBrush foreground = palette().foreground();

    if (level != None) {
  
        switch (level) {
                case Info:
                   {
                     QString logo = THEME_DIR + "icons/info_message.png";
                     background = QColor(0xc1e2fb);
                     m_document->setHtml("<img src=\"" + logo + "\"><font style=\"font-size:12px;\"><b>&nbsp;&nbsp;" \
                                         + tail);
                   }
                   break;
                case Warning:
                   {
                     QString logo = THEME_DIR + "icons/warning_message.png";
                     background = QColor(0xf77100);
                     m_document->setHtml("<img src=\"" + logo + "\"><font style=\"font-size:12px;\"><b>&nbsp;&nbsp;" \
                                         + tail);
                   }
                   break;
                case Error:
                   {
                     QString logo = THEME_DIR + "icons/error_message.png";
                     background = Qt::red;
                     m_document->setHtml("<img src=\"" + logo + "\"><font style=\"font-size:12px;\"><b>&nbsp;&nbsp;" \
                                         + tail);
                   }
                   break;
                case Fatal:
                   {
                     QString logo = THEME_DIR + "icons/fatal_message.png";
                     background = Qt::red;
                     m_document->setHtml("<img src=\"" + logo + "\"><font style=\"font-size:12px;\"><b>&nbsp;&nbsp;" \
                                         + tail);
                   }
                   break;
                default:
                   {
                     m_document->setHtml(htmlMessage);
                   }
                   break;
        }
    }

    if (ms < 0)
        ms = m_document->toPlainText().length() * 70;

    m_animator->level = level;

    QSizeF textSize = m_document->size();

    int width = (int)textSize.width() + 10;
    int height = (int)textSize.height() + 10;

    QDesktopWidget desktop;
    move((int) (desktop.screenGeometry().width() - textSize.width()) - 25, 
                (int) (desktop.screenGeometry().height() - textSize.height()) - 45);

    QRect geometry(0, 0, width + 10, height + 8);
    QRect geometry2(0, 0, width + 9, height + 7);

    // resize pixmap, mask and widget
    QBitmap mask;
    mask = QBitmap(geometry.size());
    m_pixmap = QPixmap(geometry.size());

    resize(geometry.size());

    // create and set transparency mask
    QPainter maskPainter(&mask);
    maskPainter.setRenderHint(QPainter::Antialiasing);
    mask.fill(Qt::white);

    maskPainter.drawRoundedRect(0, 0, width + 9, height + 7, 1, 1, Qt::AbsoluteSize);

    setMask(mask);

    maskPainter.end();
   
    drawPixmap(background, foreground);

    // show widget and schedule a repaint
    show();
    update();

    // close the message window after given mS
    if (ms > 0) {
        m_animator->timer.start(300);
        m_timer->start(ms);
    } else if (m_timer) {
        m_timer->stop();
    }
}

KOsd *KOsd::self()
{
    if (! s_osd)
        s_osd = new KOsd(QApplication::desktop());

    return s_osd;
}

void KOsd::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.drawPixmap(e->rect().topLeft(), m_pixmap, e->rect());
}

void KOsd::mousePressEvent(QMouseEvent *e)
{
    if (m_timer)
        m_timer->stop();

    hide();
}

void KOsd::animate()
{
    if (!isVisible())
        m_animator->timer.stop();

    QBrush background;

    if (m_animator->level == Info)
        return;

    if (m_animator->level == Error) {
        if (m_animator->on)
            background = Qt::red;
        else
            background = palette().background();
    } else if (m_animator->level == Warning) {
               if (m_animator->on)
                   background = QColor("orange");
               else
                   background = palette().background();
    } else if (m_animator->level == Fatal) {
               if (m_animator->on)
                   background = Qt::magenta;
               else
                   background = palette().background();
    }

    m_animator->on = m_animator->on ? false : true;

    drawPixmap(background, palette().foreground());

    repaint();
}

void KOsd::drawPixmap(const QBrush &background, const QBrush &foreground)
{
    QPixmap symbol;

    QRect textRect = QRect(QPoint(0, 0), m_document->size().toSize());

    QSizeF textSize = m_document->size();

    int width = (int)textSize.width() + 10;
    int height = (int)textSize.height() + 10;

    int textXOffset = 0;
    int shadowOffset = QApplication::isRightToLeft() ? -1 : 1;

    QRect geometry(0, 0, width + 10, height + 8);
    QRect geometry2(0, 0, width + 9, height + 7);

    textXOffset = 2;

    width += textXOffset;
    height = qMax(height, symbol.height());

    // draw background
    m_pixmap.fill(Qt::gray);
    QPainter bufferPainter(&m_pixmap);
    bufferPainter.setRenderHint(QPainter::Antialiasing);
    bufferPainter.setPen(QPen(QBrush(foreground), 1));

    QLinearGradient gradient(geometry.topLeft(), geometry.bottomLeft());

    QColor color0 = background.color();
    color0.setAlpha(180);

    QColor color1 = palette().color(QPalette::Button);
    color1.setAlpha(180);

    gradient.setColorAt(0.0, color0);
    gradient.setColorAt(1.0, color1);
    gradient.setSpread(QGradient::ReflectSpread);

    bufferPainter.setBrush(gradient); 
    bufferPainter.drawRoundedRect(0, 0, width + 8, height + 6, 1, 1, Qt::AbsoluteSize);

    // draw shadow and text
    bufferPainter.setPen(palette().background().color().dark(115));
    bufferPainter.translate(5 + textXOffset + shadowOffset, 1);

    m_document->drawContents(&bufferPainter,QRect(0,0, textRect.width(), textRect.height()));
}
