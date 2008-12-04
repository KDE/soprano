/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "iodbcmodel.h"
#include "iodbcqueryiteratorbackend.h"
#include "iodbcstatementhandler.h"
#include "soprano.h"
#include "iodbcconnection.h"
#include "iodbctools.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <stdlib.h>


// FIXME: this (and some code below) is copied from SparqlModel.
namespace {
    QString statementToConstructGraphPattern( const Soprano::Statement& s, bool withContext = false )
    {
        QString query;

        if ( withContext ) {
            query += "graph ";
            if ( s.context().isValid() ) {
                query += s.context().toN3();
            }
            else {
                query += "?g";
            }
            query += " { ";
        }

        if ( s.subject().isValid() ) {
            query += s.subject().toN3() + ' ';
        }
        else {
            query += "?s ";
        }

        if ( s.predicate().isValid() ) {
            query += s.predicate().toN3() + ' ';
        }
        else {
            query += "?p ";
        }

        if ( s.object().isValid() ) {
            query += s.object().toN3();
        }
        else {
            query += "?o";
        }

        if ( withContext ) {
            query += " }";
        }

        return query;
    }
}


class Soprano::IODBCModel::Private
{
public:
    IODBCConnection connection;
};


Soprano::IODBCModel::IODBCModel( const Backend* b )
    : StorageModel(b),
      d( new Private() )
{
}


Soprano::IODBCModel::~IODBCModel()
{
    delete d;
}


bool Soprano::IODBCModel::connect( const QString& name )
{
    return d->connection.connect( name );
}


bool Soprano::IODBCModel::isConnected() const
{
    return d->connection.isConnected();
}


Soprano::Error::ErrorCode Soprano::IODBCModel::addStatement( const Statement& statement )
{
    if( !statement.isValid() ) {
        setError( "Cannot add invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    Statement s( statement );
    if ( !s.context().isValid() ) {
        s.setContext( IODBC::defaultGraph() );
    }

    QString insert = QString("insert into %1")
                     .arg( statementToConstructGraphPattern( s, true ) );
    qDebug() << "addStatement query:" << insert;
    QueryResultIterator it = executeQuery( insert );

    // on success a single binding is returned which contains a string (FIXME: is this always the case?)
    if ( it.next() ) {
        qDebug() << it.binding( 0 ).toString();
        return Error::ErrorNone;
    }
    else {
        return Error::convertErrorCode( lastError().code() );
    }
}


Soprano::NodeIterator Soprano::IODBCModel::listContexts() const
{
    return executeQuery( QString( "select distinct ?g where { "
                                  "graph ?g { ?s ?p ?o . } . "
                                  "FILTER(?g != %1 && ?g != %2) . }" )
                         .arg( Node( IODBC::openlinkVirtualGraph() ).toN3() )
                         .arg( Node( IODBC::defaultGraph() ).toN3() ) )
        .iterateBindings( 0 );
}


bool Soprano::IODBCModel::containsStatement( const Statement& statement ) const
{
    // fail on invalid statements
    if ( !statement.isValid() ) {
        setError( "Cannot call containsStatement on invalid statements", Error::ErrorInvalidArgument );
        return false;
    }

    Statement s( statement );
    if ( !statement.context().isValid() )
        s.setContext( IODBC::defaultGraph() );
    QString query = QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( s, true ) );
    qDebug() << "containsStatement query" << query;
    // FIXME: once the ask queries are fixed, use the code below:
//    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
    if ( IODBCStatementHandler* sh = d->connection.execute( "sparql " + query ) ) {
        bool b = sh->fetchScroll();
        delete sh;
        return b;
    }
    return false;
}


bool Soprano::IODBCModel::containsAnyStatement( const Statement &statement ) const
{
    QString query = QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( statement, true ) );
    // FIXME: once the ask queries are fixed, use the code below:
//    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
    if ( IODBCStatementHandler* sh = d->connection.execute( "sparql " + query ) ) {
        bool b = sh->fetchScroll();
        delete sh;
        return b;
    }
    return false;
}


Soprano::StatementIterator Soprano::IODBCModel::listStatements( const Statement& partial ) const
{
    // we cannot use a construct query due to missing graph support
    QString query = QString( "select * where { %1 ." ).arg( statementToConstructGraphPattern( partial, true ) );
    if ( !partial.context().isValid() )
        query += QString( " FILTER(?g != %1) }" ).arg( Node( IODBC::openlinkVirtualGraph() ).toN3() );
    else
        query += '}';
    qDebug() << "List Statements Query" << query;
    return executeQuery( query, Query::QueryLanguageSparql )
        .iterateStatementsFromBindings( partial.subject().isValid() ? QString() : QString( 's' ),
                                        partial.predicate().isValid() ? QString() : QString( 'p' ),
                                        partial.object().isValid() ? QString() : QString( 'o' ),
                                        partial.context().isValid() ? QString() : QString( 'g' ),
                                        partial );
}


Soprano::Error::ErrorCode Soprano::IODBCModel::removeStatement( const Statement& statement )
{
    if ( !statement.isValid() ) {
        setError( "Cannot remove invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    Statement s( statement );
    if ( !s.context().isValid() ) {
        s.setContext( IODBC::defaultGraph() );
    }
    else if ( s.context().uri() == IODBC::openlinkVirtualGraph() ) {
        setError( "Cannot remove statements from the virtual openlink graph. Virtuoso would not like that.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    QString query = QString( "delete from %1" )
                    .arg( statementToConstructGraphPattern( s, true ) );
    qDebug() << "removeStatement query:" << query;
    executeQuery( query );

    return Error::convertErrorCode( lastError().code() );
}


Soprano::Error::ErrorCode Soprano::IODBCModel::removeAllStatements( const Statement& statement )
{
    if ( statement.context().isValid() ) {
        if ( statement.context().uri() == IODBC::openlinkVirtualGraph() ) {
            setError( "Cannot remove statements from the virtual openlink graph. Virtuoso would not like that.", Error::ErrorInvalidArgument );
            return Error::ErrorInvalidArgument;
        }

        QString query;
        if ( !statement.subject().isValid() &&
             !statement.predicate().isValid() &&
             !statement.object().isValid() ) {
            // Virtuoso docu says this might be faster
            query = QString( "clear graph %1" ).arg( statement.context().toN3() );
        }
        else {
            query = QString( "delete from %1 { %2 } where { %3 }" )
                    .arg( statement.context().toN3() )
                    .arg( statementToConstructGraphPattern( statement, false ) )
                    .arg( statementToConstructGraphPattern( statement, true ) );

        }
        qDebug() << "removeAllStatements query:" << query;
        executeQuery( query );
        return Error::convertErrorCode( lastError().code() );
    }
    else {
//         if ( IODBCStatementHandler* sh = d->connection.execute( "delete from RDF_QUAD wheresparql " + query ) ) {
//             bool b = sh->fetchScroll();
//             delete sh;
//             return b;
//         }
        // FIXME: do this in a fancy way, maybe an inner sql query or something
        QList<Node> allContexts = listContexts().allNodes();
        allContexts << Node( IODBC::defaultGraph() );
        foreach( const Node& node, allContexts ) {
            Statement s( statement );
            s.setContext( node );
            Error::ErrorCode c = removeAllStatements( s );
            if ( c != Error::ErrorNone )
                return c;
        }
        return Error::ErrorNone;
    }
}


int Soprano::IODBCModel::statementCount() const
{
    QueryResultIterator it = executeQuery( "select count(*) where { graph ?g { ?s ?p ?o . } }" );
    if ( it.next() ) {
        return it.binding( 0 ).literal().toInt();
    }
    else {
        return -1;
    }
}


Soprano::Node Soprano::IODBCModel::createBlankNode()
{
#warning IODBCModel::createBlankNode not implemented
    setError( "createBlankNode not supported by the Virtuoso backend", Error::ErrorNotSupported );
    return Node();
}


Soprano::QueryResultIterator Soprano::IODBCModel::executeQuery( const QString& query,
                                                                Query::QueryLanguage language,
                                                                const QString& userQueryLanguage ) const
{
    if ( language != Soprano::Query::QueryLanguageSparql ) {
        setError( Error::Error( QString( "Unsupported query language %1." )
                                .arg( Query::queryLanguageToString( language, userQueryLanguage ) ) ) );
        return QueryResultIterator();
    }

    IODBCStatementHandler* hdl = d->connection.execute( "sparql " + query );
    if ( hdl ) {
        clearError();
        return new IODBCQueryResultIteratorBackend( hdl );
    }
    else {
        setError( d->connection.lastError() );
        return 0;
    }
}

#include "iodbcmodel.moc"
