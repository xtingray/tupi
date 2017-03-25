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

#ifndef TCOLLAPSIBLEWIDGET_H
#define TCOLLAPSIBLEWIDGET_H

#include "tglobal.h"

#include <QScrollArea>
#include <QLabel>
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
#include <QDrag>

class QScrollArea;
class QTextDocument;

class T_GUI_EXPORT TClickableLabel : public QWidget
{
    Q_OBJECT

    public:
        TClickableLabel(QWidget* parent = 0);
        ~TClickableLabel();
        
        void setText(const QString &text);
        QString text() const;
        
        void setChecked(bool c);
        bool isChecked() const;
        QSize sizeHint() const;
        
    protected:
        void paintEvent(QPaintEvent *e);
        void enterEvent(QEvent * e);
        void leaveEvent(QEvent *e);
        void mousePressEvent(QMouseEvent *e);
        void mouseReleaseEvent(QMouseEvent *e);
        void mouseMoveEvent(QMouseEvent* e);
        
    signals:
        void clicked();
        
    private:
        bool m_isEnter;
        QPoint m_position;
        QTextDocument *m_text;
        bool m_isDragging;
        bool m_checked;
};

/**
  @short A widget that has a caption and a collapsible widget
  @author Daniel Molkentin <molkentin@kde.org>
 */
class T_GUI_EXPORT TCollapsibleWidget : public QWidget
{
    Q_OBJECT

    public:
        TCollapsibleWidget(QWidget *parent = 0);
        TCollapsibleWidget(const QString& caption, QWidget *parent = 0);
        ~TCollapsibleWidget();
    
        QString caption() const;
        bool isExpanded() const;

        QWidget* innerWidget() const;
        void setInnerWidget(QWidget *w);

    public slots:
        void setExpanded(bool collapsed);
        void setCaption(const QString& caption);
        
    private slots:
        void toggleExpanded();
        
    protected:
        void init();

    private:
        Q_DISABLE_COPY(TCollapsibleWidget);
        class Private;
        Private *k;
};

/**
  @short A scrollable container that contains groups of settings,
         usually in the form of TCollapsibleWidgets.
  @author Daniel Molkentin <molkentin@kde.org>
 */
class T_GUI_EXPORT KSettingsContainer : public QScrollArea
{
    Q_ENUMS(CollapseState);

    Q_OBJECT

    public:
        enum CollapseState { Collapsed, Uncollapsed };
        KSettingsContainer(QWidget *parent = 0);
        ~KSettingsContainer();

        TCollapsibleWidget* insertWidget(QWidget* w, const QString& name);
        void removeWidget(QWidget *w);
        
    protected:
        void dragEnterEvent(QDragEnterEvent * event);
        void dragMoveEvent(QDragMoveEvent* event);
        void dropEvent(QDropEvent* e);

    private:
        Q_DISABLE_COPY(KSettingsContainer);
        class Private;
        Private *k;
};

#endif // KCOLLAPSIBLEWIDGET_H
