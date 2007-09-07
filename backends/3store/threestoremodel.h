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

#ifndef _SOPRANO_THREE_STORE_MODEL_H_
#define _SOPRANO_THREE_STORE_MODEL_H_

#include "storagemodel.h"
#include "node.h"
#include "statement.h"

extern "C" {
#include <3store3/tstore.h>
}


namespace Soprano {
    namespace ThreeStore {

	class Model : public Soprano::StorageModel
	{
	public:
	    Model( const Backend*, ts_connection* conn );
	    ~Model();

	    Error::ErrorCode addStatement( const Statement &statement );

	    NodeIterator listContexts() const;

	    bool containsAnyStatement( const Statement &statement ) const;

	    Soprano::QueryResultIterator executeQuery( const QueryLegacy &query ) const;

	    Soprano::StatementIterator listStatements( const Statement &partial ) const;

	    Error::ErrorCode removeStatement( const Statement &statement );

	    int statementCount() const;

	    Node createBlankNode();

	private:
	    class Private;
	    Private *d;
	};
    }
}

#endif
