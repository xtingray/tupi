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

#ifndef KTMODULEWIDGETBASE_H
#define KTMODULEWIDGETBASE_H

#include <qlayout.h>
#include <qsizepolicy.h> 
#include <qobject.h>
#include <qevent.h>

#include <QBoxLayout>
#include <QDockWidget>

#include "kosd.h"
#include "ktprojectresponse.h"
#include "ktabstractprojectresponsehandler.h"
#include "ktglobal.h"

/**
 * @author Jorge Cuadrado
*/
class TUPI_EXPORT KTModuleWidgetBase : public QWidget, public KTAbstractProjectResponseHandler
{
	Q_OBJECT;
	
	public:
		KTModuleWidgetBase(QWidget *parent, const char *name = 0);
		~KTModuleWidgetBase();
		virtual void addChild(QWidget * child, Qt::Alignment alignment = 0);
		void setFont(const QFont &);
		
		QBoxLayout *boxLayout();
		
	private:
		struct Private;
		Private *const k;
		
	protected:
		virtual bool event( QEvent * e );
		
	public slots:
		bool handleProjectResponse(KTProjectResponse *response);
		
	protected:
		virtual void enterEvent(QEvent *e);
		virtual void leaveEvent(QEvent *e);
		virtual void frameResponse(KTFrameResponse *frameRequest);
		virtual void layerResponse(KTLayerResponse  *layerRequest);
		virtual void sceneResponse(KTSceneResponse  *sceneRequest);
		virtual void projectResponse(KTProjectResponse  *projectRequest);
		virtual void itemResponse(KTItemResponse  *event);
		virtual void libraryResponse(KTLibraryResponse *libraryResponse);
		
	signals:
		void documentModified(bool);
		void sendToStatus(const QString &);
		void toggle();
		void activate(bool);
		
		void requestTriggered(const KTProjectRequest *event);
		void localRequestTriggered(const KTProjectRequest *event);
		void postPage(QWidget *widget);
};

#endif
