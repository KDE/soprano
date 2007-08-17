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

#ifndef _SESAME2_MODEL_H_
#define _SESAME2_MODEL_H_

#include <soprano/storagemodel.h>

#include <jni.h>

namespace Soprano {
    namespace Sesame2 {

	class RepositoryWrapper;

	class Model : public StorageModel
	{
	public:
	    Model( const Backend* backend, RepositoryWrapper* sesame2Repository );
	    ~Model();

	    ErrorCode addStatement( const Statement &statement );
	    NodeIterator listContexts() const;
	    QueryResultIterator executeQuery( const QueryLegacy &query ) const;
	    StatementIterator listStatements( const Statement &partial ) const;
	    ErrorCode removeStatements( const Statement &statement );
	    int statementCount() const;
	    bool containsStatements( const Statement &statement ) const;

	private:
	    RepositoryWrapper* m_repository;
	};
    }
}

#endif
