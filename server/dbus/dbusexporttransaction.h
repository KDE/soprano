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

#ifndef _SOPRANO_SERVER_DBUS_EXPORT_TRANSACTION_H_
#define _SOPRANO_SERVER_DBUS_EXPORT_TRANSACTION_H_

#include "dbusexportmodel.h"

namespace Soprano {
    namespace Server {
        class DBusExportTransaction : public DBusExportModel
        {
            Q_OBJECT

        public:
            DBusExportTransaction( Transaction* t );
            ~DBusExportTransaction();

            Transaction* transaction() const;

        public Q_SLOTS:
            /**
             * Register the transaction under the given D-Bus object path.
             * \sa QDBusConnection::registerObject
             *
             * \param dbusObjectPath The D-Bus object path to register the iterator under.
             * \param dbusClient The D-Bus client which uses the transaction, i.e. which called
             * the method that triggered the creation of the transaction. The transaction
             * will be deleted once the client dies.
             */
            bool registerTransaction( const QString& dbusObjectPath, const QString& dbusClient );

        private:
            class Private;
            Private* const d;

            Q_PRIVATE_SLOT( d, void slotServiceOwnerChanged( const QString& name, const QString&, const QString& ) )
        };
    }
}

#endif
