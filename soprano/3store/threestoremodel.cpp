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

#include "threestoremodel.h"
#include "threestorequeryresult.h"
#include "threestoreresultiterator.h"

#include "resultset.h"
#include "statementiterator.h"
#include "query.h"

#include <QtCore/QDebug>


extern "C" {
#include <3store3/datatypes.h>
}


// FIXME: how about a method in Node that creates the SPARQL representation (or is called N3)?
static QString nodeToN3( const Soprano::Node& node )
{
    if ( node.isLiteral() ) {
        Soprano::LiteralValue v = node.literal();
        return QString( "\"%1\"^^%2" ).arg( v.toString() ).arg( v.dataTypeUri().toString() );
    }
    else {
        return QString( "<%1>" ).arg( node.uri().toString() );
    }
}


static QString statementToConstructGraphPattern( const Soprano::Statement& s )
{
    if ( s.subject().isValid() ||
         s.predicate().isValid() ||
         s.object().isValid() ) {

        QString query;

        if ( s.subject().isValid() ) {
            query += nodeToN3( s.subject() ) + ' ';
        }
        else {
            query += "?s ";
        }

        if ( s.predicate().isValid() ) {
            query += nodeToN3( s.predicate() ) + ' ';
        }
        else {
            query += "?p ";
        }

        if ( s.object().isValid() ) {
            query += nodeToN3( s.object() );
        }
        else {
            query += "?o";
        }
        return query;
    }
    else {
        return "?s ?p ?o";
    }
}


class Soprano::ThreeStore::Model::Private
{
public:
    ts_connection* connection;

    ts_hash ensureNode( const Node& node ) {
        if ( node.isLiteral() ) {
            LiteralValue v = node.literal();
            return ts_ensure_literal( connection, v.toString().toUtf8().data(),
                                      ts_ensure_datatype( connection, v.dataTypeUri().toEncoded().data() ),
                                      ts_ensure_language( connection, node.language().toUtf8().data() ) );
        }
        else {
            return ts_ensure_resource( connection, node.uri().toEncoded().data() );
        }
    }
};


Soprano::ThreeStore::Model::Model( ts_connection* conn )
    : Soprano::Model(),
      d( new Private() )
{
    d->connection = conn;
}


Soprano::ThreeStore::Model::~Model()
{
    ts_disconnect( d->connection );
    delete d;
}


Soprano::ErrorCode Soprano::ThreeStore::Model::add( const Statement &statement )
{
    return ( ts_assert_triple( d->connection,
                               d->ensureNode( statement.context() ),
                               statement.subject().toString().toUtf8(),
                               statement.predicate().toString().toUtf8(),
                               statement.object().toString().toUtf8(),
                               ( statement.object().isLiteral() ? ObjLiteral : ObjURI ),
                               ( statement.object().isLiteral() ? ts_ensure_datatype( d->connection, statement.object().literal().dataTypeUri().toString().toUtf8().data() ) : 0 ),
                               ( statement.object().isLiteral() ? ts_ensure_language( d->connection, statement.object().language().toUtf8().data() ) : 0 ) ) == 0
             ? ERROR_NONE : ERROR_UNKNOW );
}


Soprano::ErrorCode Soprano::ThreeStore::Model::add( const StatementIterator &iter )
{
    ErrorCode c( ERROR_NONE );

    ts_assert_block_mode( d->connection, TS_BLOCK_MODE_START );
    while ( iter.hasNext() ) {
        if ( ( c = add( iter.next() ) ) != ERROR_NONE ) {
            break;
        }
    }
    ts_assert_block_mode( d->connection, TS_BLOCK_MODE_COMMIT );

    return c;
}


Soprano::ErrorCode Soprano::ThreeStore::Model::add( const QList<Statement> &statements )
{
    ErrorCode c( ERROR_NONE );

    ts_assert_block_mode( d->connection, TS_BLOCK_MODE_START );
    for ( QList<Statement>::const_iterator it = statements.constBegin(); it != statements.constEnd(); ++it ) {
        if ( ( c = add( *it ) ) != ERROR_NONE ) {
            break;
        }
    }
    ts_assert_block_mode( d->connection, TS_BLOCK_MODE_COMMIT );

    return c;
}


QList<Soprano::Node> Soprano::ThreeStore::Model::contexts() const
{
    // FIXME: maybe use ts_mysql_query for this or use "select ?g where { graph ?g }" (although I dont think the latter is supported by rasqal)
    ResultSet r = executeQuery( Query( "select ?g where { graph ?g }", Query::SPARQL ) );
    QList<Soprano::Node> ng;
    while ( r.next() ) {
        ng.append( r.binding( 0 ) );
    }
    return ng;
}


bool Soprano::ThreeStore::Model::contains( const Statement &statement ) const
{
    // our listStatements does not support fully defined statements
    if ( statement.isValid() ) {
        // FIXME: I am not sure if this actually works. In redland it does not. But then again the redland ask query is completely buggy
//        return executeQuery( Query( QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( statement ) ), Query::SPARQL ) ).next();
        Statement dummy( statement.subject(), statement.predicate(), Node() );
        StatementIterator it = listStatements( dummy );
        while ( it.hasNext() ) {
            if ( it.next() == statement ) {
                return true;
            }
        }
        return false;
    }
    else {
        return Soprano::Model::contains( statement );
    }
}


Soprano::ResultSet Soprano::ThreeStore::Model::executeQuery( const Query &query ) const
{
    qDebug() << "(Soprano::ThreeStore::Model) executing query: " << query.query();

    // FIXME: should we use another base URI?
    ts_query* q = ts_query_prepare( d->connection,
                                    Query::queryTypeToString( query.type() ).toUtf8().data(),
                                    query.query().toUtf8().data(),
                                    "3store:default#");

    if ( ts_result* res = ts_query_execute( d->connection, q ) ) {
        return ResultSet( new ThreeStore::QueryResult( res ) );
    }
    else {
        return ResultSet( 0 );
    }
}


Soprano::StatementIterator Soprano::ThreeStore::Model::listStatements( const Statement &partial ) const
{
    QString query = QString( "construct { %1 } where { " ).arg( statementToConstructGraphPattern( partial ) );

    if ( partial.context().isValid() ) {
        query += QString( "graph <%1> { %2 } . }" ).arg( partial.context().uri().toString() ).arg( statementToConstructGraphPattern( partial ) );
    }
    else {
        query += QString( "%2 . }" ).arg( statementToConstructGraphPattern( partial ) );
    }

    ResultSet r = executeQuery( Query( query, Query::SPARQL ) );

    return Soprano::StatementIterator( new ThreeStore::ResultIterator( r ) );
}


Soprano::ErrorCode Soprano::ThreeStore::Model::remove( const Statement &statement )
{
    if ( statement.isValid() ) {
        return removeAll( statement );
    }
    else {
        return ERROR_INVALID_STATEMENT;
    }
}


Soprano::ErrorCode Soprano::ThreeStore::Model::remove( const Node &context )
{
    // FIXME: unsure if this works
    if ( context.isValid() ) {
        executeQuery( Query( QString( "delete graph %1" ).arg( context.uri().toString() ), Query::SPARQL ) );
        // FIXME: check if it succeeded
        return ERROR_NONE;
    }
    else {
        return ERROR_UNKNOW;
    }
}


Soprano::ErrorCode Soprano::ThreeStore::Model::removeAll( const Statement &statement )
{
    // FIXME: unsure if this works
    executeQuery( Query( QString( "delete where { %1 . }" ).arg( statementToConstructGraphPattern( statement ) ), Query::SPARQL ) );
    return ERROR_NONE;
}


int Soprano::ThreeStore::Model::size() const
{
    return -1;
}
