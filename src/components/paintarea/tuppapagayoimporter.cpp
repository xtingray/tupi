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

#include "tuppapagayoimporter.h" 

struct TupPapagayoImporter::Private
{
    bool isValid;
    int framesCount;
    int fps;
    TupLipSync *lipsync;
};

TupPapagayoImporter::TupPapagayoImporter(const QString &file, const QSize &projectSize, const QString &extension, 
                                         int initFrame) : QObject(), k(new Private)
{
    k->framesCount = 0;
    k->isValid = true;
    QFile input(file);

    QFileInfo info(file);
    QString name = info.fileName().toLower();
    k->lipsync = new TupLipSync();
    k->lipsync->setName(name);
    k->lipsync->setInitFrame(initFrame);
    k->lipsync->setPicsExtension(extension);

    if (input.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&input);
        int i = 0;
        int voicesNumber = 0;
        QString line;
        while (i < 5) {
               line = stream.readLine(); 
               switch (i) {
                   case 0:
                   {
                       if (!line.startsWith("lipsync version")) {
                           k->isValid = false;      
                           return;
                       }
                   } 
                   break;
                   /*
                   case 1:
                   {
                       // Load audio file
                       k->lipsync->setSoundFile(line.trimmed());
                   }
                   break;
                   */
                   case 2:
                   {
                       // FPS 
                       k->fps = line.trimmed().toInt();
                       k->lipsync->setFPS(k->fps);
                   }
                   break;
                   case 4:
                   {
                       // Total of voices 
                       voicesNumber = line.trimmed().toInt();
                   }
                   break;
               }
               i++;
        }

        int x = projectSize.width()/(voicesNumber+1);
        int y = projectSize.height()/2;

        for(int j=1; j<=voicesNumber; j++) {
            TupVoice *voice = new TupVoice(); 
            x = x*j;

            QPointF point(x, y);
            voice->setMouthPos(point);
            voice->setVoiceTitle(stream.readLine().trimmed());
            voice->setText(stream.readLine().trimmed());
            int numPhrases = stream.readLine().toInt();
            int numPhonemes = 0;
            int numWords;
            QString str;
            int firstFrame = 0;
            int lastFrame = 0;

            for (int p = 0; p < numPhrases; p++) {
                 QString text = stream.readLine().trimmed();
                 int phInitFrame = stream.readLine().toInt();
                 if (p == 0)
                     phInitFrame = 0;
                 stream.readLine();
                 TupPhrase *phrase = new TupPhrase(phInitFrame);
                 numWords = stream.readLine().toInt();

                 for (int w = 0; w < numWords; w++) {
                      QString str = stream.readLine().trimmed();
                      QStringList strList = str.split(' ', QString::SkipEmptyParts);
                      QString strWord; 
                      TupWord *word = 0;
                      if (strList.size() >= 4) {
                          strWord = strList.at(0);   
                          firstFrame = strList.at(1).toInt();
                          word = new TupWord(firstFrame);
                          lastFrame = strList.at(2).toInt();
                          word->setEndFrame(lastFrame);
                          numPhonemes = strList.at(3).toInt();
                      }
                      QList<int> frames;
                      QList<QString> blocks;
                      for (int ph = 0; ph < numPhonemes; ph++) {
                           str = stream.readLine().trimmed();
                           QStringList strList = str.split(' ', QString::SkipEmptyParts);
                           if (strList.size() >= 2) {
                               frames << strList.at(0).toInt();
                               blocks << strList.at(1).toLower();
                           }
                      } // for ph

                      for (int ph = 0; ph < numPhonemes-1; ph++) {
                           int total = frames.at(ph+1) - frames.at(ph);
                           for (int i=0; i<total; i++) {
                                TupPhoneme *phoneme = new TupPhoneme(blocks.at(ph), point);
                                word->addPhoneme(phoneme);
                           }
                      } // for ph

                      if (!frames.isEmpty()) {
                          int total = (lastFrame - frames.at(numPhonemes-1)) + 1;
                          for (int i=0; i<total; i++) {
                               TupPhoneme *phoneme = new TupPhoneme(blocks.at(numPhonemes-1), point);
                               word->addPhoneme(phoneme);
                          }
                      } else {
                          #ifdef K_DEBUG
                              QString msg = "TupPapagayoImporter() - Fatal Error: frames size is less than numPhonemes -> ";
                              msg += QString::number(frames.count()) + " < " + QString::number(numPhonemes);
                              #ifdef Q_OS_WIN
                                  qDebug() << msg;
                              #else
                                  tError() << msg;
                              #endif
                          #endif
                      }

                      if (w == numWords - 1) {
                          if (lastFrame > k->framesCount)
                              k->framesCount = lastFrame;
                      }
                      phrase->addWord(word);
                 } // for w
                 phrase->setEndFrame(k->framesCount);
                 voice->addPhrase(phrase); 
            }
            k->lipsync->addVoice(voice);
        }
        k->framesCount++;
        k->lipsync->setFramesCount(k->framesCount);
        k->lipsync->verifyStructure();
    } else {
        k->isValid = false;
        #ifdef K_DEBUG
            QString msg = "TupPapagayoImporter() - Fatal Error: Insufficient permissions to load file! -> " + file;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }

    input.close();
}

TupPapagayoImporter::~TupPapagayoImporter()
{
}

void TupPapagayoImporter::setSoundFile(const QString &soundFile)
{
    k->lipsync->setSoundFile(soundFile);
}

bool TupPapagayoImporter::fileIsValid()
{
    return k->isValid;
}

QString TupPapagayoImporter::file2Text() const
{
    QDomDocument document;
    QDomElement root = k->lipsync->toXml(document);

    QString xml;
    {
      QTextStream ts(&xml);
      ts << root;
    }

    return xml;
}

int TupPapagayoImporter::framesCount()
{
    return k->framesCount;
}

int TupPapagayoImporter::fps()
{
    return k->fps;
}


