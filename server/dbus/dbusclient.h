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

#include <soprano/backend.h>
#include <soprano/error.h>
#include <soprano/soprano_export.h>

namespace Soprano {

    class Model;

    namespace Server {
	/**
	 * \class DBusClient dbusclient.h Server/DBusClient
	 *
	 * \brief Core class to handle a connection to a Soprano server through the
	 * DBus interface.
	 *
	 * DBusClient creates a connection to a running Soprano Server via its DBus
	 * interface. All DBus communication is handled internally.
	 */
	class SOPRANO_EXPORT DBusClient : public QObject, public Error::ErrorCache
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
	    Model* createModel( const QString& name, const BackendSettings& settings = BackendSettings() );

	private:
	    class Private;
	    Private* const d;
	};
    }
}

#endif
