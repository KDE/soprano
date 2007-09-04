/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#include <soprano/querylegacy.h>
#include <soprano/queryresultiterator.h>
#include <soprano/statementiterator.h>
#include <soprano/nodeiterator.h>

#include "redlandworld.h"
#include "redlandutil.h"
#include "redlandqueryresult.h"
#include "redlandstatementiterator.h"
#include "redlandnodeiteratorbackend.h"

#include <QtCore/QReadWriteLock>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>
#include <QtCore/QDebug>


static bool isContextOnlyStatement( const Soprano::Statement& statement )
{
    return ( !statement.subject().isValid() &&
             !statement.predicate().isValid() &&
             !statement.object().isValid() &&
             statement.context().isValid() );
}


class Soprano::Redland::RedlandModel::Private {
public:
    Private() :
        world(0),
        model(0),
        storage(0)
    {}

    librdf_world *world;
    librdf_model *model;
    librdf_storage *storage;

    QReadWriteLock readWriteLock;

    QList<RedlandStatementIterator*> iterators;
    QList<Redland::NodeIteratorBackend*> nodeIterators;
    QList<RedlandQueryResult*> results;
};

Soprano::Redland::RedlandModel::RedlandModel( const Backend* b, librdf_model *model, librdf_storage *storage )
    : StorageModel( b )
{
    d = new Private;
    d->world = World::self()->worldPtr();
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

    delete d;
}


librdf_model *Soprano::Redland::RedlandModel::redlandModel() const
{
    return d->model;
}

Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::addStatement( const Statement &statement )
{
    if ( !statement.isValid() ) {
        setError( "Cannot add invalid statement", Error::ERROR_INVALID_ARGUMENT );
        return Error::ERROR_INVALID_ARGUMENT;
    }

    clearError();

    QWriteLocker lock( &d->readWriteLock );

    librdf_statement* redlandStatement = Util::createStatement( statement );
    if ( !redlandStatement ) {
        setError( Redland::World::self()->lastError() );
        return Error::ERROR_INVALID_ARGUMENT;
    }

    if ( statement.context().isEmpty() ) {
        if ( librdf_model_add_statement( d->model, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            setError( Redland::World::self()->lastError() );
            return Error::ERROR_UNKNOWN;
        }
    }
    else {
        librdf_node* redlandContext = Util::createNode( statement.context() );
        if ( librdf_model_context_add_statement( d->model, redlandContext, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            Util::freeNode( redlandContext );
            setError( Redland::World::self()->lastError() );
            return Error::ERROR_UNKNOWN;
        }
    }

    //if ( librdf_model_sync( d->model ) )
    //{
    //  return Error::ERROR_UNKNOWN;
    //}

    emit statementsAdded();

    return Error::ERROR_NONE;
}


Soprano::NodeIterator Soprano::Redland::RedlandModel::listContexts() const
{
    clearError();

    QReadLocker lock( &d->readWriteLock );

    librdf_iterator *iter = librdf_model_get_contexts( d->model );
    if (!iter) {
        setError( Redland::World::self()->lastError() );
        return 0;
    }

    // second lock for the iterator itself
    NodeIteratorBackend* it = new NodeIteratorBackend( this, iter );
    d->readWriteLock.lockForRead();

    d->nodeIterators.append( it );

    return it;
}

bool Soprano::Redland::RedlandModel::containsStatements( const Statement &statement ) const
{
    clearError();

    if ( isContextOnlyStatement( statement ) ) {
        QReadLocker lock( &d->readWriteLock );

        librdf_node *ctx = Util::createNode( statement.context() );
        if ( !ctx ) {
            setError( Redland::World::self()->lastError() );
            return false;
        }

        int result = librdf_model_contains_context( d->model, ctx );
        Util::freeNode( ctx );

        return result != 0;
    }

    // FIXME: looks as if librdf_model_contains_statement does not support
    // empty nodes and also there is no context support for contains.
    return listStatements( statement ).next();
}


Soprano::QueryResultIterator Soprano::Redland::RedlandModel::executeQuery( const QueryLegacy &query ) const
{
    QReadLocker lock( &d->readWriteLock );

    clearError();

    librdf_query *q = librdf_new_query( d->world, Util::queryType( query ), 0L, (unsigned char *)query.query().toLatin1().data(), 0L );
    if ( !q ) {
        setError( Redland::World::self()->lastError() );
        return QueryResultIterator();
    }

//     librdf_query_set_limit( q , query.limit() );
//     librdf_query_set_offset( q, query.offset() );

    librdf_query_results *res = librdf_model_query_execute( d->model, q );
    if ( !res ) {
        setError( Redland::World::self()->lastError() );
        return QueryResultIterator();
    }

    librdf_free_query( q );

    // second lock for the iterator itself
    RedlandQueryResult* result = new RedlandQueryResult( this, res );
    d->readWriteLock.lockForRead();

    d->results.append( result );

    return QueryResultIterator( result );
}


Soprano::StatementIterator Soprano::Redland::RedlandModel::listStatements( const Statement &partial ) const
{
    QReadLocker lock( &d->readWriteLock );

    clearError();

    if ( isContextOnlyStatement( partial ) ) {

        librdf_node *ctx = Util::createNode( partial.context() );

        librdf_stream *stream = librdf_model_context_as_stream( d->model, ctx );
        if ( !stream ) {
            setError( Redland::World::self()->lastError() );
            return StatementIterator();
        }
        Util::freeNode( ctx );

        // see listStatements( Statement ) for details on the context hack
        // second lock for the iterator itself
        d->readWriteLock.lockForRead();
        RedlandStatementIterator* it = new RedlandStatementIterator( this, stream, partial.context() );

        d->iterators.append( it );

        return StatementIterator( it );
    }
    else {
        librdf_statement *st = Util::createStatement( partial );
        if ( !st ) {
            setError( Redland::World::self()->lastError() );
            return StatementIterator();
        }

        librdf_node *ctx = Util::createNode( partial.context() );

        // FIXME: context support does not work, redland API claims that librdf_model_find_statements_in_context
        // with a NULL context is the same as librdf_model_find_statements. Well, in practice it is not.
        // We even have to set the context on all statements if we only search in one context!
        librdf_stream *stream = 0;
        if( partial.context().isEmpty() ) {
            stream = librdf_model_find_statements( d->model, st );
        }
        else {
            stream = librdf_model_find_statements_in_context( d->model, st, ctx );
        }

        if ( !stream ) {
            Util::freeNode( ctx );
            Util::freeStatement( st );
            setError( Redland::World::self()->lastError() );
            return StatementIterator();
        }
        Util::freeNode( ctx );
        Util::freeStatement( st );

        // second lock for the iterator itself
        d->readWriteLock.lockForRead();
        RedlandStatementIterator* it = new RedlandStatementIterator( this, stream, partial.context() );

        d->iterators.append( it );

        return StatementIterator( it );
    }
}


// internal method
Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::removeStatement( const Statement& statement )
{
    clearError();

    librdf_statement* redlandStatement = Util::createStatement( statement );
    if ( !redlandStatement ) {
        setError( Redland::World::self()->lastError() );
        return Error::ERROR_INVALID_ARGUMENT;
    }

    if ( statement.context().isEmpty() ) {
        if ( librdf_model_remove_statement( d->model, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            setError( Redland::World::self()->lastError() );
            return Error::ERROR_UNKNOWN;
        }
    }
    else {
        librdf_node* redlandContext = Util::createNode( statement.context() );
        if ( librdf_model_context_remove_statement( d->model, redlandContext, redlandStatement ) ) {
            Util::freeNode( redlandContext );
            Util::freeStatement( redlandStatement );
            setError( Redland::World::self()->lastError() );
            return Error::ERROR_UNKNOWN;
        }
        Util::freeNode( redlandContext );
    }

    Util::freeStatement( redlandStatement );

    return Error::ERROR_NONE;
}


Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::removeStatements( const Statement &statement )
{
    clearError();

    if ( isContextOnlyStatement( statement ) ) {
        QWriteLocker lock( &d->readWriteLock );

        librdf_node *ctx = Util::createNode( statement.context() );

        if (  librdf_model_context_remove_statements( d->model, ctx ) ) {
            Util::freeNode( ctx );
            setError( Redland::World::self()->lastError() );
            return Error::ERROR_UNKNOWN;
        }

        Util::freeNode( ctx );

        emit statementsRemoved();

        return Error::ERROR_NONE;
    }

    else if ( !statement.isValid() ||
              !statement.context().isValid() ) {
        // FIXME: use redland streams for this
        QList<Statement> statementsToRemove = listStatements( statement ).allStatements();

        QWriteLocker lock( &d->readWriteLock );

        int cnt = 0;
        for ( QList<Statement>::const_iterator it = statementsToRemove.constBegin();
              it != statementsToRemove.constEnd(); ++it ) {
            ++cnt;
            Error::ErrorCode error = removeStatement( *it );
            if ( error != Error::ERROR_NONE ) {
                return error;
            }
        }
        if ( cnt ) {
            emit statementsRemoved();
        }
        return Error::ERROR_NONE;
    }

    else {
        QWriteLocker lock( &d->readWriteLock );

        Error::ErrorCode error = removeStatement( statement );
        if ( error == Error::ERROR_NONE ) {
            emit statementsRemoved();
        }
        return error;
    }
}


int Soprano::Redland::RedlandModel::statementCount() const
{
    QReadLocker lock( &d->readWriteLock );
    clearError();
    int size = librdf_model_size( d->model );
    if ( size < 0 ) {
        setError( Redland::World::self()->lastError() );
    }
    return size;
}

Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::write( QTextStream &os ) const
{
    QReadLocker lock( &d->readWriteLock );

    clearError();

    if ( unsigned char *serialized = librdf_model_to_string( d->model, 0, 0, 0, 0  ) ) {
        os << ( const char* )serialized;
        free( serialized );
        return Error::ERROR_NONE;
    }
    else {
        setError( Redland::World::self()->lastError() );
        return Error::ERROR_UNKNOWN;
    }
}

Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::print() const
{
    clearError();

    QReadLocker lock( &d->readWriteLock );

    librdf_model_print( d->model, stdout );

    return Error::ERROR_NONE;
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
