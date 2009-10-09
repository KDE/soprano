/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008-2009 Sebastian Trueg <trueg@kde.org>
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
            query += "graph ";
            if ( s.context().isValid() ) {
                query += nodeToN3( s.context() );
            }
            else {
                query += "?g";
            }
            query += " { ";
        }

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
            // The fake types do not work anymore in 5.0.12!
#if 1
            if ( s.object().literal().isBool() )
                query += Soprano::Node( Soprano::LiteralValue::fromString( s.object().literal().toBool() ? QString( QLatin1String( "true" ) ) : QLatin1String("false"),
                                                                           Soprano::Virtuoso::fakeBooleanType() ) ).toN3();
            else if ( s.object().literal().isByteArray() )
                query += Soprano::Node( Soprano::LiteralValue::fromString( s.object().literal().toString(),
                                                                           Soprano::Virtuoso::fakeBase64BinaryType() ) ).toN3();
            else
#endif
                query += nodeToN3( s.object() );
        }
        else {
            query += "?o";
        }

        if ( withContext ) {
            query += " . }";
        }

        return query;
    }

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
    foreach( Virtuoso::QueryResultIteratorBackend* it, d->m_openIterators )
        it->close();
    delete d->connectionPool;
    delete d;
}


Soprano::Error::ErrorCode Soprano::VirtuosoModel::addStatement( const Statement& statement )
{
//    qDebug() << Q_FUNC_INFO << statement;

    if( !statement.isValid() ) {
        setError( "Cannot add invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    Statement s( statement );
    if ( !s.context().isValid() ) {
        s.setContext( Virtuoso::defaultGraph() );
    }

    QString insert = QString("sparql insert into %1")
                     .arg( statementToConstructGraphPattern( s, true ) );

    if ( statement.object().toN3().length() > 100000 ) {
        qDebug() << "Ignoring very long object for debugging purposes." << statement.subject() << statement.predicate() << statement.object().toN3().length();
        return Error::ErrorNone;
    }

//    qDebug() << "addStatement query:" << insert;
    ODBC::Connection* conn = d->connectionPool->connection();
    if ( conn->executeCommand( insert ) == Error::ErrorNone ) {
        clearError();

        // FIXME: can this be done with SQL/RDF views?
        emit statementAdded( statement );
        emit statementsAdded();

        return Error::ErrorNone;
    }
    else {
        setError( conn->lastError() );
        return Error::convertErrorCode( lastError().code() );
    }
}


Soprano::NodeIterator Soprano::VirtuosoModel::listContexts() const
{
//    qDebug() << Q_FUNC_INFO;

    return executeQuery( QString( "select distinct ?g where { "
                                  "graph ?g { ?s ?p ?o . } . "
                                  "FILTER(?g != %1 && ?g != %2) . }" )
                         .arg( Node::resourceToN3( Virtuoso::defaultGraph() ) )
                         .arg( Node::resourceToN3( Virtuoso::openlinkVirtualGraph() ) ) )
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
        query = QString( "ask from %1 where { %2 . }" ).arg( statement.context().toN3(), statementToConstructGraphPattern( statement, false ) );
    else
        query = QString( "ask where { %1 . }" ).arg( statementToConstructGraphPattern( statement, true ) );
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
        query = QString( "select * from %1 where { %2 . }" ).arg( partial.context().toN3(), statementToConstructGraphPattern( partial, false ) );
    else
        query = QString( "select * where { %1 . FILTER(?g != %2) . }" )
                .arg( statementToConstructGraphPattern( partial, true ) )
                .arg( Node::resourceToN3( Virtuoso::openlinkVirtualGraph() ) );
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

    QString query = QString( "delete from %1" )
                    .arg( statementToConstructGraphPattern( s, true ) );
//    qDebug() << "removeStatement query:" << query;
    ODBC::Connection* conn = d->connectionPool->connection();
    if ( conn->executeCommand( "sparql " + query ) == Error::ErrorNone ) {
        // FIXME: can this be done with SQL/RDF views?
        emit statementRemoved( statement );
        emit statementsRemoved();
    }
    setError( conn->lastError() );
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
            query = QString( "clear graph %1" ).arg( statement.context().toN3() );
        }
        else {
            query = QString( "delete from %1 { %2 } where { %3 }" )
                    .arg( statement.context().isValid() ? statement.context().toN3() : QString( "?g" ) )
                    .arg( statementToConstructGraphPattern( statement, false ) )
                    .arg( statementToConstructGraphPattern( statement, true ) );

        }
        qDebug() << "removeAllStatements query:" << query;
        ODBC::Connection* conn = d->connectionPool->connection();
        if ( conn->executeCommand( "sparql " + query ) == Error::ErrorNone ) {
            // FIXME: can this be done with SQL/RDF views?
            emit statementsRemoved();
            emit statementRemoved( statement );
        }
        setError( conn->lastError() );
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        // FIXME: do this in a fancy way, maybe an inner sql query or something
        QList<Node> allContexts = executeQuery( QString( "select distinct ?g where { %1 . FILTER(?g != %2) . }" )
                                                .arg( statementToConstructGraphPattern( statement, true ) )
                                                .arg( Node::resourceToN3( Virtuoso::openlinkVirtualGraph() ) ) )
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
            conditions << QString( "s=iri_to_id ('')" ).arg( QString::fromAscii( statement.subject().uri().toEncoded() ) );
        if ( statement.predicate().isValid() )
            conditions << QString( "p=iri_to_id ('')" ).arg( QString::fromAscii( statement.predicate().uri().toEncoded() ) );
        if ( statement.object().isValid() ) {
            if ( statement.object().isResource() ) {
                conditions << QString( "o=iri_to_id ('')" ).arg( QString::fromAscii( statement.object().uri().toEncoded() ) );
            }
            else {
                Q_ASSERT( 0 );
            }
        }

        QString query = QLatin1String( "delete from DB.DBA.RDF_QUAD where " ) + conditions.join( QLatin1String( " AND " ) ) + ';';

        ODBC::Connection* conn = d->connectionPool->connection();
        if ( ODBC::QueryResult* sh = conn->executeQuery( query ) ) {
            bool b = sh->fetchScroll();
            delete sh;
            return Error::ErrorUnknown;
        }
#endif

        return Error::ErrorNone;
    }
}


int Soprano::VirtuosoModel::statementCount() const
{
//    qDebug() << Q_FUNC_INFO;

    QueryResultIterator it = executeQuery( QString( "select count(*) where { "
                                                    "graph ?g { ?s ?p ?o . } . "
                                                    "FILTER(?g != %1) . }" )
                                           .arg( Node::resourceToN3( Virtuoso::openlinkVirtualGraph() ) ) );
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

    if ( language != Soprano::Query::QueryLanguageSparql ) {
        setError( Error::Error( QString( "Unsupported query language %1." )
                                .arg( Query::queryLanguageToString( language, userQueryLanguage ) ) ) );
        return QueryResultIterator();
    }

    // exclude the default system graph via defines from s_queryPrefix
    ODBC::Connection* conn = d->connectionPool->connection();
    ODBC::QueryResult* result = conn->executeQuery( QLatin1String( s_queryPrefix ) + ' ' + query );
    if ( result ) {
        clearError();
        Virtuoso::QueryResultIteratorBackend* backend = new Virtuoso::QueryResultIteratorBackend( result );
        backend->d->m_model = d;
        d->m_openIterators.append( backend );
        return backend;
    }
    else {
        qDebug() << "Query failed:" << query;
        setError( conn->lastError() );
        return 0;
    }
}

#include "virtuosomodel.moc"
