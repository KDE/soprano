/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_CLIENT_LOCAL_SOCKET_CLIENT_H_
#define _SOPRANO_CLIENT_LOCAL_SOCKET_CLIENT_H_

#include <QtCore/QObject>
#include <QtCore/QList>

#include "error.h"
#include "backend.h"
#include "soprano_export.h"

// FIXME: add signals for conection and disconnection, maybe even make it async as Qt's network stuff itself

namespace Soprano {

    class Model;

    namespace Client {
        /**
         * \class LocalSocketClient localsocketclient.h Soprano/Client/LocalSocketClient
         *
         * \brief Creates a connection to the %Soprano server through a local socket interface.
         *
         * The %Soprano server supports more than one way of communication. Beside D-Bus (see
         * DBusClient) it can be contacted via a local socket.
         *
         * Models creates by this client are thread-safe and provide faster access to the server
         * than D-Bus communication.
         *
         * \warning The LocalSocketClient does not support signals. Thus, the models created
         * by it will not emit signals such as Model::statementAdded.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \since 2.1
         */
        class SOPRANO_CLIENT_EXPORT LocalSocketClient : public QObject, public Error::ErrorCache
        {
            Q_OBJECT

        public:
            /**
             * Create a new Client instance.
             */
            LocalSocketClient( QObject* parent = 0 );

            /**
             * Destructor.
             */
            virtual ~LocalSocketClient();

            /**
             * Check if the client is connected to a server.
             *
             * \return \p true if this client is connected to a server, \p false
             * otherwise.
             */
            bool isConnected() const;

            /**
             * Creates a new Model instance that wraps a server model.
             * %Client models are very light wrappers and creating them is
             * very fast.
             *
             * \param name The name of the model to access.
             * \param settings The settings to send to the server for creating a new model.
             * These settings may be ignored by the server if a model with that name has
             * already been created.
             *
             * \return A new Model instance wrapping the requested server
             * model or 0 on error (check lastError() for details.)
             */
            Model* createModel( const QString& name, const QList<BackendSetting>& settings = QList<BackendSetting>() );

            /**
             * Deletes a model including all its data.
             *
             * \param name The name of the model to remove.
             *
             * \warning Calling this method will remove all data physically. It can not
             * be reverted. Use with care.
             */
            void removeModel( const QString& name );

        public Q_SLOTS:
            /**
             * Tries to connect to the %Soprano server.
             *
             * \return \p true on success, \p false if an error occured.
             * Check lastError() for details.
             */
            bool connect( const QString& name = QString() );

            /**
             * Disconnect from the server. The created model instances are not
             * deleted but remain useless; open iterators are closed.
             */
            void disconnect();

        private:
            class Private;
            Private* const d;

            Q_PRIVATE_SLOT( d, void _s_localSocketError( QLocalSocket::LocalSocketError ) )
        };
    }
}

#endif
