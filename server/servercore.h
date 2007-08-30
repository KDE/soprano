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

#include <QtNetwork/QTcpServer>

#include <soprano/error.h>


namespace Soprano {

    class Backend;
    class Model;
    class BackendSetting;

    namespace Server {
	/**
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
	 * Optionally ServerCore can be configured using normal BackendSetting
	 * settings through setBackendSettings().
	 *
	 * \code
	 * Soprano::Server::ServerCore core;
	 * core.start();
	 * \endcode
	 */
	class ServerCore : private QTcpServer, public Error::ErrorCache
	{
	    Q_OBJECT

	public:
	    ServerCore();
	    virtual ~ServerCore();

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
	     * instances. Be aware that Soprano::BACKEND_OPTION_STORAGE_DIR will be changed
	     * to include a subdir which is the Model's name.
	     *
	     * \param settings The settings to use for new Models.
	     */
	    void setBackendSettings( const QList<BackendSetting>& settings );

	    /**
	     * Get or create Model with the specific name.
	     * This method will use createModel() to create a new Model
	     * if none with the specified name exists.
	     *
	     * \param name The name of the requested Model.
	     */
	    Model* model( const QString& name );

	    /**
	     * Start the Server. Calling this method will make the Server
	     * listen on the specified port for incoming client connections.
	     *
	     * \param port The port to listen on for client connections.
	     *
	     * \return \p true if the server was successfully started, \p false otherwise.
	     */
	    bool start( quint16 port = DEFAULT_PORT );

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
	    virtual Model* createModel( const QList<BackendSetting>& settings );

	private:
	    void incomingConnection( int );

	    class Private;
	    Private* const d;
	};
    }
}

#endif