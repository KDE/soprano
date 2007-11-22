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

#include <soprano/queryresultiterator.h>
#include <soprano/statementiterator.h>
#include <soprano/nodeiterator.h>

#include "redlandworld.h"
#include "redlandutil.h"
#include "redlandqueryresult.h"
#include "redlandstatementiterator.h"
#include "redlandnodeiteratorbackend.h"
#include "multimutex.h"

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

    MultiMutex readWriteLock; // restricts multiple reads to one thread

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
        setError( "Cannot add invalid statement", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    clearError();

    d->readWriteLock.lockForWrite();

    librdf_statement* redlandStatement = Util::createStatement( statement );
    if ( !redlandStatement ) {
        setError( Redland::World::self()->lastError() );
        d->readWriteLock.unlock();
        return Error::ErrorInvalidArgument;
    }

    if ( statement.context().isEmpty() ) {
        if ( librdf_model_add_statement( d->model, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            setError( Redland::World::self()->lastError() );
            d->readWriteLock.unlock();
            return Error::ErrorUnknown;
        }
    }
    else {
        librdf_node* redlandContext = Util::createNode( statement.context() );
        if ( librdf_model_context_add_statement( d->model, redlandContext, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            Util::freeNode( redlandContext );
            setError( Redland::World::self()->lastError() );
            d->readWriteLock.unlock();
            return Error::ErrorUnknown;
        }

        Util::freeNode( redlandContext );
    }

    Util::freeStatement( redlandStatement );

    // make sure we store everything in case we crash
    librdf_model_sync( d->model );

    d->readWriteLock.unlock();

    emit statementsAdded();

    return Error::ErrorNone;
}


Soprano::NodeIterator Soprano::Redland::RedlandModel::listContexts() const
{
    clearError();

    d->readWriteLock.lockForRead();

    librdf_iterator *iter = librdf_model_get_contexts( d->model );
    if (!iter) {
        setError( Redland::World::self()->lastError() );
        d->readWriteLock.unlock();
        return 0;
    }

    // we do not unlock d->readWriteLock here. That is done once the iterator closes
    NodeIteratorBackend* it = new NodeIteratorBackend( this, iter );
    d->nodeIterators.append( it );
    return it;
}

bool Soprano::Redland::RedlandModel::containsAnyStatement( const Statement &statement ) const
{
    clearError();

    if ( isContextOnlyStatement( statement ) ) {
        MultiMutexReadLocker lock( &d->readWriteLock );

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


Soprano::QueryResultIterator Soprano::Redland::RedlandModel::executeQuery( const QString &query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    d->readWriteLock.lockForRead();

    clearError();

    librdf_query *q = librdf_new_query( d->world,
                                        Query::queryLanguageToString( language, userQueryLanguage ).toLower().toLatin1().data(),
                                        0,
                                        (unsigned char *)query.toLatin1().data(),
                                        0 );
    if ( !q ) {
        setError( Redland::World::self()->lastError() );
        d->readWriteLock.unlock();
        return QueryResultIterator();
    }

    librdf_query_results *res = librdf_model_query_execute( d->model, q );
    if ( !res ) {
        setError( Redland::World::self()->lastError() );
        d->readWriteLock.unlock();
        return QueryResultIterator();
    }

    librdf_free_query( q );

    // we do not unlock d->readWriteLock here. That is done once the iterator closes
    RedlandQueryResult* result = new RedlandQueryResult( this, res );
    d->results.append( result );
    return QueryResultIterator( result );
}


Soprano::StatementIterator Soprano::Redland::RedlandModel::listStatements( const Statement &partial ) const
{
    d->readWriteLock.lockForRead();

    clearError();

    if ( isContextOnlyStatement( partial ) ) {

        librdf_node *ctx = Util::createNode( partial.context() );

        librdf_stream *stream = librdf_model_context_as_stream( d->model, ctx );
        Util::freeNode( ctx );
        if ( !stream ) {
            setError( Redland::World::self()->lastError() );
            d->readWriteLock.unlock();
            return StatementIterator();
        }

        // see listStatements( Statement ) for details on the context hack
        // second lock for the iterator itself
        RedlandStatementIterator* it = new RedlandStatementIterator( this, stream, partial.context() );
        d->iterators.append( it );
        return StatementIterator( it );
    }
    else {
        librdf_statement *st = Util::createStatement( partial );
        if ( !st ) {
            setError( Redland::World::self()->lastError() );
            d->readWriteLock.unlock();
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

        Util::freeNode( ctx );
        Util::freeStatement( st );

        if ( !stream ) {
            setError( Redland::World::self()->lastError() );
            d->readWriteLock.unlock();
            return StatementIterator();
        }

        // we do not unlock d->readWriteLock here. That is done once the iterator closes
        RedlandStatementIterator* it = new RedlandStatementIterator( this, stream, partial.context() );
        d->iterators.append( it );
        return StatementIterator( it );
    }
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

    librdf_statement* redlandStatement = Util::createStatement( statement );
    if ( !redlandStatement ) {
        setError( Redland::World::self()->lastError() );
        return Error::ErrorInvalidArgument;
    }

    if ( statement.context().isEmpty() ) {
        if ( librdf_model_remove_statement( d->model, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            setError( Redland::World::self()->lastError() );
            return Error::ErrorUnknown;
        }
    }
    else {
        librdf_node* redlandContext = Util::createNode( statement.context() );
        if ( librdf_model_context_remove_statement( d->model, redlandContext, redlandStatement ) ) {
            Util::freeNode( redlandContext );
            Util::freeStatement( redlandStatement );
            setError( Redland::World::self()->lastError() );
            return Error::ErrorUnknown;
        }
        Util::freeNode( redlandContext );
    }

    Util::freeStatement( redlandStatement );

    return Error::ErrorNone;
}


Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::removeAllStatements( const Statement &statement )
{
    clearError();

    if ( isContextOnlyStatement( statement ) ) {
        d->readWriteLock.lockForWrite();

        librdf_node *ctx = Util::createNode( statement.context() );

        if ( librdf_model_context_remove_statements( d->model, ctx ) ) {
            Util::freeNode( ctx );
            setError( Redland::World::self()->lastError() );
            d->readWriteLock.unlock();
            return Error::ErrorUnknown;
        }

        Util::freeNode( ctx );

        // make sure we store everything in case we crash
        librdf_model_sync( d->model );

        d->readWriteLock.unlock();

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
        setError( Redland::World::self()->lastError() );
    }
    return size;
}

Soprano::Error::ErrorCode Soprano::Redland::RedlandModel::write( QTextStream &os ) const
{
    MultiMutexReadLocker lock( &d->readWriteLock );

    clearError();

    if ( unsigned char *serialized = librdf_model_to_string( d->model, 0, 0, 0, 0  ) ) {
        os << ( const char* )serialized;
        free( serialized );
        return Error::ErrorNone;
    }
    else {
        setError( Redland::World::self()->lastError() );
        return Error::ErrorUnknown;
    }
}

Soprano::Node Soprano::Redland::RedlandModel::createBlankNode()
{
    clearError();
    Node n = Util::createNode( librdf_new_node_from_blank_identifier( Redland::World::self()->worldPtr(), 0 ) );
    if ( n.isEmpty() ) {
        setError( Redland::World::self()->lastError() );
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
