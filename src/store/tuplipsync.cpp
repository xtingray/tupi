/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "tuplipsync.h" 

TupPhoneme::TupPhoneme() : QObject()
{
}

TupPhoneme::TupPhoneme(const QString &value, int duration) : QObject()
{
    phoneme = value;
    frames = duration;
}

TupPhoneme::~TupPhoneme()
{
}

void TupPhoneme::setDuration(int duration)
{
    frames = duration;
}

int TupPhoneme::duration()
{
    return frames;
}

void TupPhoneme::setValue(const QString &value)
{
    phoneme = value;
}

QString TupPhoneme::value() const
{
    return phoneme;
}

void TupPhoneme::fromXml(const QString &xml)
{
    QDomDocument document;

    if (document.setContent(xml)) {
        QDomElement e = document.documentElement();
        if (!e.isNull()) {
            if (e.tagName() == "phoneme") {
                phoneme = e.attribute("value");
                frames = e.attribute("duration").toInt();
            }
        }
    }
}

QDomElement TupPhoneme::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("phoneme");
    root.setAttribute("value", phoneme);
    root.setAttribute("duration", frames);

    return root;
}

TupWord::TupWord()
{
}

TupWord::TupWord(int index)
{
    initIndex = index;
}

TupWord::~TupWord()
{
}

void TupWord::setInitFrame(int index)
{
    initIndex = index;
}

int TupWord::initFrame()
{
    return initIndex;
}

int TupWord::endFrame()
{
    return endIndex;
}

void TupWord::addPhoneme(TupPhoneme *phoneme)
{
    if (phoneme)
        phonemes << phoneme;
}

QList<TupPhoneme *> TupWord::phonemesList()
{
    return phonemes;
}

bool TupWord::contains(int frame)
{
    if (frame >= initIndex && frame <= endIndex)
        return true;

    return false;
}

void TupWord::fromXml(const QString &xml)
{
    QDomDocument document;

    if (document.setContent(xml)) {
        QDomElement root = document.documentElement();
        initIndex = root.attribute("initFrame").toInt();
        endIndex = initIndex - 1;
        QDomNode n = root.firstChild();

        while (!n.isNull()) {
               QDomElement e = n.toElement();
               if (!e.isNull()) {
                   if (e.tagName() == "phoneme") {
                       QString value = e.attribute("value");
                       int duration = e.attribute("duration").toInt();
                       endIndex += duration;

                       TupPhoneme *phoneme = new TupPhoneme(value, duration);
                       phonemes << phoneme;
                   }
               }

               n = n.nextSibling();
        }
    }
}

QDomElement TupWord::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("word");
    root.setAttribute("initFrame", initIndex);

    int total = phonemes.size();
    for(int i=0; i<total; i++) {
        TupPhoneme *phoneme = phonemes.at(i);
        root.appendChild(phoneme->toXml(doc));
    }

    return root;
}

TupPhrase::TupPhrase()
{
}

TupPhrase::TupPhrase(int index) : QObject()
{
    initIndex = index;
}

TupPhrase::~TupPhrase()
{
}

void TupPhrase::setInitFrame(int index)
{
    initIndex = index;
}

int TupPhrase::initFrame()
{
    return initIndex;
}

int TupPhrase::endFrame()
{
    return endIndex;
}

void TupPhrase::addWord(TupWord *word)
{
    if (word)
        words << word;
}

QList<TupWord *> TupPhrase::wordsList()
{
    return words;
}

bool TupPhrase::contains(int frame)
{
    if (frame >= initIndex && frame <= endIndex) 
        return true;

    return false;
}

void TupPhrase::fromXml(const QString &xml)
{
    QDomDocument document;

    if (document.setContent(xml)) {
        QDomElement root = document.documentElement();
        initIndex = root.attribute("initFrame").toInt();
        QDomNode n = root.firstChild();

        while (!n.isNull()) {
               QDomElement e = n.toElement();
               if (!e.isNull()) {
                   if (e.tagName() == "word") {
                       TupWord *word = new TupWord();
                       QString newDoc;
                       {
                           QTextStream ts(&newDoc);
                           ts << n;
                       }

                       word->fromXml(newDoc);
                       words << word;
                   }
               }

               n = n.nextSibling();
        }

        TupWord *last = words.last();
        endIndex = last->endFrame();
    }
}

QDomElement TupPhrase::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("phrase");
    root.setAttribute("initFrame", initIndex);

    int total = words.size();
    for(int i=0; i<total; i++) {
        TupWord *word = words.at(i);
        root.appendChild(word->toXml(doc));
    }

    return root;
}

TupVoice::TupVoice()
{
}

TupVoice::TupVoice(const QString &label, QPointF pos) 
{
    title = label;
    point = pos;
}

TupVoice::~TupVoice()
{
}

void TupVoice::setVoiceTitle(const QString &label)
{
    title = label;
}

QString TupVoice::voiceTitle() const
{
    return title;
}

void TupVoice::setMouthPos(QPointF pos)
{
    point = pos;
}

QPointF TupVoice::mouthPos()
{
    return point;
}

void TupVoice::setText(const QString &content)
{
    script = content;
}

QString TupVoice::text() const
{
    return script;
}

int TupVoice::initFrame()
{
    return initIndex;
}

int TupVoice::endFrame()
{
    return endIndex;
}

void TupVoice::addPhrase(TupPhrase *phrase)
{
    if (phrase)
        phrases << phrase;
}

QString TupVoice::getPhoneme(int frame)
{
    QString mouth = "rest";

    foreach (TupPhrase *phrase, phrases) {
             if (phrase->contains(frame)) {
                 foreach (TupWord *word, phrase->wordsList()) {
                          if (word->contains(frame)) {
                              int initFrame = word->initFrame();
                              foreach (TupPhoneme *phoneme, word->phonemesList()) {
                                       int duration = phoneme->duration();
                                       int first = initFrame; 
                                       int last = first + duration - 1;
                                       if (frame >= first && frame <= last) {
                                           mouth = phoneme->value();
                                           return mouth;
                                       }
                                       initFrame = last + 1;
                              }
                          }
                 }
             }
    }

    return mouth;
}

bool TupVoice::contains(int frame)
{
    if (frame >= initIndex && frame <= endIndex)
        return true;

    return false;
}

void TupVoice::fromXml(const QString &xml)
{
    QDomDocument document;

    if (document.setContent(xml)) {
        QDomElement root = document.documentElement();
        script = root.attribute("text");

        QDomNode n = root.firstChild();

        while (!n.isNull()) {
               QDomElement e = n.toElement();
               if (!e.isNull()) {
                   if (e.tagName() == "phrase") {
                       TupPhrase *phrase = new TupPhrase();
                       QString newDoc;
                       {
                           QTextStream ts(&newDoc);
                           ts << n;
                       }

                       phrase->fromXml(newDoc);
                       phrases << phrase;
                   }
               }
               n = n.nextSibling();
        }

        TupPhrase *first = phrases.first();
        initIndex = first->initFrame();

        TupPhrase *last = phrases.last();
        endIndex = last->endFrame();
    }
}

QDomElement TupVoice::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("voice");
    root.setAttribute("name", title);
    root.setAttribute("origin", QString::number(point.x()) + "," + QString::number(point.y()));
    root.setAttribute("text", script);

    int total = phrases.size();
    for(int i=0; i<total; i++) {
        TupPhrase *phrase = phrases.at(i);
        root.appendChild(phrase->toXml(doc));
    }

    return root;
}

struct TupLipSync::Private
{
    QString name;
    QString soundFile;
    QString extension;
    int fps;
    int initFrame;
    int framesCount;
    QList<TupVoice *> voices;
};

TupLipSync::TupLipSync() : QObject(), k(new Private)
{
}

TupLipSync::TupLipSync(const QString &name, const QString &soundFile, int initFrame) : QObject(), k(new Private)
{
    k->name = name;
    k->soundFile = soundFile;
    k->initFrame = initFrame;
}

TupLipSync::~TupLipSync()
{
}

QString TupLipSync::name() const
{
    return k->name;
}

void TupLipSync::setName(const QString &title)
{
    k->name = title;
}

void TupLipSync::setPicsExtension(const QString &extension)
{
    k->extension = extension;
}

QString TupLipSync::picExtension() const
{
    return k->extension;
}

QString TupLipSync::soundFile() const
{
    return k->soundFile;
}

void TupLipSync::setSoundFile(const QString &file)
{
    k->soundFile = file;
}

int TupLipSync::fps()
{
    return k->fps;
}

void TupLipSync::setFPS(int fps)
{
    k->fps = fps;
}

int TupLipSync::initFrame()
{
    return k->initFrame;
}

void TupLipSync::setInitFrame(int frame)
{
    k->initFrame = frame;
}

int TupLipSync::framesCount()
{
    return k->framesCount;
}

void TupLipSync::setFramesCount(int framesCount)
{
    k->framesCount = framesCount;
}

void TupLipSync::addVoice(TupVoice *voice)
{
    if (voice)
        k->voices << voice;
}

void TupLipSync::fromXml(const QString &xml)
{
    QDomDocument document;
   
    if (! document.setContent(xml)) {
        #ifdef K_DEBUG
            QString msg = "TupLipSync::fromXml() - File corrupted!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
                qWarning() << "Content:";
                qWarning() << xml;
            #else
                tError() << msg;
                tWarning() << "Content:";
                tWarning() << xml;
            #endif
        #endif

        return;
    }

    QDomElement root = document.documentElement();
    k->name = root.attribute("name");
    k->soundFile = root.attribute("soundFile");
    k->initFrame = root.attribute("initFrame").toInt();
    k->framesCount = root.attribute("framesTotal").toInt();
    k->extension = root.attribute("extension");
    k->fps = root.attribute("fps").toInt();

    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();
           if (!e.isNull()) {
               if (e.tagName() == "voice") {
                   QString name = e.attribute("name");
                   QStringList xy = e.attribute("origin").split(",");
                   double x = xy.first().toDouble();
                   double y = xy.last().toDouble();
                   QPointF point = QPointF(x, y);
                   TupVoice *voice = new TupVoice(name, point);

                   QString newDoc;
                   {
                       QTextStream ts(&newDoc);
                       ts << n;
                   }

                   voice->fromXml(newDoc);
                   k->voices << voice;
               }
           }
           n = n.nextSibling();
    }
}

QDomElement TupLipSync::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("lipsync");
    root.setAttribute("name", k->name);
    root.setAttribute("soundFile", k->soundFile);
    root.setAttribute("initFrame", k->initFrame);
    root.setAttribute("framesTotal", k->framesCount);
    root.setAttribute("extension", k->extension);
    root.setAttribute("fps", k->fps);

    int total = k->voices.size();
    for(int i=0; i<total; i++) {
        TupVoice *voice = k->voices.at(i);
        root.appendChild(voice->toXml(doc));
    }

    return root;
}

QList<TupVoice *> TupLipSync::voices()
{
    return k->voices;
}

TupVoice * TupLipSync::voiceAt(int index)
{
    if (index >= 0 && index < k->voices.count())
        return k->voices.at(index);

    return 0;
}

void TupLipSync::updateMouthPosition(int mouthIndex, QPointF point)
{
    TupVoice *voice = k->voices.at(mouthIndex);
    if (voice)
        voice->setMouthPos(point);
}

