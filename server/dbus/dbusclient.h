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

#ifndef _SOPRANO_SERVER_DBUS_CLIENT_H_
#define _SOPRANO_SERVER_DBUS_CLIENT_H_

#include <QtCore/QObject>

#include "backend.h"
#include "error.h"
#include "soprano_export.h"

namespace Soprano {

    class Model;

    namespace Client {

        class DBusModel;

        /**
         * \class DBusClient dbusclient.h Soprano/Client/DBusClient
         *
         * \brief Core class to handle a connection to a Soprano server through the
         * DBus interface.
         *
         * DBusClient creates a connection to a running Soprano Server via its DBus
         * interface. All DBus communication is handled internally.
         *
         * See DBusModel for details about thread-safety.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \sa \ref soprano_server_dbus
         */
        class SOPRANO_CLIENT_EXPORT DBusClient : public QObject, public Error::ErrorCache
        {
            Q_OBJECT

        public:
            /**
             * Create a new DBus client.
             *
             * \param service The DBus service name. If empty the client will use the
             * default Soprano service name.
             * \param parent The parent object.
             */
            DBusClient( const QString& service = QString(), QObject* parent = 0 );

            /**
             * Destructor
             */
            ~DBusClient();

            /**
             * Check if the service is valid and available.
             *
             * \return \p true if the Soprano server service could be found and used.
             * Otherwise returns \p false.
             */
            bool isValid() const;

            /**
             * Retrive a list of all models that are available.
             *
             * \return A list of model names to be used with createModel()
             */
            QStringList allModels() const;

            /**
             * Creates a new Model instance that wraps a dbus server model.
             *
             * \param name The name of the model to access.
             * \param settings Settings for future extension. Not used yet.
             *
             * \return A new Model instance wrapping the requested server
             * model or 0 on error (check lastError() for details.)
             */
            DBusModel* createModel( const QString& name, const BackendSettings& settings = BackendSettings() );

            /**
             * Deletes a model including all its data.
             *
             * \param name The name of the model to remove.
             *
             * \warning Calling this method will remove all data physically. It can not
             * be reverted. Use with care.
             */
            void removeModel( const QString& name );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
