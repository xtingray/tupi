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

#ifndef KGSTENGINE_H
#define KGSTENGINE_H

#include <QObject>
#include <QHash>

#include <ksound/kaudioengineiface.h>

/*
#ifdef HAVE_GST10
#include <gst/gst.h>
#endif
*/

/**
 * @author David Cuadrado <krawek@gmail.com>
*/

class KGstEngine : public QObject, public KAudioEngineIface
{
    Q_OBJECT;
    Q_INTERFACES(KAudioEngineIface);
    
    public:
        KGstEngine();
        ~KGstEngine();
        virtual QString key() const;
        
        virtual int load(const QUrl &url, int id = -1);
        
        virtual bool init();
        virtual bool play(int offset = 0);
        
        virtual void stop();
        virtual void pause();
        virtual void seek(uint ms);
        
        bool setCurrentPlayer(int id);
        
        void setVolume(int percent);
        
        static KGstEngine *instance();

    private:
        static KGstEngine *s_instance;
        
        struct PlayInfo
        {
            KGstEngine *engine;
            GstElement *player;
            QUrl url;
            int id;
            bool stopped;
        };
        
        QHash<int, PlayInfo> m_players;
        
        int m_currentPlayer;
        
    private:
        static GstBusSyncReply bus_cb(GstBus*, GstMessage*, gpointer);
        void destroyPlayInfo(const PlayInfo *playInfo);
        
    protected:
        PlayInfo createPlayInfo(const QUrl &url);
};

#endif
