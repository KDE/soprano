/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SERVER_DBUS_UTIL_H_
#define _SOPRANO_SERVER_DBUS_UTIL_H_

class QDBusMessage;
class QDBusError;
class QString;

namespace Soprano {

    namespace Error {
        class Error;
    }

    namespace DBus {
        /**
         * Encodes \p e into a DBus error reply and sends it back to the sender of \p m.
         */
        void sendErrorReply( const QDBusMessage& m, const Soprano::Error::Error& e );

        /**
         * Converts a DBus error as encoded by sendErrorReply() to a Soprano::Error.
         */
        Soprano::Error::Error convertError( const QDBusError& e );
    }
}

#endif
