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

#ifndef _SOPRANO_SERVER_CORE_H_
#define _SOPRANO_SERVER_CORE_H_

#include <QtCore/QObject>

#include "error.h"
#include "soprano_export.h"

namespace Soprano {

    class Backend;
    class Model;
    class BackendSetting;

    namespace Server {

        class ServerCorePrivate;

        /**
         * \class ServerCore servercore.h Soprano/Server/ServerCore
         *
         * \brief Central %Soprano server class.
         *
         * The %ServerCore provides a %Soprano server which maintains a set
         * of named Model instances that can be accessed by clients over
         * a tcp connection through Server::BackendPlugin.
         *
         * Creating a server is very simple: Either derive from ServerCore or
         * create an instance and then call start() to make the server listen
         * for incoming connections.
         *
         * \code
         * Soprano::Server::ServerCore core;
         * core.start();
         * \endcode
         *
         * Optionally ServerCore can be configured using normal BackendSetting
         * settings through setBackendSettings().
         *
         * Be aware the ServerCode supports multiple ways of communication.
         * start() opens a TCP socket to accept new connections, registerAsDBusObject()
         * registers a DBus interface on the DBus session bus. Both ways of
         * communication can be used simultaneously.
         *
         * ServerCore will spawn a new thread for each incoming TCP or local socket
         * connection. In addition the D-Bus interface is running in its own thread.
         * The maximum number of threads that are spawned can be set via setMaximumConnectionCount().
         * By default there is no restriction on the maximum thread count to keep
         * backwards compatibility.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_SERVER_EXPORT ServerCore : public QObject, public Error::ErrorCache
        {
            Q_OBJECT

        public:
            ServerCore( QObject* parent = 0 );
            virtual ~ServerCore();

            /**
             * The default %Soprano server port: 5000
             */
            static const quint16 DEFAULT_PORT;

            /**
             * Set the Backend used in the Server to create Models.
             */
            void setBackend( const Backend* backend );

            /**
             * The Backend used by the Server to create Model instances.
             */
            const Backend* backend() const;

            /**
             * Set the settings that are to be used by createModel() to create new Model
             * instances. Be aware that Soprano::BackendOptionStorageDir will be changed
             * to include a subdir which is the Model's name.
             *
             * \param settings The settings to use for new Models.
             */
            void setBackendSettings( const QList<BackendSetting>& settings );

            /**
             * Retrieve the backend settings configured via setBackendSettings().
             *
             * \return A list of BackendSetting objects.
             */
            QList<BackendSetting> backendSettings() const;

            /**
             * Set the maximum number of TCP and local socket connections. If the max is reached
             * new incoming connections are refused.
             *
             * This makes sense to restrict the number of threads the server spawns.
             *
             * A typical usage would be to server a max number of clients via a local
             * socket connection and let the rest share the one D-Bus interface.
             *
             * \param max The maximum number of connections. Using a value of 0 means no restriction.
             *
             * \since 2.4
             */
            void setMaximumConnectionCount( int max );

            /**
             * The maximum number of incoming connections that are served.
             *
             * \return The maxium number of connections the server will handle
             * or 0 if there is no restriction.
             *
             * \sa setMaximumConnectionCount.
             *
             * \since 2.4
             */
            int maximumConnectionCount() const;

            /**
             * Get or create Model with the specific name.
             * The default implementation will use createModel() to create a new Model
             * if none with the specified name exists and protect it against deadlocks
             * with a Util::AsyncModel.
             *
             * \param name The name of the requested Model.
             */
            virtual Model* model( const QString& name );

            /**
             * Remove a model and delete all data in it.
             * Use with care.
             *
             * Be aware that the Model instance will also be deleted.
             *
             * Should be reimplemented if model() is reimplemented.
             *
             * \param name The name of the requested Model.
             *
             * \warning This is not the same as deleting the Model instance.
             * It will also delete all the data on the harddisk.
             */
            virtual void removeModel( const QString& name );

            /**
             * Retrieve all models that have been loaded.
             *
             * \return A list of Model names.
             */
            virtual QStringList allModels() const;

            /**
             * Start the core on a local socket.
             * On Windows this is a named pipe and on Unix this this is a local domain socket.
             *
             * Use Client::LocalSocketClient to connect.
             *
             * \warning Via the local socket connection signals are not supported.
             *
             * \return \p true if the local socket could be successfully
             * opened. \p false on error. See ErrorCache::lastError() for details.
             */
            bool start( const QString& socketPath = QString() );

            /**
             * Start the Server. Calling this method will make the Server
             * listen on the specified port for incoming client connections.
             *
             * \param port The port to listen on for client connections.
             *
             * Use Client::TcpClient to connect.
             *
             * \warning Via the TCP connection signals are not supported. Thus, the models created
             * by it will not emit signals such as Model::statementAdded. Also no permission handling or
             * any kind of security is implemented at the moment. Thus, if a server is running and is
             * listening on a port, it is open to connections from any client on any computer in the
             * network.
             *
             * \return \p true if the server was successfully started, \p false otherwise.
             */
            bool listen( quint16 port = DEFAULT_PORT );

            /**
             * \return The port this server is listening on or 0 if listen has not
             * been called successfully.
             *
             * \since 2.1
             */
            quint16 serverPort() const;

            /**
             * Register the ServerCore as a DBus object. The process needs to be registered
             * as a DBus service before (QDBusConnection::registerService()).
             *
             * \param objectPath The DBus object path to register the server as. If empty
             * the default path will be used (/org/soprano/Server).
             *
             * Use Client::DBusClient to connect.
             *
             * In case Soprano is compiled without D-Bus support this method does nothing.
             *
             * D-Bus clients will be served in a separate thread.
             */
            void registerAsDBusObject( const QString& objectPath = QString() );

        private Q_SLOTS:
            void serverConnectionFinished();

        protected:
            /**
             * Create a new Model. The default implementation uses the configured Backend
             * with the configured settings to create a new Model. This method can be
             * reimplemented to create specialized Model, for example in combination with
             * some FilterModel.
             *
             * \param settings The settings to use (a reimplementation may choose to ignore the
             * settings.)
             *
             * \return A newly create Model.
             */
            // FIXME: 3.0: this method should be const
            virtual Model* createModel( const QList<BackendSetting>& settings );

        private:
            ServerCorePrivate* const d;
        };
    }
}

#endif
