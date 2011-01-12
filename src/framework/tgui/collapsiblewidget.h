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

#ifndef COLLAPSIBLEWIDGET_H
#define COLLAPSIBLEWIDGET_H

#include <QScrollArea>
#include <QLabel>
#include <QDebug>

class QScrollArea;
class CollapsibleWidget;
class ClickableLabel;

/**
  @short A scrollable container that contains groups of settings,
         usually in the form of CollapsibleWidgets.
  @author Daniel Molkentin <molkentin@kde.org>
 */
class Q_GUI_EXPORT SettingsContainer : public QScrollArea
{
    Q_ENUMS(CollapseState);
    Q_OBJECT;

    public:
        enum CollapseState { Collapsed, Uncollapsed };
        SettingsContainer(QWidget *parent = 0);
        ~SettingsContainer();

        CollapsibleWidget* insertWidget(QWidget* w, const QString& name);

    private:
        Q_DISABLE_COPY(SettingsContainer);
        class Private;
        Private *k;
};

/**
  @short A widget that has a caption and a collapsible widget
  @author Daniel Molkentin <molkentin@kde.org>
 */
class Q_GUI_EXPORT CollapsibleWidget : public QWidget
{
    Q_OBJECT
    public:
        CollapsibleWidget(QWidget *parent = 0);
        CollapsibleWidget(const QString& caption, QWidget *parent = 0);
        ~CollapsibleWidget();
  
        QString caption() const;
        bool isExpanded() const;

        QWidget* innerWidget() const;
        void setInnerWidget(QWidget *w);

    public slots:
        void setExpanded(bool collapsed);
        void setCaption(const QString& caption);

    protected:
        void init();

    private:
        Q_DISABLE_COPY(CollapsibleWidget);
        class Private;
        Private *k;
};

#endif // COLLAPSIBLEWIDGET_H
