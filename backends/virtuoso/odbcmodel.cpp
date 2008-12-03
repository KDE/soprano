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

#include "odbcmodel.h"
#include "iodbcqueryiteratorbackend.h"
#include "iodbcstatementhandler.h"
#include "soprano.h"
#include "iodbcconnection.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <stdlib.h>


// FIXME: this (and some code below) is copied from SparqlModel.
namespace {
    QString statementToConstructGraphPattern( const Soprano::Statement& s, bool withContext = false )
    {
        QString query;

        if ( withContext ) {
            query += "graph { ";
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


class Soprano::OdbcModel::Private
{
public:
    IODBCConnection connection;
};


Soprano::OdbcModel::OdbcModel()
    : StorageModel(0),
      d( new Private() )
{
}


Soprano::OdbcModel::~OdbcModel()
{
    delete d;
}


bool Soprano::OdbcModel::connect( const QString& name )
{
    return d->connection.connect( name );
}


bool Soprano::OdbcModel::isConnected() const
{
    return d->connection.isConnected();
}


Soprano::Error::ErrorCode Soprano::OdbcModel::addStatement( const Statement &statement )
{
    if( !statement.isValid() ) {
        setError( "Cannot add invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    // FIXME: what if the context is not defined?
    if ( !statement.context().isValid() ) {
        setError( "No support for the default context.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    QString insert = QString("insert into %1")
                     .arg( statementToConstructGraphPattern( statement, true ) );
    QueryResultIterator it = executeQuery( insert );

    // on success a single binding is returned which contains a string (FIXME: is this always the case?)
    if ( it.next() ) {
        return Error::ErrorNone;
    }
    else {
        return Error::convertErrorCode( lastError().code() );
    }
}


Soprano::NodeIterator Soprano::OdbcModel::listContexts() const
{
    return executeQuery( "select distinct ?c where { graph ?c { ?r ?p ?o . } }", Query::QueryLanguageSparql ).iterateBindings( 0 );
}


bool Soprano::OdbcModel::containsStatement( const Statement& statement ) const
{
    // fail on invalid statements
    if ( !statement.isValid() ) {
        setError( "Cannot call containsStatement on invalid statements", Error::ErrorInvalidArgument );
        return false;
    }

    QString query;
    if ( statement.context().isValid() ) {
        query = QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( statement, true ) );
    }
    else {
        // an empty context means the default context, i.e. no context set
        query = QString( "ask { %1 . OPTIONAL { %2 } . FILTER(!bound(?g)) . }" )
                .arg( statementToConstructGraphPattern( statement, false ) )
                .arg( statementToConstructGraphPattern( statement, true ) );
    }
    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
}


bool Soprano::OdbcModel::containsAnyStatement( const Statement &statement ) const
{
    QString query = QString( "ask { %1 }" ).arg( statementToConstructGraphPattern( statement, true ) );
    return executeQuery( query, Query::QueryLanguageSparql ).boolValue();
}


Soprano::StatementIterator Soprano::OdbcModel::listStatements( const Statement& partial ) const
{
    // we cannot use a construct query due to missing graph support
    QString query = QString( "select * where { %1 }" ).arg( statementToConstructGraphPattern( partial, true ) );
    qDebug() << "List Statements Query" << query;
    return executeQuery( query, Query::QueryLanguageSparql )
        .iterateStatementsFromBindings( partial.subject().isValid() ? QString() : QString( 's' ),
                                        partial.predicate().isValid() ? QString() : QString( 'p' ),
                                        partial.object().isValid() ? QString() : QString( 'o' ),
                                        partial.context().isValid() ? QString() : QString( 'g' ),
                                        partial );
}


Soprano::Error::ErrorCode Soprano::OdbcModel::removeStatement( const Statement& statement )
{
    if ( !statement.isValid() ) {
        setError( "Cannot remove invalid statement.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    if ( !statement.context().isValid() ) {
        setError( "No support for the default context.", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    executeQuery( QString( "delete from %1" )
                  .arg( statementToConstructGraphPattern( statement, true ) ) );

    return Error::convertErrorCode( lastError().code() );
}


Soprano::Error::ErrorCode Soprano::OdbcModel::removeAllStatements( const Statement& statement )
{
    if ( statement.context().isValid() &&
         !statement.subject().isValid() &&
         !statement.predicate().isValid() &&
         !statement.object().isValid() ) {
        // Virtuoso docu says this might be faster
        executeQuery( QString( "clear graph %1" ).arg( statement.context().toN3() ) );
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        executeQuery( QString( "delete from %1 { ?s ?p ?o } where { ?s ?p ?o . %2 }" )
                      .arg( statement.context().toN3() )
                      .arg( statementToConstructGraphPattern( statement, statement.context().isValid() ) ) );

        return Error::convertErrorCode( lastError().code() );

    }
}


int Soprano::OdbcModel::statementCount() const
{
    QueryResultIterator it = executeQuery( "select count(*) where { graph ?g { ?s ?p ?o . } }" );
    if ( it.next() ) {
        return it.binding( 0 ).literal().toInt();
    }
    else {
        return -1;
    }
}


Soprano::Node Soprano::OdbcModel::createBlankNode()
{
}


Soprano::QueryResultIterator Soprano::OdbcModel::executeQuery( const QString& query,
                                                               Query::QueryLanguage language,
                                                               const QString& userQueryLanguage ) const
{
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

#include "odbcmodel.moc"
