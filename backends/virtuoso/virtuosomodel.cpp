/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008-2010 Sebastian Trueg <trueg@kde.org>
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

#include "virtuosomodel.h"
#include "virtuosomodel_p.h"
#include "virtuosoqueryresultiteratorbackend.h"
#include "virtuosoqueryresultiteratorbackend_p.h"
#include "virtuosobackend.h"
#include "soprano.h"
#include "odbcenvironment.h"
#include "odbcconnection.h"
#include "odbcconnectionpool.h"
#include "virtuosotools.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <stdlib.h>


namespace {
    QString nodeToN3( const Soprano::Node& node ) {
        if ( node.isBlank() ) {
            // looks like Virtuoso needs a special syntax here, at least that is what the redland bindings do.
            return '<' + node.toN3() + '>';
        }
        else {
            return node.toN3();
        }
    }

    QString statementToConstructGraphPattern( const Soprano::Statement& s, bool withContext = false )
    {
        QString query;

        if ( withContext ) {
            query += QLatin1String( "graph " );
            if ( s.context().isValid() ) {
                query += nodeToN3( s.context() );
            }
            else {
                query += QLatin1String( "?g" );
            }
            query += QLatin1String( " { " );
        }

        if ( s.subject().isValid() ) {
            query += nodeToN3( s.subject() ) + ' ';
        }
        else {
            query += QLatin1String( "?s " );
        }

        if ( s.predicate().isValid() ) {
            query += nodeToN3( s.predicate() ) + ' ';
        }
        else {
            query += QLatin1String( "?p " );
        }

        if ( s.object().isValid() ) {
            if ( s.object().literal().isBool() )
                query += Soprano::Node( Soprano::LiteralValue::fromString( s.object().literal().toBool() ? QString( QLatin1String( "true" ) ) : QLatin1String("false"),
                                                                           Soprano::Virtuoso::fakeBooleanType() ) ).toN3();
            else if ( s.object().literal().isByteArray() )
                query += Soprano::Node( Soprano::LiteralValue::fromString( s.object().literal().toString(),
                                                                           Soprano::Virtuoso::fakeBase64BinaryType() ) ).toN3();
            else
                query += nodeToN3( s.object() );
        }
        else {
            query += QLatin1String( "?o" );
        }

        if ( withContext ) {
            query += QLatin1String( " . }" );
        }

        return query;
    }

    // there is still a bug in Virtuoso which makes the define prefix unusable: if a query defines the
    // graph the result will be empty if the exclude graph is specified.
    const char* s_queryPrefix =
        "sparql ";
//         "define input:default-graph-exclude <http://www.openlinksw.com/schemas/virtrdf#> "
//         "define input:named-graph-exclude <http://www.openlinksw.com/schemas/virtrdf#>";
}


Soprano::VirtuosoModel::VirtuosoModel( ODBC::ConnectionPool* connectionPool, const Backend* b )
    : StorageModel(b),
      d( new VirtuosoModelPrivate() )
{
    d->connectionPool = connectionPool;
}


Soprano::VirtuosoModel::~VirtuosoModel()
{
    d->closeAllIterators();
    delete d->connectionPool;
    delete d;
}


//
// Patrick van Kleef <pkleef@openlinksw.com> says:
// You may  want to concider using the TTLP_MT stored procedure to load
// either large number of triples or large triples, since this is much more
// efficient. You can call this with:
//
//         TTLP_MT( ?, 'graphname', 'graphname', 255);
//
// and bind with a blob parameter that holds the triples.
//
Soprano::Error::ErrorCode Soprano::VirtuosoModel::addStatement( const Statement& statement )
{
//    qDebug() << Q_FUNC_INFO << statement;

    if( !statement.isValid() ) {
        qDebug() << Q_FUNC_INFO << "Cannot add invalid statement:" << statement;
        setError( "Cannot add invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    Statement s( statement );
    if ( !s.context().isValid() ) {
        s.setContext( Virtuoso::defaultGraph() );
    }

    QString insert = QString::fromLatin1("sparql insert into %1")
                     .arg( statementToConstructGraphPattern( s, true ) );

    if ( ODBC::Connection* conn = d->connectionPool->connection() ) {
        if ( conn->executeCommand( insert ) == Error::ErrorNone ) {
            clearError();

            // FIXME: can this be done with SQL/RDF views?
            emit statementAdded( statement );
            emit statementsAdded();

            return Error::ErrorNone;
        }
        else {
            setError( conn->lastError() );
        }
    }
    else {
        setError( d->connectionPool->lastError() );
    }
    return Error::convertErrorCode( lastError().code() );
}


// TODO: use "select GRAPH_IRI from DB.DBA.SPARQL_SELECT_KNOWN_GRAPHS_T"
Soprano::NodeIterator Soprano::VirtuosoModel::listContexts() const
{
//    qDebug() << Q_FUNC_INFO;

    return executeQuery( QString::fromLatin1( "select distinct ?g where { "
                                              "graph ?g { ?s ?p ?o . } . "
                                              "FILTER(?g != <%1> && ?g != <%2>) . }" )
                         .arg( QLatin1String( Virtuoso::defaultGraphString() ),
                               QLatin1String( Virtuoso::openlinkVirtualGraphString() ) ) )
        .iterateBindings( 0 );
}


bool Soprano::VirtuosoModel::containsStatement( const Statement& statement ) const
{
//    qDebug() << Q_FUNC_INFO << statement;

    // fail on invalid statements
    if ( !statement.isValid() ) {
        setError( "Cannot call containsStatement on invalid statements", Error::ErrorInvalidArgument );
        return false;
    }

    Statement s( statement );
    if ( !statement.context().isValid() )
        s.setContext( Virtuoso::defaultGraph() );
    return containsAnyStatement( s );
}


bool Soprano::VirtuosoModel::containsAnyStatement( const Statement& statement ) const
{
//    qDebug() << Q_FUNC_INFO << statement;

    QString query;
    if ( statement.context().isValid() )
        query = QString::fromLatin1( "ask from %1 where { %2 . }" ).arg( statement.context().toN3(), statementToConstructGraphPattern( statement, false ) );
    else
        query = QString::fromLatin1( "ask where { %1 . }" ).arg( statementToConstructGraphPattern( statement, true ) );
//     if ( VirtuosoStatementHandler* sh = d->connection.execute( "sparql " + query ) ) {
//         bool b = sh->fetchScroll();
//         delete sh;
//         return b;
//     }
//     return false;
    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
}


Soprano::StatementIterator Soprano::VirtuosoModel::listStatements( const Statement& partial ) const
{
//    qDebug() << Q_FUNC_INFO << partial;

    // we cannot use a construct query due to missing graph support
    QString query;
    if ( partial.context().isValid() )
        query = QString::fromLatin1( "select * from %1 where { %2 . }" )
                .arg( partial.context().toN3(), statementToConstructGraphPattern( partial, false ) );
    else
        query = QString::fromLatin1( "select * where { %1 . FILTER(?g != <%2>) . }" )
                .arg( statementToConstructGraphPattern( partial, true ),
                      QLatin1String( Virtuoso::openlinkVirtualGraphString() ) );
//    qDebug() << "List Statements Query" << query;
    return executeQuery( query, Query::QueryLanguageSparql )
        .iterateStatementsFromBindings( partial.subject().isValid() ? QString() : QString( 's' ),
                                        partial.predicate().isValid() ? QString() : QString( 'p' ),
                                        partial.object().isValid() ? QString() : QString( 'o' ),
                                        partial.context().isValid() ? QString() : QString( 'g' ),
                                        partial );
}


Soprano::Error::ErrorCode Soprano::VirtuosoModel::removeStatement( const Statement& statement )
{
//    qDebug() << Q_FUNC_INFO << statement;

    if ( !statement.isValid() ) {
        setError( "Cannot remove invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    Statement s( statement );
    if ( !s.context().isValid() ) {
        s.setContext( Virtuoso::defaultGraph() );
    }
    else if ( s.context().uri() == Virtuoso::openlinkVirtualGraph() ) {
        setError( "Cannot remove statements from the virtual openlink graph. Virtuoso would not like that.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    QString query = QString::fromLatin1( "delete from %1" )
                    .arg( statementToConstructGraphPattern( s, true ) );
//    qDebug() << "removeStatement query:" << query;
    if ( ODBC::Connection* conn = d->connectionPool->connection() ) {
        if ( conn->executeCommand( QLatin1String( "sparql " ) + query ) == Error::ErrorNone ) {
            // FIXME: can this be done with SQL/RDF views?
            emit statementRemoved( statement );
            emit statementsRemoved();
        }
        setError( conn->lastError() );
    }
    else {
        setError( d->connectionPool->lastError() );
    }
    return Error::convertErrorCode( lastError().code() );
}


Soprano::Error::ErrorCode Soprano::VirtuosoModel::removeAllStatements( const Statement& statement )
{
//    qDebug() << Q_FUNC_INFO << statement;

    if ( statement.context().isValid() ) {
        if ( statement.context().uri() == Virtuoso::openlinkVirtualGraph() ) {
            setError( "Cannot remove statements from the virtual openlink graph. Virtuoso would not like that.", Error::ErrorInvalidArgument );
            return Error::ErrorInvalidArgument;
        }

        QString query;
        if ( statement.context().isValid() &&
             !statement.subject().isValid() &&
             !statement.predicate().isValid() &&
             !statement.object().isValid() ) {
            // Virtuoso docu says this might be faster
            query = QString::fromLatin1( "clear graph %1" ).arg( statement.context().toN3() );
        }
        else {
            query = QString::fromLatin1( "delete from %1 { %2 } where { %3 }" )
                    .arg( statement.context().isValid() ? statement.context().toN3() : QString( "?g" ),
                          statementToConstructGraphPattern( statement, false ),
                          statementToConstructGraphPattern( statement, true ) );

        }
//        qDebug() << "removeAllStatements query:" << query;
        if ( ODBC::Connection* conn = d->connectionPool->connection() ) {
            if ( conn->executeCommand( "sparql " + query ) == Error::ErrorNone ) {
                // FIXME: can this be done with SQL/RDF views?
                emit statementsRemoved();
                emit statementRemoved( statement );
            }
            setError( conn->lastError() );
        }
        else {
            setError( d->connectionPool->lastError() );
        }
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        // FIXME: do this in a fancy way, maybe an inner sql query or something
        QList<Node> allContexts = executeQuery( QString::fromLatin1( "select distinct ?g where { %1 . FILTER(?g != <%2>) . }" )
                                                .arg( statementToConstructGraphPattern( statement, true ),
                                                      QLatin1String( Virtuoso::openlinkVirtualGraphString() ) ) )
                                  .iterateBindings( 0 ).allNodes();
        foreach( const Node& node, allContexts ) {
            Statement s( statement );
            if ( node.isValid() )
                s.setContext( node );
            else
                s.setContext( Virtuoso::defaultGraph() );
            Error::ErrorCode c = removeAllStatements( s );
            if ( c != Error::ErrorNone )
                return c;
        }

// the code below is not finished yet, still missing is a literal comparison. But then it should be much faster
#if 0
QStringList conditions;
        if ( statement.subject().isValid() )
            conditions << QString::fromLatin1( "s=iri_to_id ('')" ).arg( QString::fromAscii( statement.subject().uri().toEncoded() ) );
        if ( statement.predicate().isValid() )
            conditions << QString::fromLatin1( "p=iri_to_id ('')" ).arg( QString::fromAscii( statement.predicate().uri().toEncoded() ) );
        if ( statement.object().isValid() ) {
            if ( statement.object().isResource() ) {
                conditions << QString::fromLatin1( "o=iri_to_id ('')" ).arg( QString::fromAscii( statement.object().uri().toEncoded() ) );
            }
            else {
                Q_ASSERT( 0 );
            }
        }

        QString query = QLatin1String( "delete from DB.DBA.RDF_QUAD where " ) + conditions.join( QLatin1String( " AND " ) ) + ';';

        if ( ODBC::Connection* conn = d->connectionPool->connection() ) {
            if ( ODBC::QueryResult* sh = conn->executeQuery( query ) ) {
                bool b = sh->fetchScroll();
                delete sh;
                return Error::ErrorUnknown;
            }
            else {
                setError( conn->lastError() );
            }
        }
        else {
            setError( d->connectionPool->lastError() );
        }
#endif

        return Error::ErrorNone;
    }
}


int Soprano::VirtuosoModel::statementCount() const
{
//    qDebug() << Q_FUNC_INFO;

    QueryResultIterator it = executeQuery( QString::fromLatin1( "select count(*) where { "
                                                                "graph ?g { ?s ?p ?o . } . "
                                                                "FILTER(?g != <%1>) . }" )
                                           .arg( QLatin1String( Virtuoso::openlinkVirtualGraphString() ) ) );
    if ( it.isValid() && it.next() ) {
        return it.binding( 0 ).literal().toInt();
    }
    else {
        return -1;
    }
}


Soprano::Node Soprano::VirtuosoModel::createBlankNode()
{
    setError( "createBlankNode not supported by the Virtuoso backend", Error::ErrorNotSupported );
    return Node();
}


Soprano::QueryResultIterator Soprano::VirtuosoModel::executeQuery( const QString& query,
                                                                   Query::QueryLanguage language,
                                                                   const QString& userQueryLanguage ) const
{
//    qDebug() << Q_FUNC_INFO << query;

    QString finalQuery( query );

    if ( language != Soprano::Query::QueryLanguageSparql ) {
        setError( Error::Error( QString::fromLatin1( "Unsupported query language %1." )
                                .arg( Query::queryLanguageToString( language, userQueryLanguage ) ) ) );
        return QueryResultIterator();
    }

    finalQuery.prepend( QLatin1String( s_queryPrefix ) + ' ' );

//    qDebug() << Q_FUNC_INFO << finalQuery;

    if ( ODBC::Connection* conn = d->connectionPool->connection() ) {
        ODBC::QueryResult* result = conn->executeQuery( finalQuery );
        if ( result ) {
            clearError();
            Virtuoso::QueryResultIteratorBackend* backend = new Virtuoso::QueryResultIteratorBackend( d, result );
            return backend;
        }
        else {
            qDebug() << "Query failed:" << finalQuery;
            setError( conn->lastError() );
            return 0;
        }
    }
    else {
        setError( d->connectionPool->lastError() );
        return 0;
    }
}

#include "virtuosomodel.moc"
