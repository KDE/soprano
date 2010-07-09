/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2010 Thiago Macieira <thiago@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*
 * This file is a most ugly hack.
 *
 * Soprano defined 'uint qHash(const QUrl &)' as public API and
 * exported it from its library. Since Qt 4.7, Qt will provide its own
 * hashing function.
 *
 * To avoid breaking binary compatibility, this file defines the
 * function again and does the same as the Qt 4.7 code does.
 *
 * We cannot define the function on Mac OS X because its file format
 * does not allow for duplicated symbols. All other platforms allow
 * for duplication.
 *
 * Unfortunately, we can't #include <QUrl> because that would cause a
 * compilation error. So we need to divert to a separate .cpp that
 * implements the actual call (see the bottom of node.cpp).
 */

#include "soprano_export.h"
class QUrl;

SOPRANO_EXPORT uint qHash(const QUrl &);
namespace Soprano {
    uint soprano_qHash_QUrl(const QUrl &);
}

#if !((defined(Q_OS_MACX) || defined(_WIN32) ) && QT_VERSION >= 0x040700)
uint qHash(const QUrl &url)
{
    return Soprano::soprano_qHash_QUrl(url);
}
#endif
