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

#ifndef QNUMERIC_P_H
#define QNUMERIC_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qglobal.h"

static const unsigned char qt_be_inf_bytes[] = { 0x7f, 0xf0, 0, 0, 0, 0, 0, 0 };
static const unsigned char qt_le_inf_bytes[] = { 0, 0, 0, 0, 0, 0, 0xf0, 0x7f };
static const unsigned char qt_armfpa_inf_bytes[] = { 0, 0, 0xf0, 0x7f, 0, 0, 0, 0 };
static inline double qt_inf()
{
#ifdef QT_ARMFPA
    return *reinterpret_cast<const double *>(qt_armfpa_inf_bytes);
#else
    return *reinterpret_cast<const double *>(QSysInfo::ByteOrder == QSysInfo::BigEndian ? qt_be_inf_bytes : qt_le_inf_bytes);
#endif
}

// Signaling NAN
static const unsigned char qt_be_snan_bytes[] = { 0x7f, 0xf8, 0, 0, 0, 0, 0, 0 };
static const unsigned char qt_le_snan_bytes[] = { 0, 0, 0, 0, 0, 0, 0xf8, 0x7f };
static const unsigned char qt_armfpa_snan_bytes[] = { 0, 0, 0xf8, 0x7f, 0, 0, 0, 0 };
static inline double qt_snan()
{
#ifdef QT_ARMFPA
    return *reinterpret_cast<const double *>(qt_armfpa_snan_bytes);
#else
    return *reinterpret_cast<const double *>(QSysInfo::ByteOrder == QSysInfo::BigEndian ? qt_be_snan_bytes : qt_le_snan_bytes);
#endif
}

// Quiet NAN
static const unsigned char qt_be_qnan_bytes[] = { 0xff, 0xf8, 0, 0, 0, 0, 0, 0 };
static const unsigned char qt_le_qnan_bytes[] = { 0, 0, 0, 0, 0, 0, 0xf8, 0xff };
static const unsigned char qt_armfpa_qnan_bytes[] = { 0, 0, 0xf8, 0xff, 0, 0, 0, 0 };
static inline double qt_qnan()
{
#ifdef QT_ARMFPA
    return *reinterpret_cast<const double *>(qt_armfpa_qnan_bytes);
#else
    return *reinterpret_cast<const double *>(QSysInfo::ByteOrder == QSysInfo::BigEndian ? qt_be_qnan_bytes : qt_le_qnan_bytes);
#endif
}

static inline bool qt_is_inf(double d)
{
    uchar *ch = (uchar *)&d;
#ifdef QT_ARMFPA
    return (ch[3] & 0x7f) == 0x7f && ch[2] == 0xf0;
#else
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        return (ch[0] & 0x7f) == 0x7f && ch[1] == 0xf0;
    } else {
        return (ch[7] & 0x7f) == 0x7f && ch[6] == 0xf0;
    }
#endif
}

static inline bool qt_is_nan(double d)
{
    uchar *ch = (uchar *)&d;
#ifdef QT_ARMFPA
    return (ch[3] & 0x7f) == 0x7f && ch[2] > 0xf0;
#else
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        return (ch[0] & 0x7f) == 0x7f && ch[1] > 0xf0;
    } else {
        return (ch[7] & 0x7f) == 0x7f && ch[6] > 0xf0;
    }
#endif
}

static inline bool qt_is_finite(double d)
{
    uchar *ch = (uchar *)&d;
#ifdef QT_ARMFPA
    return (ch[3] & 0x7f) != 0x7f || (ch[2] & 0xf0) != 0xf0;
#else
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        return (ch[0] & 0x7f) != 0x7f || (ch[1] & 0xf0) != 0xf0;
    } else {
        return (ch[7] & 0x7f) != 0x7f || (ch[6] & 0xf0) != 0xf0;
    }
#endif
}

static inline bool qt_is_inf(float d)
{
    uchar *ch = (uchar *)&d;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        return (ch[0] & 0x7f) == 0x7f && ch[1] == 0x80;
    } else {
        return (ch[3] & 0x7f) == 0x7f && ch[2] == 0x80;
    }
}

static inline bool qt_is_nan(float d)
{
    uchar *ch = (uchar *)&d;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        return (ch[0] & 0x7f) == 0x7f && ch[1] > 0x80;
    } else {
        return (ch[3] & 0x7f) == 0x7f && ch[2] > 0x80;
    }
}

static inline bool qt_is_finite(float d)
{
    uchar *ch = (uchar *)&d;
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
        return (ch[0] & 0x7f) != 0x7f || (ch[1] & 0x80) != 0x80;
    } else {
        return (ch[3] & 0x7f) != 0x7f || (ch[2] & 0x80) != 0x80;
    }
}

#endif // QNUMERIC_P_H
