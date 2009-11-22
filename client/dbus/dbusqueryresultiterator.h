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

#ifndef _SOPRANO_CLIENT_DBUS_QUERYRESULT_ITERATOR_H_
#define _SOPRANO_CLIENT_DBUS_QUERYRESULT_ITERATOR_H_

#include "queryresultiterator.h"
#include "soprano_export.h"

namespace Soprano {
    namespace Client {
        /**
         * \class DBusQueryResultIterator dbusqueryresultiterator.h Soprano/Client/DBusQueryResultIterator
         *
         * \brief Interface to a DBus server query result iterator.
         *
         * The DBusQueryResultIterator provides a fully functional interface
         * to a QueryResultIterator provided through a %Soprano D-Bus server. It can thus,
         * be seen as the counterpart to Server::DBusExportIterator.
         *
         * Given the DBus service name and the object path of the QueryResultIterator
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
        class SOPRANO_CLIENT_EXPORT DBusQueryResultIterator : public QueryResultIterator
        {
        public:
            /**
             * Create a new DBus QueryResultIterator interface.
             *
             * \param serviceName The name of the DBus service that provides the QueryResultIterator.
             * \param dbusObject The path to the QueryResultIterator object in the DBus service.
             */
            DBusQueryResultIterator( const QString& serviceName, const QString& dbusObject );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
