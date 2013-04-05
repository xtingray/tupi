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

#include "tgstengine.h"
#include "talgorithm.h"
#include "tdebug.h"

#include <QtDebug>
#include <QTimer>
#include <QFile>

TGstEngine *TGstEngine::s_instance = 0;

GstBusSyncReply TGstEngine::bus_cb(GstBus*, GstMessage* msg, gpointer pinfo) // static
{
    PlayInfo *playInfo = static_cast<PlayInfo *>(pinfo);

    switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
               {
                 GError* error;
                 gchar* debugs;

                 gst_message_parse_error(msg,&error,&debugs);
                 qDebug() << "ERROR RECEIVED IN BUS_CB <" << error->message << ">";

                 playInfo->engine->destroyPlayInfo(playInfo);

                 delete playInfo;
               }
               break;
            case GST_MESSAGE_EOS:
               {
                 qDebug() << "FINISHED " << playInfo->id;
               }
               break;
            default: ;
    }

    gst_message_unref(msg);
    return GST_BUS_DROP;
}

TGstEngine *TGstEngine::instance()
{
    if (!s_instance) {
        qDebug("CREATING INSTANCE");
        s_instance = new TGstEngine;
    }

    return s_instance;
}

TGstEngine::TGstEngine() : m_currentPlayer(0)
{
}

TGstEngine::~TGstEngine()
{
    QHashIterator<int, PlayInfo> i(m_players);

    while (i.hasNext()) {
           i.next();
           gst_object_unref (GST_OBJECT (i.value().player));
    }

    gst_deinit();
}


QString TGstEngine::key() const
{
    return "gstreamer";
}

int TGstEngine::load( const QUrl &url, int id )
{
    QUrl path = url;

    if (path.scheme().isEmpty())
        path.setScheme("file");

    qDebug() << "LOAD: " << path.toString() << " " << m_currentPlayer;

    if (!QFile::exists(url.path())) {
        m_currentPlayer = -1;

        kError() << "Cannot load: " << path.toString();
        return -1;
    }

    if (id < 0) {
        id = m_players.count();
        if (m_players.contains(id))
            id = KAlgorithm::random();
        }

        m_players.insert(id, createPlayInfo(path));

        m_currentPlayer = id;

        m_players[id].id = id;

        gst_bus_set_sync_handler(gst_pipeline_get_bus(GST_PIPELINE(m_players[id].player)), bus_cb, &m_players[id]);

        return id;
}

bool TGstEngine::init()
{
    GError *err;
    if (!gst_init_check(NULL, NULL, &err)) {
        qDebug() << "Error gst_init_check";
        return false;
    }

    // Check if registry exists
    GstElement* dummy = gst_element_factory_make ( "fakesink", "fakesink" );
    if (!dummy) {
        qDebug() << "gst-register";
        return false;
    }

    gst_object_unref(dummy);

    return true;
}

bool TGstEngine::play(int offset)
{
    qDebug() << "PLAY " << m_currentPlayer;

    if (m_currentPlayer < 0)
        return false;
	
    if (m_players.contains(m_currentPlayer)) {
        g_object_set(G_OBJECT (m_players[m_currentPlayer].player), "uri", m_players[m_currentPlayer].url.toString().toLocal8Bit().data(), NULL);

        m_players[m_currentPlayer].stopped = false;
        gst_element_set_state(m_players[m_currentPlayer].player, GST_STATE_PLAYING);

        if (offset >= 0)
            seek(offset);

        return true;
    }

    return false;
}

void TGstEngine::stop()
{
    qDebug() << "STOP " << m_currentPlayer;
    if (m_players.contains(m_currentPlayer)) {
        m_players[m_currentPlayer].stopped = true;
        gst_element_set_state(m_players[m_currentPlayer].player, GST_STATE_NULL);
    }
}

void TGstEngine::pause()
{
    qDebug("PAUSE");
    if (m_players.contains(m_currentPlayer))
        gst_element_set_state (m_players[m_currentPlayer].player, GST_STATE_PAUSED);
}

void TGstEngine::seek(uint ms)
{
    qDebug() << "SEEKING "<< ms;
	
    if (m_players.contains(m_currentPlayer)) {
        gst_element_get_state(m_players[m_currentPlayer].player, NULL, NULL, 100*GST_MSECOND);

    if (!gst_element_seek (m_players[m_currentPlayer].player, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET, ms*GST_MSECOND, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE))
        qDebug("Can't seek");

        // gst_element_get_state(m_players[m_currentPlayer].player, NULL, NULL, 100*GST_MSECOND);
    }
}

TGstEngine::PlayInfo TGstEngine::createPlayInfo(const QUrl &url)
{
    qDebug("Create play Info");

    PlayInfo playInfo;
    playInfo.engine = this;
    playInfo.player = gst_element_factory_make("playbin", "play");
    playInfo.url = url;

    return playInfo;
}

bool TGstEngine::setCurrentPlayer(int id)
{
    if (m_players.contains(id)) {
        m_currentPlayer = id;
        return true;
    }

    return false;
}

void TGstEngine::destroyPlayInfo(const PlayInfo *playInfo)
{
    qDebug() << "Destroy play info" << playInfo->id << " players: " << m_players.count();

    if (!m_players.contains(playInfo->id)) 
        return;

    m_players.remove(playInfo->id);

    tDebug() << "UUU: " << m_players.count();

    if (playInfo->player != 0) {
        gst_element_set_state( playInfo->player, GST_STATE_NULL );
        gst_object_unref( GST_OBJECT( playInfo->player ) );
    }
}

void TGstEngine::setVolume(int percent)
{
    if (m_currentPlayer < 0)
        return;

    g_object_set(G_OBJECT(m_players[m_currentPlayer].player), "volume", percent*0.01, NULL);
}

Q_EXPORT_PLUGIN2(gst_engine, TGstEngine);
