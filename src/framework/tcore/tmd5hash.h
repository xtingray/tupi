/****************************************************************************
** Last updated [dd/mm/yyyy]: 01/10/2005
**
** Computes MD5 hash of a file or buffer [Requires Qt4 - QtCore module].
**
** The MD5 algorithm was written by Ron Rivest.
** This class is based on the code written by Colin Plumb in 1993,
** our understanding is that no copyright is claimed and that this code is in
** the public domain.
**
** NO WARRANTY is given for this code.
** Use it at your own risk.
** It should produce correct SHA1 digests, but no security-related test has been made.
**
** Copyright (C) 2005 Angius Fabrizio. All rights reserved.
**
** This file is part of the OSDaB project (http://osdab.sourceforge.net/).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See the file LICENSE.GPL that came with this software distribution or
** visit http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
**********************************************************************/

#ifndef TMD5HASH_H
#define TMD5HASH_H

#include "tglobal.h"
#include "tapplicationproperties.h"

#include <QStringList>

class QByteArray;
class QString;
class QFile;

class T_CORE_EXPORT TMD5Hash
{
    public:
        static QString hash(const QString &str);
        static QString hashData(const QByteArray& bytes);
        static QString hashData(const char* bytes, int size);
        static QString hashFile(QFile& file);
        static QString hashFile(const QString& filename);
        static QStringList passwords(const QString &plainPass);

    private:
        struct md5Context
        {
            unsigned int buffer[4];
            unsigned int bits[2];
            unsigned char in[64];
        };

        static void updateHash(md5Context* context, const char* data, long int length);
        static void md5Transform(unsigned int buf[4], unsigned int const in[16]);
        static QString finalize(md5Context& ctx);
};

#endif
