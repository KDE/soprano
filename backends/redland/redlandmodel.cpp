/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

#include "redlandmodel.h"

#include "queryresultiterator.h"
#include "statementiterator.h"
#include "nodeiterator.h"

#include "redlandworld.h"
#include "redlandqueryresult.h"
#include "redlandstatementiterator.h"
#include "redlandnodeiteratorbackend.h"
#include "multimutex.h"

#include <QtCore/QDebug>


namespace {
    bool isContextOnlyStatement( const Soprano::Statement& statement )
    {
        return ( !statement.subject().isValid() &&
                 !statement.predicate().isValid() &&
                 !statement.object().isValid() &&
                 statement.context().isValid() );
    }

    bool isRedlandStatementEmpty( librdf_statement* statement )
    {
        return ( !statement ||
                 ( !librdf_statement_get_subject( statement ) &&
                   !librdf_statement_get_predicate( statement ) &&
                   !librdf_statement_get_object( statement ) ) );
    }
}


class Soprano::Redland::RedlandModel::Private
{
public:
    Private() :
        world(0),
        model(0),
        storage(0)
    {}

    World *world;
    librdf_model *model;
    librdf_storage *storage;

    MultiMutex readWriteLock; // restricts multiple reads to one thread

    QList<RedlandStatementIterator*> iterators;
    QList<Redland::NodeIteratorBackend*> nodeIterators;
    QList<RedlandQueryResult*> results;

    /**
     * librdf_model_find_statements_in_context does not support empty contexts. All in all
     * redland is not very flexible. Thus, we have to do it all manually.
     */
    librdf_stream* redlandFindStatements( const Soprano::Statement& statement );
    librdf_stream* redlandFindStatements( librdf_statement* statement, librdf_node* context );
    bool redlandContainsStatement( const Soprano::Statement& statement );
    bool redlandContainsStatement( librdf_statement* statement, librdf_node* context );
};


librdf_stream* Soprano::Redland::RedlandModel::Private::redlandFindStatements( const Soprano::Statement& statement )
{
    librdf_node* ctx = world->createNode( statement.context() );
    librdf_statement* st = world->createStatement( statement );
    librdf_stream *stream = redlandFindStatements( st, ctx );
    world->freeNode( ctx );
    world->freeStatement( st );
    return stream;
}


librdf_stream* Soprano::Redland::RedlandModel::Private::redlandFindStatements( librdf_statement* statement, librdf_node* context )
{
    if ( isRedlandStatementEmpty( statement ) && context ) {
        return librdf_model_context_as_stream( model, context );
    }
    else {
        // context support does not work, redland API claims that librdf_model_find_statements_in_context
        // with a NULL context is the same as librdf_model_find_statements. Well, in practice it is not.
        // We even have to set the context on all statements if we only search in one context!
        librdf_stream *stream = 0;
        if( !context ) {
            stream = librdf_model_find_statements( model, statement );
        }
        else {
            stream = librdf_model_find_statements_in_context( model, statement, context );
        }
        return stream;
    }

    return 0;
}


bool Soprano::Redland::RedlandModel::Private::redlandContainsStatement( const Soprano::Statement& statement )
{
    librdf_statement* s = world->createStatement( statement );
    librdf_node* c = statement.context().isValid() ? world->createNode( statement.context() ) : 0;
    bool cr = redlandContainsStatement( s, c );
    world->freeStatement( s );
    world->freeNode( c );
    return cr;
}


bool Soprano::Redland::RedlandModel::Private::redlandContainsStatement( librdf_statement* statement, librdf_node* context )
{
    if ( isRedlandStatementEmpty( statement ) && context ) {
        return librdf_model_contains_context( model, context ) != 0;
    }
    else {
        // librdf_model_contains_statement does not support
        // empty nodes and also there is no context support for contains.
        librdf_stream* s = redlandFindStatements( statement, context );
        if ( s ) {
            bool c = !librdf_stream_end( s );
            librdf_free_stream( s );
            return c;
        }
        else {
            return false;
        }
    }
}


Soprano::Redland::RedlandModel::RedlandModel( const Backend* b, librdf_model *model, librdf_storage *storage, World* world )
    : StorageModel( b )
{
    d = new Private;
    d->world = world;
    d->model = model;
    d->storage = storage;

    Q_ASSERT( model != 0L );
    Q_ASSERT( storage != 0L );
}

Soprano::Redland::RedlandModel::~RedlandModel()
{
    for ( QList<RedlandStatementIterator*>::iterator it = d->iterators.begin();
          it != d->iterators.end(); ++it ) {
        ( *it )->close();
    }
    for ( QList<Redland::NodeIteratorBackend*>::iterator it = d->nodeIterators.begin();
          it != d->nodeIterators.end(); ++it ) {
        ( *it )->close();
    }
    for ( QList<RedlandQueryResult*>::iterator it = d->results.begin();
          it != d->results.end(); ++it ) {
        ( *it )->close();
    }

    librdf_free_model( d->model );
    librdf_free_storage( d->storage );

//    delete d->world;
    delete d;
}


Soprano::Redland::World* Soprano::Redland::RedlandModel::world() const
{
    return d->world;
}


librdf_model *Soprano::Redland::RedlandModel::redlandModel() const
{
    return d->model;
}


Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::addStatement( const Statement &statement )
{
    if ( !statement.isValid() ) {
        setError( "Cannot add invalid statement", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    clearError();

    d->readWriteLock.lockForWrite();

    librdf_statement* redlandStatement = d->world->createStatement( statement );
    if ( !redlandStatement ||
         !librdf_statement_get_subject( redlandStatement ) ||
         !librdf_statement_get_predicate( redlandStatement ) ||
         !librdf_statement_get_object( redlandStatement ) ) {
        setError( d->world->lastError( Error::Error( "Could not convert to redland statement",
                                                     Error::ErrorInvalidArgument ) ) );
        d->readWriteLock.unlock();
        return Error::ErrorInvalidArgument;
    }

    if ( statement.context().isEmpty() ) {
        if ( int r = librdf_model_add_statement( d->model, redlandStatement ) ) {
            d->world->freeStatement( redlandStatement );
            setError( d->world->lastError( Error::Error( QString( "Failed to add statement. Redland error code %1." ).arg( r ),
                                                         Error::ErrorUnknown ) ) );
            d->readWriteLock.unlock();
            return Error::ErrorUnknown;
        }
    }
    else {
        //
        // there is a bug (at least IMHO it is a bug) in redland which allows to add the same statement to one graph
        // multiple times.
        //
        librdf_node* redlandContext = d->world->createNode( statement.context() );
        if ( d->redlandContainsStatement( redlandStatement, redlandContext ) ||
             librdf_model_context_add_statement( d->model, redlandContext, redlandStatement ) ) {
            d->world->freeStatement( redlandStatement );
            d->world->freeNode( redlandContext );
            setError( d->world->lastError( Error::Error( "Failed to add statement",
                                                         Error::ErrorUnknown ) ) );
            d->readWriteLock.unlock();
            return Error::ErrorUnknown;
        }

        d->world->freeNode( redlandContext );
    }

    d->world->freeStatement( redlandStatement );

    // make sure we store everything in case we crash
    librdf_model_sync( d->model );

    d->readWriteLock.unlock();

    emit statementAdded( statement );
    emit statementsAdded();

    return Error::ErrorNone;
}


Soprano::NodeIterator Soprano::Redland::RedlandModel::listContexts() const
{
    clearError();

    d->readWriteLock.lockForRead();

    librdf_iterator *iter = librdf_model_get_contexts( d->model );
    if (!iter) {
        setError( d->world->lastError() );
        d->readWriteLock.unlock();
        return 0;
    }

    // we do not unlock d->readWriteLock here. That is done once the iterator closes
    NodeIteratorBackend* it = new NodeIteratorBackend( this, iter );
    d->nodeIterators.append( it );
    return it;
}


bool Soprano::Redland::RedlandModel::containsStatement( const Statement& statement ) const
{
    if ( statement.isValid() ) {
        MultiMutexReadLocker lock( &d->readWriteLock );
        if ( statement.context().isValid() ) {
            bool c = d->redlandContainsStatement( statement );
            setError( d->world->lastError() );
            return c;
        }
        else {
            return StorageModel::containsStatement( statement );
        }
    }
    else {
        setError( "Cannot check for invalid statement", Error::ErrorInvalidArgument );
        return false;
    }
}


bool Soprano::Redland::RedlandModel::containsAnyStatement( const Statement& statement ) const
{
    clearError();

    MultiMutexReadLocker lock( &d->readWriteLock );

    bool c = d->redlandContainsStatement( statement );
    setError( d->world->lastError() );
    return c;
}


Soprano::QueryResultIterator Soprano::Redland::RedlandModel::executeQuery( const QString &query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    d->readWriteLock.lockForRead();

    clearError();

    librdf_query *q = librdf_new_query( d->world->worldPtr(),
                                        Query::queryLanguageToString( language, userQueryLanguage ).toLower().toLatin1().data(),
                                        0,
                                        (unsigned char *)query.toLatin1().data(),
                                        0 );
    if ( !q ) {
        setError( d->world->lastError() );
        d->readWriteLock.unlock();
        return QueryResultIterator();
    }

    librdf_query_results *res = librdf_model_query_execute( d->model, q );
    if ( !res ) {
        librdf_free_query( q );
        setError( d->world->lastError() );
        d->readWriteLock.unlock();
        return QueryResultIterator();
    }

    // we do not unlock d->readWriteLock here. That is done once the iterator closes
    RedlandQueryResult* result = new RedlandQueryResult( this, res );
    d->results.append( result );
    return QueryResultIterator( result );
}


Soprano::StatementIterator Soprano::Redland::RedlandModel::listStatements( const Statement& partial ) const
{
    d->readWriteLock.lockForRead();

    clearError();

    librdf_stream* stream = d->redlandFindStatements( partial );
    if ( !stream ) {
        setError( d->world->lastError() );
        d->readWriteLock.unlock();
        return StatementIterator();
    }

    RedlandStatementIterator* it = new RedlandStatementIterator( this, stream, partial.context() );
    d->iterators.append( it );
    return StatementIterator( it );
}


Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::removeStatement( const Statement& statement )
{
    d->readWriteLock.lockForWrite();
    Error::ErrorCode r = removeOneStatement( statement );

    // make sure we store everything in case we crash
    librdf_model_sync( d->model );

    d->readWriteLock.unlock();
    if ( r == Error::ErrorNone ) {
        emit statementsRemoved();
    }
    return r;
}


Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::removeOneStatement( const Statement& statement )
{
    clearError();

    if ( !statement.isValid() ) {
        setError( "Cannot remove invalid statement", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    librdf_statement* redlandStatement = d->world->createStatement( statement );
    if ( !redlandStatement ) {
        setError( d->world->lastError() );
        return Error::ErrorInvalidArgument;
    }

    if ( statement.context().isEmpty() ) {
        if ( librdf_model_remove_statement( d->model, redlandStatement ) ) {
            d->world->freeStatement( redlandStatement );
            setError( d->world->lastError() );
            return Error::ErrorUnknown;
        }
    }
    else {
        librdf_node* redlandContext = d->world->createNode( statement.context() );
        if ( librdf_model_context_remove_statement( d->model, redlandContext, redlandStatement ) ) {
            d->world->freeNode( redlandContext );
            d->world->freeStatement( redlandStatement );
            setError( d->world->lastError() );
            return Error::ErrorUnknown;
        }
        d->world->freeNode( redlandContext );
    }

    d->world->freeStatement( redlandStatement );

    emit statementRemoved( statement );

    return Error::ErrorNone;
}


Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::removeAllStatements( const Statement &statement )
{
    clearError();

    if ( isContextOnlyStatement( statement ) ) {
        d->readWriteLock.lockForWrite();

        librdf_node *ctx = d->world->createNode( statement.context() );

        if ( librdf_model_context_remove_statements( d->model, ctx ) ) {
            d->world->freeNode( ctx );
            setError( d->world->lastError() );
            d->readWriteLock.unlock();
            return Error::ErrorUnknown;
        }

        d->world->freeNode( ctx );

        // make sure we store everything in case we crash
        librdf_model_sync( d->model );

        d->readWriteLock.unlock();

        // FIXME: list all the removed statements? That could mean a bad slowdown...
        emit statementRemoved( statement );
        emit statementsRemoved();

        return Error::ErrorNone;
    }

    else if ( !statement.isValid() ||
              !statement.context().isValid() ) {
        // FIXME: use redland streams for this
        QList<Statement> statementsToRemove = listStatements( statement ).allStatements();

        d->readWriteLock.lockForWrite();

        int cnt = 0;
        for ( QList<Statement>::const_iterator it = statementsToRemove.constBegin();
              it != statementsToRemove.constEnd(); ++it ) {
            ++cnt;
            Error::ErrorCode error = removeOneStatement( *it );
            if ( error != Error::ErrorNone ) {
                d->readWriteLock.unlock();
                return error;
            }
        }

        // make sure we store everything in case we crash
        librdf_model_sync( d->model );

        d->readWriteLock.unlock();

        if ( cnt ) {
            emit statementsRemoved();
        }
        return Error::ErrorNone;
    }

    else {
        return removeStatement( statement );
    }
}


int Soprano::Redland::RedlandModel::statementCount() const
{
    MultiMutexReadLocker lock( &d->readWriteLock );
    clearError();
    int size = librdf_model_size( d->model );
    if ( size < 0 ) {
        setError( d->world->lastError() );
    }
    return size;
}


Soprano::Node Soprano::Redland::RedlandModel::createBlankNode()
{
    clearError();
    Node n = d->world->createNode( librdf_new_node_from_blank_identifier( d->world->worldPtr(), 0 ) );
    if ( n.isEmpty() ) {
        setError( d->world->lastError() );
    }
    return n;
}


void Soprano::Redland::RedlandModel::removeIterator( RedlandStatementIterator* it ) const
{
    d->iterators.removeAll( it );
    d->readWriteLock.unlock();
}


void Soprano::Redland::RedlandModel::removeIterator( Redland::NodeIteratorBackend* it ) const
{
    d->nodeIterators.removeAll( it );
    d->readWriteLock.unlock();
}


void Soprano::Redland::RedlandModel::removeQueryResult( RedlandQueryResult* r ) const
{
    d->results.removeAll( r );
    d->readWriteLock.unlock();
}
