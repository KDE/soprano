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

#ifndef _SOPRANO_SERVER_DBUS_CLIENT_MODEL_H_
#define _SOPRANO_SERVER_DBUS_CLIENT_MODEL_H_

#include <soprano/storagemodel.h>
#include <soprano/soprano_export.h>

namespace Soprano {
    namespace Client {

	/**
	 * \class DBusModel dbusmodel.h soprano/Client/DBusModel
	 *
	 * \brief Interface to a DBus server model.
	 *
	 * The DBusModel provides a fully functional interface
	 * to a Model provided through a %Soprano DBus server.
	 *
	 * Given the DBus service name and the object path of the Model
	 * in question it handles all DBus communication transparently.
	 *
	 * Normally there is no need to use this class directly as 
	 * DBusClient does create instances of it on request.
	 *
	 * \author Sebastian Trueg <trueg@kde.org>
	 */
	class DBusModel : public Soprano::StorageModel
	{
	    Q_OBJECT

	public:
	    /**
	     * Create a new DBus Model interface.
	     *
	     * \param serviceName The name of the DBus service that provides the Model.
	     * \param dbusObject The path to the Model object in the DBus service.
	     * \param backend The backend that created this model. Should always be set to 0.
	     */
	    DBusModel( const QString& serviceName, const QString& dbusObject, const Backend* backend = 0 );
	    ~DBusModel();

	    Error::ErrorCode addStatement( const Statement &statement );
	    NodeIterator listContexts() const;
	    QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;
	    StatementIterator listStatements( const Statement &partial ) const;
	    Error::ErrorCode removeStatement( const Statement &statement );
	    Error::ErrorCode removeAllStatements( const Statement &statement );
	    int statementCount() const;
	    bool isEmpty() const;
	    bool containsStatement( const Statement &statement ) const;
	    bool containsAnyStatement( const Statement &statement ) const;
	    Node createBlankNode();

	private:
	    class Private;
	    Private* const d;
	};
    }
}

#endif
