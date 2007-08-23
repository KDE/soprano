/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "queryresultiterator.h"
#include "queryresultiteratorbackend.h"
#include "node.h"
#include "statement.h"
#include "statementiterator.h"
#include "iteratorbackend.h"
#include "bindingset.h"


Soprano::QueryResultIterator::QueryResultIterator()
    : Iterator<BindingSet>()
{
}


Soprano::QueryResultIterator::QueryResultIterator( QueryResultIteratorBackend *qr )
    : Iterator<BindingSet>( qr )
{
}


Soprano::QueryResultIterator::QueryResultIterator( const QueryResultIterator& other )
    : Iterator<BindingSet>( other )
{
}


Soprano::QueryResultIterator::~QueryResultIterator()
{
}


Soprano::QueryResultIterator& Soprano::QueryResultIterator::operator=( const QueryResultIterator& other )
{
    Iterator<BindingSet>::operator=( other );
    return *this;
}


Soprano::Statement Soprano::QueryResultIterator::currentStatement() const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->currentStatement() : Statement() );
}


Soprano::BindingSet Soprano::QueryResultIterator::currentBindings() const
{
    return current();
}


QList<Soprano::BindingSet> Soprano::QueryResultIterator::allBindings()
{
    return allElements();
}


Soprano::Node Soprano::QueryResultIterator::binding( const QString &name ) const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->binding( name ) : Node() );
}


Soprano::Node Soprano::QueryResultIterator::binding( int offset ) const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->binding( offset ) : Node() );
}


int Soprano::QueryResultIterator::bindingCount() const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->bindingCount() : 0 );
}


QStringList Soprano::QueryResultIterator::bindingNames() const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->bindingNames() : QStringList() );
}


bool Soprano::QueryResultIterator::isGraph() const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->isGraph() : false );
}


bool Soprano::QueryResultIterator::isBinding() const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->isBinding() : false );
}


bool Soprano::QueryResultIterator::isBool() const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->isBool() : false );
}


bool Soprano::QueryResultIterator::boolValue() const
{
    return ( backend() ? dynamic_cast<QueryResultIteratorBackend*>( backend() )->boolValue() : false );
}


class QueryResultStatementIteratorBackend : public Soprano::IteratorBackend<Soprano::Statement>
{
public:
    QueryResultStatementIteratorBackend( const Soprano::QueryResultIterator& r )
        : m_result( r ) {
    }

    ~QueryResultStatementIteratorBackend() {
    }

    bool next() {
        return m_result.next();
    }

    Soprano::Statement current() const {
        return m_result.currentStatement();
    }

    void close() {
        m_result.close();
    }

private:
    Soprano::QueryResultIterator m_result;
};

Soprano::StatementIterator Soprano::QueryResultIterator::iterateStatements() const
{
    return new QueryResultStatementIteratorBackend( *this );
}
