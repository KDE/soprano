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

#ifndef _QUERY_RESULT_STATEMENT_ITERATOR_H_
#define _QUERY_RESULT_STATEMENT_ITERATOR_H_

#include "statementiterator.h"
#include "queryresultiterator.h"

namespace Soprano {

    class Statement;

    /**
     * \brief A simple wrapper around QueryResultIterator that iterates over the results of a 
     * SPARQL describe or construct query.
     *
     * The QueryResultStatementIterator has been designed as a trivial extension
     * to StatementIterator and can be used as a drop-in-replacement as shown
     * below:
     * \code
     * QueryResultStatementIterator it( model->query( someGraphQuery ) );
     *
     * // it and it2 iterate over the exact same data
     * StatementIterator it2 = it;
     * \endcode
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class QueryResultStatementIterator : public Soprano::StatementIterator
    {
    public:
	QueryResultStatementIterator();
	QueryResultStatementIterator( const QueryResultIterator& );
	QueryResultStatementIterator( const QueryResultStatementIterator& );
	virtual ~QueryResultStatementIterator();

	QueryResultStatementIterator& operator=( const QueryResultIterator& );
	QueryResultStatementIterator& operator=( const QueryResultStatementIterator& );

    private:
	class Private;
	Private* const d;
    };
}

#endif
