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

#ifndef _SERVER_BACKEND_H_
#define _SERVER_BACKEND_H_

#include <soprano/backend.h>
#include "../soprano/soprano_export.h"

#include <QObject>

namespace Soprano
{
    namespace Server
    {
	/**
	 * \brief Backend to access RDF storage models through the %Soprano daemon.
	 *
	 * Additonal BackendOptions supported:
	 * \li name - The name for the Model to be created. The daemon handles a set of models which are identified by a unique name.
	 * This option has to be specified.
	 * \li port - The port the Backend is supposed to try to connect to the soprano server.
	 *
	 * The Backend maintains a set of connections, one for each port, i.e. each different server instance.
	 */
	class SOPRANO_EXPORT ServerBackend : public QObject, public Soprano::Backend
	{
	    Q_OBJECT
	    Q_INTERFACES(Soprano::Backend)

	public:
	    /**
	     * Create a new ServerBackend instance.
	     */
	    ServerBackend();
	    ~ServerBackend();

	    StorageModel* createModel( const QList<BackendSetting>& settings = QList<BackendSetting>() ) const;
	    BackendFeatures supportedFeatures() const;

	private Q_SLOTS:
	    void modelDeleted();

	private:
	    class Private;
	    Private* const d;
	};
    }
}

#endif
