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
    QString prepareUrlForSqlCommand( const QUrl& url ) {
        return QString::fromAscii( url.toEncoded() ).replace( '\'', "\\\'" ).replace( '\"', "\\\"" );
    }

    QString nodeToN3( const Soprano::Node& node ) {
        if ( node.isResource() ) {
            QString encoded = node.toN3();
            encoded.replace( '\'', "%27" );
            return encoded;
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
            // HACK: fake datetime to prevent loosing fractions of seconds
            if ( s.object().literal().isDateTime() )
                query += Soprano::Node( Soprano::LiteralValue::fromString( s.object().literal().toString(), Soprano::IODBC::fakeDateTimeType() ) ).toN3();
            else if ( s.object().literal().isTime() )
                query += Soprano::Node( Soprano::LiteralValue::fromString( s.object().literal().toString(), Soprano::IODBC::fakeTimeType() ) ).toN3();
            else if ( s.object().literal().isBool() )
                query += Soprano::Node( Soprano::LiteralValue::fromString( s.object().literal().toBool() ? QString( QLatin1String( "1" ) ) : QString(),
                                                                           Soprano::IODBC::fakeBooleanType() ) ).toN3();
            else
                query += nodeToN3( s.object() );
        }
        else {
            query += "?o";
        }

        if ( withContext ) {
            query += " }";
        }

        return query;
    }

#if 0
    QString odbcFormatLiteral( const Soprano::LiteralValue& value ) {
        if ( value.isDate() ) {
            return QString( "{d '%1'}" ).arg( value.toString() );
        }

        else if ( value.isTime() ) {
            QString ts = value.toString();
            ts.chop( 1 ); // remove the timezone 'Z'
            return QString( "{t '%1'}" ).arg( ts );
        }

        else if ( value.isDateTime() ) {
            QString dts = value.toString();
            dts.replace( 'T', ' ' );
            dts.chop( 1 ); // remove the timezone 'Z'
            return QString( "{ts '%1'}" ).arg( dts );
        }

        else if ( value.isString() ) {
            return QString( "'%1'" ).arg( value.toString() );
        }

        else if ( value.isByteArray() ) {
            // FIXME
            Q_ASSERT( 0 );
            return QString();
        }

        else {
            // numbers
            return value.toString();
        }
    }
#endif
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


// TODO: is it faster to use DB.DBA.RDF_QUAD_URI, DB.DBA.RDF_QUAD_URI_L, and DB.DBA.RDF_QUAD_URI_L_TYPED?
//       (http://docs.openlinksw.com/virtuoso/rdfapiandsql.html)
//       (the code disabled by the ifdef)
Soprano::Error::ErrorCode Soprano::IODBCModel::addStatement( const Statement& statement )
{
    qDebug() << Q_FUNC_INFO << statement;

    if( !statement.isValid() ) {
        setError( "Cannot add invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    Statement s( statement );
    if ( !s.context().isValid() ) {
        s.setContext( IODBC::defaultGraph() );
    }

    QString insert;
#if 0
    // resource objects
    if ( s.object().isResource() ) {
        insert = QString( "DB.DBA.RDF_QUAD_URI('%1','%2','%3','%4')" )
                 .arg( prepareUrlForSqlCommand( s.context().uri() ) )
                 .arg( prepareUrlForSqlCommand( s.subject().uri() ) )
                 .arg( prepareUrlForSqlCommand( s.predicate().uri() ) )
                 .arg( prepareUrlForSqlCommand( s.object().uri() ) );
    }

    // literal objects
    else if ( s.object().isLiteral() ) {
        // strings with language or custom datatypes
        if ( !s.object().language().isEmpty() ||
             ( s.object().literal().isString() &&
               s.object().literal().dataTypeUri() != Soprano::Vocabulary::XMLSchema::string() ) ) {
            insert = QString( "DB.DBA.RDF_QUAD_URI_L_TYPED('%1','%2','%3','%4','%5','%6')" )
                     .arg( QString::fromAscii( s.context().uri().toEncoded() ) )
                     .arg( QString::fromAscii( s.subject().uri().toEncoded() ) )
                     .arg( QString::fromAscii( s.predicate().uri().toEncoded() ) )
                     .arg( s.object().toString() )
                     .arg( QString::fromAscii( s.object().literal().dataTypeUri().toEncoded() ) )
                     .arg( s.object().language() );
        }
        else if ( s.object().literal().isString() ) {
            // FIXME: properly format numbers and datetime
            insert = QString( "DB.DBA.RDF_QUAD_URI_L('%1','%2','%3','%4')" )
                     .arg( QString::fromAscii( s.context().uri().toEncoded() ) )
                     .arg( QString::fromAscii( s.subject().uri().toEncoded() ) )
                     .arg( QString::fromAscii( s.predicate().uri().toEncoded() ) )
                     .arg( s.object().toString() );
        }
    }
    else {
        setError( "No support for inserting blank nodes!", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }
#endif

    // FIXME: do this like above
    if ( insert.isEmpty() ) {
        insert = QString("sparql insert into %1")
                 .arg( statementToConstructGraphPattern( s, true ) );
    }
    qDebug() << "addStatement query:" << insert;
    if ( d->connection.executeCommand( insert ) == Error::ErrorNone ) {
        clearError();

        // FIXME: can this be done with SQL/RDF views?
        emit statementAdded( statement );
        emit statementsAdded();

        return Error::ErrorNone;
    }
    else {
        setError( d->connection.lastError() );
        return Error::convertErrorCode( lastError().code() );
    }
}


Soprano::NodeIterator Soprano::IODBCModel::listContexts() const
{
//    qDebug() << Q_FUNC_INFO;

    return executeQuery( QString( "select distinct ?g where { "
                                  "graph ?g { ?s ?p ?o . } . "
                                  "FILTER(?g != %1 && ?g != %2) . }" )
                         .arg( Node( IODBC::openlinkVirtualGraph() ).toN3() )
                         .arg( Node( IODBC::defaultGraph() ).toN3() ) )
        .iterateBindings( 0 );
}


bool Soprano::IODBCModel::containsStatement( const Statement& statement ) const
{
//    qDebug() << Q_FUNC_INFO << statement;

    // fail on invalid statements
    if ( !statement.isValid() ) {
        setError( "Cannot call containsStatement on invalid statements", Error::ErrorInvalidArgument );
        return false;
    }

    Statement s( statement );
    if ( !statement.context().isValid() )
        s.setContext( IODBC::defaultGraph() );
    QString query = QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( s, true ) );
//    qDebug() << "containsStatement query" << query;
    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
//     if ( IODBCStatementHandler* sh = d->connection.execute( "sparql " + query ) ) {
//         bool b = sh->fetchScroll();
//         delete sh;
//         return b;
//     }
//     return false;
}


bool Soprano::IODBCModel::containsAnyStatement( const Statement &statement ) const
{
//    qDebug() << Q_FUNC_INFO << statement;

    QString query = QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( statement, true ) );
//     if ( IODBCStatementHandler* sh = d->connection.execute( "sparql " + query ) ) {
//         bool b = sh->fetchScroll();
//         delete sh;
//         return b;
//     }
//     return false;
    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
}


Soprano::StatementIterator Soprano::IODBCModel::listStatements( const Statement& partial ) const
{
//    qDebug() << Q_FUNC_INFO << partial;

    // we cannot use a construct query due to missing graph support
    QString query = QString( "select * where { %1 ." ).arg( statementToConstructGraphPattern( partial, true ) );
    if ( !partial.context().isValid() )
        query += QString( " FILTER(?g != %1) }" ).arg( Node( IODBC::openlinkVirtualGraph() ).toN3() );
    else
        query += '}';
//    qDebug() << "List Statements Query" << query;
    return executeQuery( query, Query::QueryLanguageSparql )
        .iterateStatementsFromBindings( partial.subject().isValid() ? QString() : QString( 's' ),
                                        partial.predicate().isValid() ? QString() : QString( 'p' ),
                                        partial.object().isValid() ? QString() : QString( 'o' ),
                                        partial.context().isValid() ? QString() : QString( 'g' ),
                                        partial );
}


Soprano::Error::ErrorCode Soprano::IODBCModel::removeStatement( const Statement& statement )
{
//    qDebug() << Q_FUNC_INFO << statement;

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
//    qDebug() << "removeStatement query:" << query;
    if ( d->connection.executeCommand( "sparql " + query ) == Error::ErrorNone ) {
        // FIXME: can this be done with SQL/RDF views?
        emit statementRemoved( statement );
        emit statementsRemoved();
    }
    setError( d->connection.lastError() );
    return Error::convertErrorCode( lastError().code() );
}


Soprano::Error::ErrorCode Soprano::IODBCModel::removeAllStatements( const Statement& statement )
{
//    qDebug() << Q_FUNC_INFO << statement;

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
//        qDebug() << "removeAllStatements query:" << query;
        if ( d->connection.executeCommand( "sparql " + query ) == Error::ErrorNone ) {
            // FIXME: can this be done with SQL/RDF views?
            emit statementsRemoved();
            emit statementRemoved( statement );
        }
        setError( d->connection.lastError() );
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
//    qDebug() << Q_FUNC_INFO;

    QueryResultIterator it = executeQuery( QString( "select count(*) where { "
                                                    "graph ?g { ?s ?p ?o . } . "
                                                    "FILTER(?g != %1) . }" )
                                           .arg( Node( IODBC::openlinkVirtualGraph() ).toN3() ) );
    if ( it.isValid() && it.next() ) {
        return it.binding( 0 ).literal().toInt();
    }
    else {
        return -1;
    }
}


Soprano::Node Soprano::IODBCModel::createBlankNode()
{
    setError( "createBlankNode not supported by the Virtuoso backend", Error::ErrorNotSupported );
    return Node();
}


Soprano::QueryResultIterator Soprano::IODBCModel::executeQuery( const QString& query,
                                                                Query::QueryLanguage language,
                                                                const QString& userQueryLanguage ) const
{
//    qDebug() << Q_FUNC_INFO << query;

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
