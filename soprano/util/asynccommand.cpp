/*
 * This file is part of Soprano Project.
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

#include "asynccommand.h"
#include "asyncmodel_p.h"
#include "asynciteratorbackend.h"
#include "asyncqueryresultiteratorbackend.h"

#include "error.h"
#include "nodeiterator.h"
#include "statementiterator.h"
#include "queryresultiterator.h"


Q_DECLARE_METATYPE(Soprano::Error::ErrorCode)
Q_DECLARE_METATYPE(Soprano::Node)
Q_DECLARE_METATYPE(Soprano::StatementIterator)
Q_DECLARE_METATYPE(Soprano::NodeIterator)
Q_DECLARE_METATYPE(Soprano::QueryResultIterator)

Soprano::Util::Command::Command( Type type )
    : m_type( type )
{
}


Soprano::Util::Command::~Command()
{
}



Soprano::Util::StatementCountCommand::StatementCountCommand( AsyncResult* result )
    : Command( ReadCommand ),
      m_result( result )
{
}


void Soprano::Util::StatementCountCommand::execute( Model* model )
{
    int cnt = model->statementCount();
    m_result->setResult( cnt, model->lastError() );
}


Soprano::Util::IsEmptyCommand::IsEmptyCommand( AsyncResult* result )
    : Command( ReadCommand ),
      m_result( result )
{
}


void Soprano::Util::IsEmptyCommand::execute( Model* model )
{
    bool r = model->isEmpty();
    m_result->setResult( r, model->lastError() );
}


Soprano::Util::StatementCommand::StatementCommand( const Statement& s, Type type )
    : Command( type ),
      m_statement( s )
{
}


Soprano::Util::AddStatementCommand::AddStatementCommand( AsyncResult* result, const Statement& statement )
    : StatementCommand( statement, WriteCommand ),
      m_result( result )
{
}


void Soprano::Util::AddStatementCommand::execute( Model* model )
{
    Error::ErrorCode r = model->addStatement( statement() );
    m_result->setResult( QVariant::fromValue( r ), model->lastError() );
}


Soprano::Util::RemoveStatementCommand::RemoveStatementCommand( AsyncResult* result, const Statement& statement )
    : StatementCommand( statement, WriteCommand ),
      m_result( result )
{
}


void Soprano::Util::RemoveStatementCommand::execute( Model* model )
{
    Error::ErrorCode r = model->removeStatement( statement() );
    m_result->setResult( QVariant::fromValue( r ), model->lastError() );
}


Soprano::Util::RemoveAllStatementsCommand::RemoveAllStatementsCommand( AsyncResult* result, const Statement& statement )
    : StatementCommand( statement, WriteCommand ),
      m_result( result )
{
}


void Soprano::Util::RemoveAllStatementsCommand::execute( Model* model )
{
    Error::ErrorCode r = model->removeAllStatements( statement() );
    m_result->setResult( QVariant::fromValue( r ), model->lastError() );
}


Soprano::Util::ContainsStatementCommand::ContainsStatementCommand( AsyncResult* result, const Statement& statement )
    : StatementCommand( statement, ReadCommand ),
      m_result( result )
{
}


void Soprano::Util::ContainsStatementCommand::execute( Model* model )
{
    bool r = model->containsStatement( statement() );
    m_result->setResult( r, model->lastError() );
}


Soprano::Util::ContainsAnyStatementCommand::ContainsAnyStatementCommand( AsyncResult* result, const Statement& statement )
    : StatementCommand( statement, ReadCommand ),
      m_result( result )
{
}


void Soprano::Util::ContainsAnyStatementCommand::execute( Model* model )
{
    bool r = model->containsAnyStatement( statement() );
    m_result->setResult( r, model->lastError() );
}


Soprano::Util::ListStatementsCommand::ListStatementsCommand( AsyncModelPrivate* d, AsyncResult* result, const Statement& statement )
    : StatementCommand( statement, ReadCommand ),
      m_result( result ),
      m_asyncModelPrivate( d )
{
}


void Soprano::Util::ListStatementsCommand::execute( Model* model )
{
    StatementIterator r = model->listStatements( statement() );
    if ( r.isValid() ) {
        AsyncIteratorBackend<Statement>* b = new AsyncIteratorBackend<Statement>( m_asyncModelPrivate, r );
        m_result->setResult( QVariant::fromValue( StatementIterator( b ) ), model->lastError() );
    }
    else {
        m_result->setResult( QVariant::fromValue( r ), model->lastError() );
    }
}


Soprano::Util::ListContextsCommand::ListContextsCommand( AsyncModelPrivate* d, AsyncResult* result )
    : Command( ReadCommand ),
      m_result( result ),
      m_asyncModelPrivate( d )
{
}

void Soprano::Util::ListContextsCommand::execute( Model* model )
{
    NodeIterator r = model->listContexts();
    if ( r.isValid() ) {
        AsyncIteratorBackend<Node>* b = new AsyncIteratorBackend<Node>( m_asyncModelPrivate, r );
        m_result->setResult( QVariant::fromValue( NodeIterator( b ) ), model->lastError() );
    }
    else {
        m_result->setResult( QVariant::fromValue( r ), model->lastError() );
    }
}


Soprano::Util::ExecuteQueryCommand::ExecuteQueryCommand( AsyncModelPrivate* d,
                                                         AsyncResult* result,
                                                         const QString& query,
                                                         Query::QueryLanguage lang,
                                                         const QString& userQueryLang )
    : Command( ReadCommand ),
      m_result( result ),
      m_query( query ),
      m_queryLanguage( lang ),
      m_userQueryLanguage( userQueryLang ),
      m_asyncModelPrivate( d )
{
}

void Soprano::Util::ExecuteQueryCommand::execute( Model* model )
{
    QueryResultIterator r = model->executeQuery( m_query, m_queryLanguage, m_userQueryLanguage );
    if ( r.isValid() ) {
        AsyncQueryResultIteratorBackend* b = new AsyncQueryResultIteratorBackend( m_asyncModelPrivate, r );
        m_result->setResult( QVariant::fromValue( QueryResultIterator( b ) ), model->lastError() );
    }
    else {
        m_result->setResult( QVariant::fromValue( r ), model->lastError() );
    }
}


Soprano::Util::CreateBlankNodeCommand::CreateBlankNodeCommand( AsyncResult* result )
    : Command( WriteCommand ),
      m_result( result )
{
}

void Soprano::Util::CreateBlankNodeCommand::execute( Model* model )
{
    Node r = model->createBlankNode();
    m_result->setResult( QVariant::fromValue( r ), model->lastError() );
}
