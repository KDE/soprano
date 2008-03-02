/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_CLIENT_DBUS_NODE_ITERATOR_H_
#define _SOPRANO_CLIENT_DBUS_NODE_ITERATOR_H_

#include "nodeiterator.h"
#include "soprano_export.h"

namespace Soprano {
    namespace Client {
        /**
         * \class DBusNodeIterator dbusnodeiterator.h Soprano/Client/DBusNodeIterator
         *
         * \brief Interface to a DBus server node iterator.
         *
         * The DBusNodeIterator provides a fully functional interface
         * to a NodeIterator provided through a %Soprano D-Bus server. It can thus,
         * be seen as the counterpart to Server::DBusExportIterator.
         *
         * Given the DBus service name and the object path of the NodeIterator
         * in question it handles all DBus communication transparently.
         *
         * Normally there is no need to use this class directly as 
         * DBusModel does create instances of it on request.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \sa \ref soprano_server_dbus
         *
         * \since 2.1
         */
        class SOPRANO_CLIENT_EXPORT DBusNodeIterator : public NodeIterator
        {
        public:
            /**
             * Create a new DBus NodeIterator interface.
             *
             * \param serviceName The name of the DBus service that provides the NodeIterator.
             * \param dbusObject The path to the NodeIterator object in the DBus service.
             */
            DBusNodeIterator( const QString& serviceName, const QString& dbusObject );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
