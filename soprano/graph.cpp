/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#include "graph.h"
#include "statement.h"
#include "node.h"
#include "statementiterator.h"
#include "iteratorbackend.h"
#include "nodeiterator.h"
#include "util/simplenodeiterator.h"

#include <QtCore/QSharedData>
#include <QtCore/QSet>



namespace Soprano {
}


class Soprano::Graph::Private : public QSharedData
{
public:
    QSet<Statement> statements;

    class GraphStatementIteratorBackend;
};


class Soprano::Graph::Private::GraphStatementIteratorBackend : public Soprano::IteratorBackend<Statement>
{
public:
    GraphStatementIteratorBackend( const Graph& g, const Statement& filter );
    ~GraphStatementIteratorBackend();

    bool next();

    Statement current() const;

    void close() {}

private:
    Graph m_graph;
    Statement m_filter;
    bool m_first;
    QSet<Statement>::const_iterator m_it;
};


Soprano::Graph::Private::GraphStatementIteratorBackend::GraphStatementIteratorBackend( const Graph& g, const Statement& filter )
    : m_graph( g ),
      m_filter( filter ),
      m_first( true ),
      m_it( g.d->statements.constBegin() )
{
}


Soprano::Graph::Private::GraphStatementIteratorBackend::~GraphStatementIteratorBackend()
{
}


bool Soprano::Graph::Private::GraphStatementIteratorBackend::next()
{
    if ( ( !m_first || !m_it->matches( m_filter ) ) &&
         m_it != m_graph.d->statements.constEnd() ) {
        do {
            ++m_it;
        } while( m_it != m_graph.d->statements.constEnd() &&
                 !m_it->matches( m_filter ) );
    }
    m_first = false;
    return m_it != m_graph.d->statements.constEnd();
}


Soprano::Statement Soprano::Graph::Private::GraphStatementIteratorBackend::current() const
{
    if ( m_it != m_graph.d->statements.constEnd() )
        return *m_it;
    else
        return Statement();
}



Soprano::Graph::Graph()
    : d( new Private )
{
}


Soprano::Graph::Graph( const Graph& other )
{
    d = other.d;
}


Soprano::Graph::Graph( const QList<Statement>& s )
    : d( new Private )
{
    d->statements = QSet<Statement>::fromList( s );
}


Soprano::Graph::~Graph()
{
}


void Soprano::Graph::addStatement( const Statement& statement )
{
    d->statements << statement;
}


void Soprano::Graph::addStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    addStatement( Statement( subject, predicate, object, context ) );
}


void Soprano::Graph::addStatements( const QList<Statement>& statements )
{
    d->statements += QSet<Statement>::fromList( statements );
}


void Soprano::Graph::removeStatement( const Statement& statement )
{
    d->statements.remove( statement );
}


void Soprano::Graph::removeStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    removeStatement( Statement( subject, predicate, object, context ) );
}


void Soprano::Graph::removeAllStatements( const Statement& statement )
{
    QSet<Statement>::iterator it = d->statements.begin();
    while ( it != d->statements.end() ) {
        if ( it->matches( statement ) )
            it = d->statements.erase( it );
        else
            ++it;
    }
}


void Soprano::Graph::removeAllStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    removeAllStatements( Statement( subject, predicate, object, context ) );
}


void Soprano::Graph::removeStatements( const QList<Statement>& statements )
{
    d->statements -= QSet<Statement>::fromList( statements );
}


void Soprano::Graph::removeContext( const Node& c )
{
    removeAllStatements( Statement( Node(), Node(), Node(), c ) );
}


Soprano::StatementIterator Soprano::Graph::listStatements( const Statement& partial ) const
{
    return new Private::GraphStatementIteratorBackend( *this, partial );
}


Soprano::StatementIterator Soprano::Graph::listStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return listStatements( Statement( subject, predicate, object, context ) );
}


Soprano::StatementIterator Soprano::Graph::listStatementsInContext( const Node& context ) const
{
    return listStatements( Statement( Node(), Node(), Node(), context ) );
}


Soprano::NodeIterator Soprano::Graph::listContexts() const
{
    QSet<Node> contexts;
    QSet<Statement>::const_iterator end = d->statements.constEnd();
    for ( QSet<Statement>::const_iterator it = d->statements.constBegin();
          it != end; ++it ) {
        if ( !it->context().isEmpty() )
            contexts << it->context();
    }
    return Util::SimpleNodeIterator( contexts.toList() );
}


bool Soprano::Graph::containsAnyStatement( const Statement& statement ) const
{
    QSet<Statement>::const_iterator end = d->statements.constEnd();
    for ( QSet<Statement>::const_iterator it = d->statements.constBegin();
          it != end; ++it ) {
        if ( it->matches( statement ) )
            return true;
    }
    return false;
}


bool Soprano::Graph::containsAnyStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return containsAnyStatement( Statement( subject, predicate, object, context ) );
}


bool Soprano::Graph::containsStatement( const Statement& statement ) const
{
    return d->statements.contains( statement );
}


bool Soprano::Graph::containsStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return containsStatement( Statement( subject, predicate, object, context ) );
}


bool Soprano::Graph::containsContext( const Node& context ) const
{
    return containsStatement( Statement( Node(), Node(), Node(), context ) );
}


bool Soprano::Graph::isEmpty() const
{
    return d->statements.isEmpty();
}


int Soprano::Graph::statementCount() const
{
    return d->statements.count();
}


QList<Soprano::Statement> Soprano::Graph::toList() const
{
    return d->statements.toList();
}


Soprano::Graph& Soprano::Graph::operator=( const Graph& other )
{
    d = other.d;
    return *this;
}


Soprano::Graph& Soprano::Graph::operator=( const QList<Statement>& s )
{
    d->statements = QSet<Statement>::fromList( s );
    return *this;
}


Soprano::Graph Soprano::Graph::operator+( const Graph& g ) const
{
    return Graph( *this ) += g;
}


Soprano::Graph Soprano::Graph::operator+( const Statement& s ) const
{
    return Graph( *this ) += s;
}


Soprano::Graph& Soprano::Graph::operator+=( const Graph& g )
{
    d->statements += g.d->statements;
    return *this;
}


Soprano::Graph& Soprano::Graph::operator+=( const Statement& s )
{
    addStatement( s );
    return *this;
}


Soprano::Graph Soprano::Graph::operator-( const Graph& g ) const
{
    return Graph( *this ) -= g;
}


Soprano::Graph Soprano::Graph::operator-( const Statement& s ) const
{
    return Graph( *this ) -= s;
}


Soprano::Graph& Soprano::Graph::operator-=( const Graph& g )
{
    d->statements -= g.d->statements;
    return *this;
}


Soprano::Graph& Soprano::Graph::operator-=( const Statement& s )
{
    removeStatement( s );
    return *this;
}


Soprano::Graph& Soprano::Graph::operator<<( const Graph& g )
{
    return operator+=( g );
}


Soprano::Graph& Soprano::Graph::operator<<( const Statement& s )
{
    return operator+=( s );
}


bool Soprano::Graph::operator==( const Graph& g ) const
{
    return d->statements == g.d->statements;
}


bool Soprano::Graph::operator!=( const Graph& g ) const
{
    return d->statements != g.d->statements;
}
