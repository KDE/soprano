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


void Soprano::Util::AsyncModelPrivate::addIterator( AsyncIteratorBase* it )
{
    openIterators.append( it );
}


void Soprano::Util::AsyncModelPrivate::removeIterator( AsyncIteratorBase* it )
{
    Q_ASSERT( openIterators.contains( it ) );
    openIterators.removeAll( it );
    QTimer::singleShot( 0, m_model, SLOT( _s_executeNextCommand() ) );
}


void Soprano::Util::AsyncModelPrivate::enqueueCommand( Command* command )
{
    commandQueue.append( command );

    // it is important to not call _s_executeNextCommand directly to let
    // the client the time to connect to the resultReady signal
    QTimer::singleShot( 0, m_model, SLOT( _s_executeNextCommand() ) );
}


void Soprano::Util::AsyncModelPrivate::_s_executeNextCommand()
{
    for ( QLinkedList<Command*>::iterator it = commandQueue.begin();
          it != commandQueue.end(); ++it ) {
        Command* c = *it;
        if ( openIterators.isEmpty() ||
             c->type() == Command::ReadCommand ) {
            c->execute( m_model );
            commandQueue.erase( it );
            delete c;
            break;
        }
    }

    // let's see if there are more commands to be executed
    if ( !commandQueue.isEmpty()) {
        QTimer::singleShot( 0, m_model, SLOT( _s_executeNextCommand() ) );
    }
}


Soprano::Util::AsyncModel::AsyncModel( Model* parent )
    : FilterModel( parent ),
      d( new AsyncModelPrivate( this ) )
{
}


Soprano::Util::AsyncModel::~AsyncModel()
{
    // FIXME: finish all pending commands with a failure
    delete d;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::addStatementAsync( const Statement& statement )
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new AddStatementCommand( result, statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::removeStatementAsync( const Statement& statement )
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new RemoveStatementCommand( result, statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::removeAllStatementsAsync( const Statement& statement )
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new RemoveAllStatementsCommand( result, statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::isEmptyAsync() const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new IsEmptyCommand( result ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::statementCountAsync() const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new StatementCountCommand( result ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::listStatementsAsync( const Statement& statement ) const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ListStatementsCommand( d, result, statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::listContextsAsync() const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ListContextsCommand( d, result ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::executeQueryAsync( const QString& query,
                                                                          Query::QueryLanguage language,
                                                                          const QString& userQueryLanguage ) const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ExecuteQueryCommand( d, result, query, language, userQueryLanguage ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::containsStatementAsync( const Statement& statement ) const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ContainsStatementCommand( result, statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::containsAnyStatementAsync( const Statement& statement ) const
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new ContainsAnyStatementCommand( result, statement ) );
    return result;
}


Soprano::Util::AsyncResult* Soprano::Util::AsyncModel::createBlankNodeAsync()
{
    AsyncResult* result = new AsyncResult();
    d->enqueueCommand( new CreateBlankNodeCommand( result ) );
    return result;
}

#include "asyncmodel.moc"
