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

Soprano::Util::Command::Command( AsyncResult* result, Model* model, Type type )
    : m_result( result ),
      m_model( model ),
      m_type( type )
{
}


Soprano::Util::Command::~Command()
{
}



Soprano::Util::StatementCountCommand::StatementCountCommand( AsyncResult* result, Model* model )
    : Command( result, model, ReadCommand )
{
}


void Soprano::Util::StatementCountCommand::execute()
{
    int cnt = model()->statementCount();
    result()->setResult( cnt, model()->lastError() );
}


Soprano::Util::IsEmptyCommand::IsEmptyCommand( AsyncResult* result, Model* model )
    : Command( result, model, ReadCommand )
{
}


void Soprano::Util::IsEmptyCommand::execute()
{
    bool r = model()->isEmpty();
    result()->setResult( r, model()->lastError() );
}


Soprano::Util::StatementCommand::StatementCommand( AsyncResult* result, Model* model, const Statement& s, Type type )
    : Command( result, model, type )
{
    m_statements << s;
}


Soprano::Util::StatementCommand::StatementCommand( AsyncResult* result, Model* model, const QList<Statement>& s, Type type )
    : Command( result, model, type ),
      m_statements( s )
{
}


Soprano::Util::AddStatementCommand::AddStatementCommand( AsyncResult* result, Model* model, const QList<Statement>& statements )
    : StatementCommand( result, model, statements, WriteCommand )
{
}


void Soprano::Util::AddStatementCommand::execute()
{
    Error::ErrorCode r = model()->addStatements( statements() );
    result()->setResult( QVariant::fromValue( r ), model()->lastError() );
}


Soprano::Util::RemoveStatementCommand::RemoveStatementCommand( AsyncResult* result, Model* model, const QList<Statement>& statements )
    : StatementCommand( result, model, statements, WriteCommand )
{
}


void Soprano::Util::RemoveStatementCommand::execute()
{
    Error::ErrorCode r = model()->removeStatements( statements() );
    result()->setResult( QVariant::fromValue( r ), model()->lastError() );
}


Soprano::Util::RemoveAllStatementsCommand::RemoveAllStatementsCommand( AsyncResult* result, Model* model, const Statement& statement )
    : StatementCommand( result, model, statement, WriteCommand )
{
}


void Soprano::Util::RemoveAllStatementsCommand::execute()
{
    Error::ErrorCode r = model()->removeAllStatements( statement() );
    result()->setResult( QVariant::fromValue( r ), model()->lastError() );
}


Soprano::Util::ContainsStatementCommand::ContainsStatementCommand( AsyncResult* result, Model* model, const Statement& statement )
    : StatementCommand( result, model, statement, ReadCommand )
{
}


void Soprano::Util::ContainsStatementCommand::execute()
{
    bool r = model()->containsStatement( statement() );
    result()->setResult( r, model()->lastError() );
}


Soprano::Util::ContainsAnyStatementCommand::ContainsAnyStatementCommand( AsyncResult* result, Model* model, const Statement& statement )
    : StatementCommand( result, model, statement, ReadCommand )
{
}


void Soprano::Util::ContainsAnyStatementCommand::execute()
{
    bool r = model()->containsAnyStatement( statement() );
    result()->setResult( r, model()->lastError() );
}


Soprano::Util::ListStatementsCommand::ListStatementsCommand( AsyncModelPrivate* d, AsyncResult* result, Model* model, const Statement& statement )
    : StatementCommand( result, model, statement, ReadCommand ),
      m_asyncModelPrivate( d )
{
}


void Soprano::Util::ListStatementsCommand::execute()
{
    StatementIterator r = model()->listStatements( statement() );
    if ( r.isValid() ) {
        AsyncIteratorBackend<Statement>* b = new AsyncIteratorBackend<Statement>( m_asyncModelPrivate, r );
        result()->setResult( QVariant::fromValue( StatementIterator( b ) ), model()->lastError() );
        if ( m_asyncModelPrivate->mode == AsyncModel::MultiThreaded )
            b->iterate();
    }
    else {
        result()->setResult( QVariant::fromValue( r ), model()->lastError() );
    }
}


Soprano::Util::ListContextsCommand::ListContextsCommand( AsyncModelPrivate* d, AsyncResult* result, Model* model )
    : Command( result, model, ReadCommand ),
      m_asyncModelPrivate( d )
{
}


void Soprano::Util::ListContextsCommand::execute()
{
    NodeIterator r = model()->listContexts();
    if ( r.isValid() ) {
        AsyncIteratorBackend<Node>* b = new AsyncIteratorBackend<Node>( m_asyncModelPrivate, r );
        result()->setResult( QVariant::fromValue( NodeIterator( b ) ), model()->lastError() );
        if ( m_asyncModelPrivate->mode == AsyncModel::MultiThreaded )
            b->iterate();
    }
    else {
        result()->setResult( QVariant::fromValue( r ), model()->lastError() );
    }
}


Soprano::Util::ExecuteQueryCommand::ExecuteQueryCommand( AsyncModelPrivate* d,
                                                         AsyncResult* result,
                                                         Model* model,
                                                         const QString& query,
                                                         Query::QueryLanguage lang,
                                                         const QString& userQueryLang )
    : Command( result, model, ReadCommand ),
      m_query( query ),
      m_queryLanguage( lang ),
      m_userQueryLanguage( userQueryLang ),
      m_asyncModelPrivate( d )
{
}

void Soprano::Util::ExecuteQueryCommand::execute()
{
    QueryResultIterator r = model()->executeQuery( m_query, m_queryLanguage, m_userQueryLanguage );
    if ( r.isValid() ) {
        AsyncQueryResultIteratorBackend* b = new AsyncQueryResultIteratorBackend( m_asyncModelPrivate, r );
        if ( m_asyncModelPrivate->mode == AsyncModel::MultiThreaded )
            b->initWorkThread();
        result()->setResult( QVariant::fromValue( QueryResultIterator( b ) ), model()->lastError() );
        if ( m_asyncModelPrivate->mode == AsyncModel::MultiThreaded )
            b->iterate();
    }
    else {
        result()->setResult( QVariant::fromValue( r ), model()->lastError() );
    }
}


Soprano::Util::CreateBlankNodeCommand::CreateBlankNodeCommand( AsyncResult* result, Model* model )
    : Command( result, model, WriteCommand )
{
}

void Soprano::Util::CreateBlankNodeCommand::execute()
{
    Node r = model()->createBlankNode();
    result()->setResult( QVariant::fromValue( r ), model()->lastError() );
}
