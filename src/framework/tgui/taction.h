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

#ifndef TACTION_H
#define TACTION_H

#include <QAction>
#include <QKeySequence>

#include "tglobal.h"

class TActionManager;

/**
 * @brief la clase TAction hereda de QAction, para facilitar la creaccion de acciones, y dando un parametro para identificar las acciones.
 * @author David Cuadrado
*/
class T_GUI_EXPORT TAction : public QAction
{
    Q_OBJECT

    public:
        /**
         * Construye una accion con un padre y un identificador 
         * @param parent padre de la accion
         * @param id identificador de la accion
         */
        TAction(QObject * parent, const QString &id = QString());

        /**
         * Construye una accion con un texto, un padre y un identificador.
         * @param text texto de la accion.
         * @param parent padre de la accion.
         * @param id identificador de la accion.
         */
        TAction(const QString & text, QObject * parent, const QString &id = QString());

        /**
         * Construye una accion con un icono, un texto, un padre y un identificador
         * @param icon icono de la accion.
         * @param text texto de la accion.
         * @param parent padre de la accion.
         * @param id identificador de la accion.
         */
        TAction(const QIcon & icon, const QString & text, QObject * parent, const QString &id = QString());

        /**
         * Construye una accion con un icono, un texto, una tecla, un padre y un identificador.
         * 
         * @param icon icono de la accion.
         * @param text texto de la accion.
         * @param key string que define la tecla rapida que activa la accion.
         * @param parent padre de la accion.
         * @param id identificador de la accion.
         */
        TAction(const QIcon & icon, const QString & text, const QString &key, QObject * parent, const QString &id = QString());

        /**
	 * construye una accion con un icono, un padre y un identificador.
         * @param icon icono de la accion.
         * @param parent padre de la accion.
         * @param id identificador de la accion.
         */
        TAction(const QIcon & icon, QObject * parent, const QString &id = QString());

        /**
         * Construye una accion con un icono, una tecla rapida, un padre y un identificador
         * @param icon icono de la acion.
         * @param key string que define la tecla rapida que activa la accion.
         * @param parent padre de la accion.
         * @param id identificador de la accion.
         */
        TAction(const QIcon & icon,  const QKeySequence &key, QObject * parent, const QString &id = QString());

        /**
         * Construye una accion con un icono, un texto, una secuencia de teclas, un objeto, un slot, un padre y un id.
         * por ejemplo TAction *action = new KAction(QIcon(), "accion", QKeySequence(tr("a")), obj, SLOT(close()), this, "accion");
         * crea una accion que al ser activada ejecuta el slot close de el objeto obj.
         * @param icon icono de la accion.
         * @param text texto de la accion.
         * @param key teclas rapidas de la accion.
         * @param reciever objeto que es afectado por la accion.
         * @param slot slot del reciver que se ejecuta.
         * @param parent padre de la accion.
         * @param id identificador de la accion.
         */
        TAction(const QIcon & icon, const QString &text, const QKeySequence &key, QObject *reciever, const char *slot, QObject * parent, const QString &id = QString());

        TAction(const QString &text, const QKeySequence &key, QObject *reciever, const char *slot, QObject * parent, const QString &id = QString());

        /**
         * Destructor.
         */
        ~TAction();

        /**
         * Asigna un cursor a la accion.
         * @param cursor 
         */
        void setCursor(const QCursor &cursor);

        /**
         * @return el cursor asaciado a la accion.
         */
        QCursor cursor() const;

    private:
        void initWithManager(TActionManager * parent, const QString &id);
        void init();
		
    private:
        QCursor m_cursor;
};

#endif
