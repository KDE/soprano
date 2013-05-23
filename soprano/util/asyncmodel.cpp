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

#include "asyncmodel.h"
#include "asyncmodel_p.h"
#include "asynccommand.h"
#include "asynciteratorbackend.h"
#include "asyncqueryresultiteratorbackend.h"

#include "statement.h"
#include "statementiterator.h"
#include "node.h"
#include "nodeiterator.h"
#include "queryresultiterator.h"

#include <QtCore/QTimer>
#include <QtCore/QThreadPool>


Q_DECLARE_METATYPE( Soprano::Statement )


Soprano::Util::AsyncModelPrivate::AsyncModelPrivate( AsyncModel* parent )
    : mode( AsyncModel::SingleThreaded ),
      m_model( parent )
{
}


Soprano::Util::AsyncModelPrivate::~AsyncModelPrivate()
{
    foreach( AsyncIteratorHandle* it, openIterators ) {
        it->setModelGone();
    }
}


// only called in SingleThreaded mode
void Soprano::Util::AsyncModelPrivate::addIterator( AsyncIteratorHandle* it )
{
    openIterators.append( it );
}


// only called in SingleThreaded mode
void Soprano::Util::AsyncModelPrivate::removeIterator( AsyncIteratorHandle* it )
{
    Q_ASSERT( openIterators.contains( it ) );
    openIterators.removeAll( it );
    QTimer::singleShot( 0, m_model, SLOT( _s_executeNextCommand() ) );
}


void Soprano::Util::AsyncModelPrivate::enqueueCommand( Command* command )
{
    if ( mode == AsyncModel::SingleThreaded ) {
        commandQueue.append( command );

        // it is important to not call _s_executeNextCommand directly to let
        // the client the time to connect to the resultReady signal
        QTimer::singleShot( 0, m_model, SLOT( _s_executeNextCommand() ) );
    }
    else {
        // the underlying model is thread-safe
        QThreadPool::globalInstance()->start( command );
    }
}


// only called in SingleThreaded mode
void Soprano::Util::AsyncModelPrivate::_s_executeNextCommand()
{
    for ( QLinkedList<Command*>::iterator it = commandQueue.begin();
          it != commandQueue.end(); ++it ) {
        Command* c = *it;
        if ( openIterators.isEmpty() ||
             c->type() == Command::ReadCommand ) {
            c->execute();
            commandQueue.erase( it );
            delete c;

            // let's see if there are more commands to be executed
            if ( !commandQueue.isEmpty() ) {
                QTimer::singleShot( 0, m_model, SLOT( _s_executeNextCommand() ) );
            }
            return;
        }
    }
}


Soprano::Util::AsyncModel::AsyncModel( Model* parent )
    : FilterModel( parent ),
      d( new AsyncModelPrivate( this ) )
{
    // Model signals are delivered queued in MultiThreaded mode
    qRegisterMetaType<Soprano::Statement>();
}


Soprano::Util::AsyncModel::~AsyncModel()
{
    // FIXME: finish all pending commands with a failure
    delete d;
}


void Soprano::Util::AsyncModel::setMode( AsyncModelMode mode )
{
    d->mode = mode;
}


Soprano::Util::AsyncModel::AsyncModelMode Soprano::Util::AsyncModel::mode() const
{
    return d->mode;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::addStatementAsync( const Statement& statement )
{
    return addStatementsAsync( QList<Statement>() << statement );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::addStatementAsync( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return addStatementAsync( Statement( subject, predicate, object, context ) );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::addStatementsAsync( const QList<Statement>& statements )
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new AddStatementCommand( result, this, statements ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::removeStatementAsync( const Statement& statement )
{
    return removeStatementsAsync( QList<Statement>() << statement );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::removeStatementAsync( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return removeStatementAsync( Statement( subject, predicate, object, context ) );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::removeStatementsAsync( const QList<Statement>& statements )
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new RemoveStatementCommand( result, this, statements ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::removeAllStatementsAsync( const Statement& statement )
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new RemoveAllStatementsCommand( result, this, statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::removeAllStatementsAsync( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return removeAllStatementsAsync( Statement( subject, predicate, object, context ) );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::isEmptyAsync() const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new IsEmptyCommand( result, const_cast<AsyncModel*>(this) ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::statementCountAsync() const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new StatementCountCommand( result, const_cast<AsyncModel*>(this) ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::listStatementsAsync( const Statement& statement ) const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ListStatementsCommand( d, result, const_cast<AsyncModel*>(this), statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::listStatementsAsync( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return listStatementsAsync( Statement( subject, predicate, object, context ) );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::listStatementsAsync() const
{
    return listStatementsAsync( Statement() );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::listContextsAsync() const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ListContextsCommand( d, result, const_cast<AsyncModel*>(this) ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::executeQueryAsync( const QString& query,
                                                                          Query::QueryLanguage language,
                                                                          const QString& userQueryLanguage ) const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ExecuteQueryCommand( d, result, const_cast<AsyncModel*>(this), query, language, userQueryLanguage ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::containsStatementAsync( const Statement& statement ) const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ContainsStatementCommand( result, const_cast<AsyncModel*>(this), statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::containsStatementAsync( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return containsStatementAsync( Statement( subject, predicate, object, context ) );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::containsAnyStatementAsync( const Statement& statement ) const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ContainsAnyStatementCommand( result, const_cast<AsyncModel*>(this), statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::containsAnyStatementAsync( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return containsAnyStatementAsync( Statement( subject, predicate, object, context ) );
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::createBlankNodeAsync()
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new CreateBlankNodeCommand( result, this ) );
    return result;
}


Soprano::StatementIterator Soprano::Util::AsyncModel::listStatements( const Statement& partial ) const
{
    return new SyncIteratorBackend<Statement>( d, FilterModel::listStatements( partial ) );
}


Soprano::NodeIterator Soprano::Util::AsyncModel::listContexts() const
{
    return new SyncIteratorBackend<Node>( d, FilterModel::listContexts() );
}


Soprano::QueryResultIterator Soprano::Util::AsyncModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    return new SyncQueryResultIteratorBackend( d, FilterModel::executeQuery( query, language, userQueryLanguage ) );
}

#include "moc_asyncmodel.cpp"

