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

#ifndef _SOPRANO_SERVER_CLIENT_MODEL_H_
#define _SOPRANO_SERVER_CLIENT_MODEL_H_

#include <soprano/storagemodel.h>

#include <QtCore/QList>


namespace Soprano {
    namespace Server {

	class Client;

	class ClientModel : public StorageModel
	{
	public:
	    ClientModel( const Backend* backend, int modelId, Client* );
	    ~ClientModel();

	    Error::ErrorCode addStatement( const Statement &statement );
	    NodeIterator listContexts() const;
	    QueryResultIterator executeQuery( const QueryLegacy &query ) const;
	    StatementIterator listStatements( const Statement &partial ) const;
	    Error::ErrorCode removeStatements( const Statement &statement );
	    int statementCount() const;
	    bool containsStatements( const Statement &statement ) const;

	    void closeIterator( int id ) const;

	    Client* client() const { return m_client; }

	private:
	    int m_modelId;
	    mutable QList<int> m_openIterators;
	    Client* m_client;
	};
    }
}

#endif
