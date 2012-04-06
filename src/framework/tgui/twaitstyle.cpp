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

#include "twaitstyle.h"
#include "timageeffect.h"

#include <QLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPixmapCache>
#include <QRadioButton>
#include <QString>
#include <QStyleOption>
#include <QtDebug>
#include <QPushButton>
#include <QGroupBox>

QPixmap cached(const QString &img, const QColor &color)
{
    if (QPixmap *p = QPixmapCache::find(img))
        return *p;

    QPixmap pm;
    QImage original(img);
    pm = QPixmap::fromImage(TImageEffect::fade(original, 0.7, color), Qt::OrderedDither | Qt::OrderedAlphaDither);
    if (pm.isNull())
        return QPixmap();

    QPixmapCache::insert(img, pm);
    return pm;
}

TWaitStyle::TWaitStyle()
    : QPlastiqueStyle()
{
    Q_INIT_RESOURCE(tgui_images);
}

void TWaitStyle::drawHoverRect(QPainter *painter, const QRect &r, const QBrush &brush) const
{
    double h = r.height();
    double h2 = r.height() / 2.0;
    QPainterPath path;
    path.addRect(r.x() + h2, r.y() + 0, r.width() - h2 * 2, r.height());
    path.addEllipse(r.x(), r.y(), h, h);
    path.addEllipse(r.x() + r.width() - h, r.y(), h, h);
    path.setFillRule(Qt::WindingFill);
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path);
}

void TWaitStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,QPainter *painter, const QWidget *widget) const
{
    Q_ASSERT(option);
    
    switch (element) {
        case PE_FrameFocusRect: break;
        case PE_IndicatorRadioButton:
        {
            if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
                bool hover = (button->state & State_Enabled) && (button->state & State_MouseOver);
                painter->save();
                QPixmap radio;
                if (hover)
                    drawHoverRect(painter, widget->rect(), option->palette.background() );

                if (button->state & State_Sunken)
                    radio = cached(":dwaitstyle/images/radiobutton-on.png", option->palette.color(QPalette::Background) );
                else if (button->state & State_On)
                    radio = cached(":dwaitstyle/images/radiobutton_on.png", option->palette.color(QPalette::Background));
                else
                    radio = cached(":dwaitstyle/images/radiobutton_off.png", option->palette.color(QPalette::Background));
                painter->drawPixmap(button->rect.topLeft(), radio);

                painter->restore();
            }
            else
            {
                QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
            }
            break;
        }
        case PE_PanelButtonCommand:
        {
            const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option);
            if (button) {
                bool hover = (button->state & State_Enabled) && (button->state & State_MouseOver);

                const QPushButton *pushButton = qobject_cast<const QPushButton *>(widget);
                
                if (! pushButton ) {
                    QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
                    break;
                }
                
                painter->save();
                QWidget *parent = pushButton->parentWidget();

                if (parent && qobject_cast<QGroupBox *>(parent)) {
                    QLinearGradient lg(0, 0, 0, parent->height());
                    lg.setColorAt(0, QColor(224,224,224));
                    lg.setColorAt(1, QColor(255,255,255));
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(lg);
                    painter->setBrushOrigin(-widget->mapToParent(QPoint(0,0)));
                    painter->drawRect(button->rect);
                    painter->setBrushOrigin(0,0);
                }
                                
                bool down = pushButton && ((button->state & State_Sunken) || (button->state & State_On));
                
                bool goodSize = pushButton && pushButton->height() > 22;
                
                QPixmap left, right, mid;

                if (down && goodSize) {
                    left = cached(":dwaitstyle/images/button_pressed_cap_left.png", 
                                  option->palette.color(QPalette::Background));
                    
                    right = cached(":dwaitstyle/images/button_pressed_cap_right.png", 
                                   option->palette.color(QPalette::Background));

                    mid = cached(":dwaitstyle/images/button_pressed_stretch.png", 
                                 option->palette.color(QPalette::Background));
                } else if (goodSize) {
                    left = cached(":dwaitstyle/images/button_normal_cap_left.png", 
                                  option->palette.color(QPalette::Background));
                    right = cached(":dwaitstyle/images/button_normal_cap_right.png", 
                                   option->palette.color(QPalette::Background));
                    mid = cached(":dwaitstyle/images/button_normal_stretch.png", 
                                 option->palette.color(QPalette::Background));
                }
                
                if (goodSize) {
                    painter->drawPixmap(button->rect.topLeft(), left);
                    painter->drawTiledPixmap(QRect(button->rect.x() + left.width(),button->rect.y(),
                                             button->rect.width() - left.width() - right.width(),
                                             left.height()), mid);
                    painter->drawPixmap(button->rect.x() + button->rect.width() - right.width(),
                                        button->rect.y(), right);
                }
                
                if (hover)
                    painter->fillRect(widget->rect().adjusted(3,5,-3,-5), QColor(41,57,127,63));

                painter->restore();
                
                if (!goodSize)
                    QPlastiqueStyle::drawPrimitive(element, option, painter, widget);

            } else {
                QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
            }
            break;
        }
        case PE_Frame:
        {
            
        }
        break;
        case PE_FrameGroupBox:
        {
            const QStyleOptionFrameV2 *group = qstyleoption_cast<const QStyleOptionFrameV2 *>(option);

            if (group) {
                const QRect &r = group->rect;

                if (r.height() < 14) { // FIXME FIXME FIXME
                    QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
                    return;
                }
                
                painter->save();
                int radius = 14;
                int radius2 = radius*2;
                QPainterPath clipPath;
                clipPath.moveTo(radius, 0);
                clipPath.arcTo(r.right() - radius2, 0, radius2, radius2, 90, -90);
                clipPath.arcTo(r.right() - radius2, r.bottom() - radius2, radius2, radius2, 0, -90);
                clipPath.arcTo(r.left(), r.bottom() - radius2, radius2, radius2, 270, -90);
                clipPath.arcTo(r.left(), r.top(), radius2, radius2, 180, -90);
                painter->setClipPath(clipPath);
                QPixmap titleStretch = cached(":dwaitstyle/images/title_stretch.png", 
                                              option->palette.color(QPalette::Background));
                QPixmap topLeft = cached(":dwaitstyle/images/groupframe_topleft.png", 
                                         option->palette.color(QPalette::Background));
                QPixmap topRight = cached(":dwaitstyle/images/groupframe_topright.png", 
                                          option->palette.color(QPalette::Background));
                QPixmap bottomLeft = cached(":dwaitstyle/images/groupframe_bottom_left.png", 
                                            option->palette.color(QPalette::Background));
                QPixmap bottomRight = cached(":dwaitstyle/images/groupframe_bottom_right.png", 
                                             option->palette.color(QPalette::Background));
                QPixmap leftStretch = cached(":dwaitstyle/images/groupframe_left_stretch.png", 
                                             option->palette.color(QPalette::Background));
                QPixmap topStretch = cached(":dwaitstyle/images/groupframe_top_stretch.png", 
                                            option->palette.color(QPalette::Background));
                QPixmap rightStretch = cached(":dwaitstyle/images/groupframe_right_stretch.png", 
                                              option->palette.color(QPalette::Background));
                QPixmap bottomStretch = cached(":dwaitstyle/images/groupframe_bottom_stretch.png", 
                                               option->palette.color(QPalette::Background));

                QLinearGradient lg(0, 0, 0, r.height());
                
                lg.setColorAt(0, widget->palette().color(QPalette::Background));
                lg.setColorAt(1, widget->palette().color(QPalette::Light));
                
                painter->setPen(Qt::NoPen);
                painter->setBrush(lg);
                painter->drawRect(r.adjusted(0, titleStretch.height()/2, 0, 0));
                painter->setClipping(false);

                int topFrameOffset = titleStretch.height()/2 - 2;
                painter->drawPixmap(r.topLeft() + QPoint(0, topFrameOffset), topLeft);
                painter->drawPixmap(r.topRight() - QPoint(topRight.width()-1, 0)
                        + QPoint(0, topFrameOffset), topRight);
                painter->drawPixmap(r.bottomLeft() - QPoint(0, bottomLeft.height()-1), bottomLeft);
                painter->drawPixmap(r.bottomRight() - QPoint(bottomRight.width()-1,
                        bottomRight.height()-1), bottomRight);

                QRect left = r;
                left.setY(r.y() + topLeft.height() + topFrameOffset);
                left.setWidth(leftStretch.width());
                left.setHeight(r.height() - topLeft.height() - bottomLeft.height() - topFrameOffset);
                painter->drawTiledPixmap(left, leftStretch);

                QRect top = r;
                top.setX(r.x() + topLeft.width());
                top.setY(r.y() + topFrameOffset);
                top.setWidth(r.width() - topLeft.width() - topRight.width());
                top.setHeight(topLeft.height());
                painter->drawTiledPixmap(top, topStretch);

                QRect right = r;
                right.setX(r.right() - rightStretch.width()+1);
                right.setY(r.y() + topRight.height() + topFrameOffset);
                right.setWidth(rightStretch.width());
                right.setHeight(r.height() - topRight.height() - bottomRight.height() - topFrameOffset);
                painter->drawTiledPixmap(right, rightStretch);

                QRect bottom = r;
                bottom.setX(r.x() + bottomLeft.width());
                bottom.setY(r.bottom() - bottomStretch.height()+1);
                bottom.setWidth(r.width() - bottomLeft.width() - bottomRight.width());
                bottom.setHeight(bottomLeft.height());
                painter->drawTiledPixmap(bottom, bottomStretch);
                painter->restore();
            }
            else
            {
                QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
            }
            break;
        }
        default:
            QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
            break;
    }
    
    
    return;
}

void TWaitStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch (element) {
        default:
        {
            QPlastiqueStyle::drawControl(element, option, painter, widget);
        }
        break;
    }
}

void TWaitStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    switch (control) {
        case CC_Slider:
        {
            if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
                QRect groove = subControlRect(CC_Slider, option, SC_SliderGroove, widget);
                QRect handle = subControlRect(CC_Slider, option, SC_SliderHandle, widget);

                painter->save();

                bool hover = (slider->state & State_Enabled) && (slider->state & State_MouseOver);
                if (hover) {
                    QRect moderated = widget->rect().adjusted(0, 4, 0, -4);
                    drawHoverRect(painter, moderated, option->palette.background());
                }

                if ((option->subControls & SC_SliderGroove) && groove.isValid()) {
                    QPixmap grv = cached(":dwaitstyle/images/slider_bar.png", 
                                         option->palette.color(QPalette::Background));
                    painter->drawPixmap(QRect(groove.x() + 5, groove.y(),
                            groove.width() - 10, grv.height()), grv);
                }

                if ((option->subControls & SC_SliderHandle) && handle.isValid()) {
                    QPixmap hndl = cached(":dwaitstyle/images/slider_thumb_on.png", 
                                   option->palette.color(QPalette::Background));
                    painter->drawPixmap(handle.topLeft(), hndl);
                }

                painter->restore();

            } else {
                QPlastiqueStyle::drawComplexControl(control, option, painter, widget);
            }
            
            break;
        }
        case CC_GroupBox:
        {
            const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(option);
            if (groupBox) {
                QStyleOptionGroupBox groupBoxCopy(*groupBox);
                groupBoxCopy.subControls &= ~SC_GroupBoxLabel;
                QPlastiqueStyle::drawComplexControl(control, &groupBoxCopy, painter, widget);
                
                if (groupBox->subControls & SC_GroupBoxLabel) {
                    const QRect &r = groupBox->rect;
                    QPixmap titleLeft = cached(":dwaitstyle/images/title_cap_left.png", 
                                               option->palette.color(QPalette::Background));
                    QPixmap titleRight = cached(":dwaitstyle/images/title_cap_right.png", 
                                                option->palette.color(QPalette::Background));
                    QPixmap titleStretch = cached(":dwaitstyle/images/title_stretch.png", 
                                                  option->palette.color(QPalette::Background));
                    int txt_width = groupBox->fontMetrics.width(groupBox->text) + 20;
                    painter->drawPixmap(r.center().x() - txt_width/2, 0, titleLeft);
                    QRect tileRect = subControlRect(control, groupBox, SC_GroupBoxLabel, widget);
                    painter->drawTiledPixmap(tileRect, titleStretch);
                    painter->drawPixmap(tileRect.x() + tileRect.width(), 0, titleRight);
                    int opacity = 31;
                    painter->setPen(QColor(0, 0, 0, opacity));
                    painter->drawText(tileRect.translated(0, 1),
                            Qt::AlignVCenter | Qt::AlignHCenter, groupBox->text);
                    painter->drawText(tileRect.translated(2, 1),
                            Qt::AlignVCenter | Qt::AlignHCenter, groupBox->text);
                    painter->setPen(QColor(0, 0, 0, opacity * 2));
                    painter->drawText(tileRect.translated(1, 1),
                            Qt::AlignVCenter | Qt::AlignHCenter, groupBox->text);
                    painter->setPen(Qt::white);
                    painter->drawText(tileRect, Qt::AlignVCenter | Qt::AlignHCenter, groupBox->text);
                }
            } else {
                QPlastiqueStyle::drawComplexControl(control, option, painter, widget);
            }
            break;
        }
        default:
            QPlastiqueStyle::drawComplexControl(control, option, painter, widget);
            break;
    }
    return;
}

QRect TWaitStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget) const
{
    QRect rect;

    switch (control) {
        default:
        {
            rect = QPlastiqueStyle::subControlRect(control, option, subControl, widget);
            break;
        }
#if QT_VERSION >= 0x040100
        case CC_GroupBox:
        {
            if (const QStyleOptionGroupBox *group = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
                switch (subControl) {
                    default:
                        rect = QPlastiqueStyle::subControlRect(control, option, subControl, widget);
                        break;
#if QT_VERSION >= 0x040100
                    case SC_GroupBoxContents:
                        rect = QPlastiqueStyle::subControlRect(control, option, subControl, widget);
                        if (group->text.length() < 4) {
                            rect.adjust(0, 4, 0, 0);
                        } else {
                            rect.adjust(0, -10, 0, 0);
                        }
                        break;
#endif
                    case SC_GroupBoxFrame:
                        rect = group->rect;
                        break;
                    case SC_GroupBoxLabel:
                        QPixmap titleLeft = cached(":dwaitstyle/images/title_cap_left.png", 
                                                   option->palette.color(QPalette::Background));
                        QPixmap titleRight = cached(":dwaitstyle/images/title_cap_right.png", 
                                                    option->palette.color(QPalette::Background));
                        QPixmap titleStretch = cached(":dwaitstyle/images/title_stretch.png", 
                                                      option->palette.color(QPalette::Background));
                        int txt_width = group->fontMetrics.width(group->text) + 20;
                        rect = QRect(group->rect.center().x() - txt_width/2 + titleLeft.width(), 0,
                                txt_width - titleLeft.width() - titleRight.width(),
                                titleStretch.height());
                        break;
                }
            }
            break;
        }
#endif
    }

    if (control == CC_Slider && subControl == SC_SliderHandle) {
        rect.setWidth(13);
        rect.setHeight(27);
    } else if (control == CC_Slider && subControl == SC_SliderGroove) {
        rect.setHeight(9);
        rect.moveTop(27/2 - 9/2);
    }

    return rect;
}

QSize TWaitStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    QSize newSize = QPlastiqueStyle::sizeFromContents(type, option, size, widget);

    switch (type) {

       /*
        case CT_PushButton:
            newSize.boundedTo (QSize(27, 20));
            break;
       */

        case CT_Slider:
            newSize.setHeight(20);
            break;

        default:
            break;
    }

    return newSize;
}

int TWaitStyle::pixelMetric(PixelMetric pm, const QStyleOption *opt, const QWidget *widget) const
{
    if (pm == PM_SliderLength)
        return 13;
    
    switch (pm) {
           case QStyle::PM_DockWidgetSeparatorExtent:
           {
                return 1;
           }
           break;

           /*
           case QStyle::PM_DockWidgetHandleExtent:
           {
               return 0;
           }
           break;
           case QStyle::PM_DockWidgetFrameWidth:
           {
               return 0;
           }
           break;
           case QStyle::PM_DockWidgetTitleMargin:
           {
               return 0;
           }
           break;
           */

           default: 
           break;
    }
    
    return QPlastiqueStyle::pixelMetric(pm, opt, widget);
}

void TWaitStyle::polish(QWidget *widget)
{
    QPlastiqueStyle::polish(widget);

    if (widget->layout() && qobject_cast<QGroupBox *>(widget)) {
        if (qFindChildren<QGroupBox *>(widget).size() == 0) {
            widget->layout()->setSpacing(0);
        } else {
            widget->layout()->setMargin(10);
            // widget->layout()->setSpacing(0);
        }
    }

    if (qobject_cast<QPushButton *>(widget)
           || qobject_cast<QRadioButton *>(widget)
           || qobject_cast<QSlider *>(widget)) {
        widget->setAttribute(Qt::WA_Hover);
    }
}

void TWaitStyle::unpolish(QWidget *widget)
{
    if (qobject_cast<QPushButton *>(widget)
        || qobject_cast<QRadioButton *>(widget)
        || qobject_cast<QSlider *>(widget)) {
        widget->setAttribute(Qt::WA_Hover, false);
    }
}

void TWaitStyle::polish(QPalette &palette)
{
    // palette.setColor(QPalette::Background, QColor(241, 241, 241));
}

QRect TWaitStyle::subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const
{
    QRect r;
    switch (element) {
        case SE_RadioButtonClickRect:
            r = widget->rect();
            break;
        case SE_RadioButtonContents:
            r = widget->rect().adjusted(20, 0, 0, 0);
            break;
        default:
            r = QPlastiqueStyle::subElementRect(element, option, widget);
            break;
    }

    if (qobject_cast<const QRadioButton*>(widget))
        r = r.adjusted(5, 0, -5, 0);

    return r;
}
