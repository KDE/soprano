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

#include "queryresultstatementiterator.h"
#include "statementiteratorbackend.h"

#include "statement.h"

namespace Soprano {
    class QueryResultStatementIteratorBackend : public Soprano::StatementIteratorBackend
    {
    public:
	QueryResultStatementIteratorBackend( const QueryResultIterator& );
	~QueryResultStatementIteratorBackend();

	bool next();

	Soprano::Statement current() const;

    private:
	mutable QueryResultIterator m_result;
    };
}


Soprano::QueryResultStatementIteratorBackend::QueryResultStatementIteratorBackend( const QueryResultIterator& r )
    : Soprano::StatementIteratorBackend(),
      m_result( r )
{
}


Soprano::QueryResultStatementIteratorBackend::~QueryResultStatementIteratorBackend()
{
}


bool Soprano::QueryResultStatementIteratorBackend::next()
{
    return m_result.next();
}


Soprano::Statement Soprano::QueryResultStatementIteratorBackend::current() const
{
    return m_result.currentStatement();
}



class Soprano::QueryResultStatementIterator::Private
{
public:
};


Soprano::QueryResultStatementIterator::QueryResultStatementIterator()
    : d( new Private() )
{
}


Soprano::QueryResultStatementIterator::QueryResultStatementIterator( const QueryResultIterator& it )
    : StatementIterator( new QueryResultStatementIteratorBackend( it ) ),
      d ( new Private() )
{
}


Soprano::QueryResultStatementIterator::QueryResultStatementIterator( const QueryResultStatementIterator& it )
    : StatementIterator( it ),
      d( new Private() )
{
}


Soprano::QueryResultStatementIterator::~QueryResultStatementIterator()
{
    delete d;
}


Soprano::QueryResultStatementIterator& Soprano::QueryResultStatementIterator::operator=( const QueryResultIterator& r )
{
    setBackend( new QueryResultStatementIteratorBackend( r ) );
}


Soprano::QueryResultStatementIterator& Soprano::QueryResultStatementIterator::operator=( const QueryResultStatementIterator& it )
{
    StatementIterator::operator=( it );
}
