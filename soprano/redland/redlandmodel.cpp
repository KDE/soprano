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

#include "query.h"
#include "resultset.h"
#include "statementiterator.h"
#include "redlandworld.h"
#include "redlandutil.h"
#include "redlandqueryresult.h"
#include "redlandstatementiterator.h"

#include <QtCore/QReadWriteLock>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>
#include <QtCore/QDebug>


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
    QList<RedlandQueryResult*> results;
};

Soprano::Redland::RedlandModel::RedlandModel( librdf_model *model, librdf_storage *storage )
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

Soprano::ErrorCode Soprano::Redland::RedlandModel::add( const Statement &statement )
{
    if ( !statement.isValid() ) {
        return ERROR_INVALID_STATEMENT;
    }

    QWriteLocker lock( &d->readWriteLock );

    librdf_statement* redlandStatement = Util::createStatement( statement );
    if ( !redlandStatement ) {
        return ERROR_INVALID_STATEMENT;
    }

    if ( statement.context().isEmpty() ) {
        if ( librdf_model_add_statement( d->model, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            return ERROR_UNKNOW;
        }
    }
    else {
        librdf_node* redlandContext = Util::createNode( statement.context() );
        if ( librdf_model_context_add_statement( d->model, redlandContext, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            Util::freeNode( redlandContext );
            return ERROR_UNKNOW;
        }
    }

    //if ( librdf_model_sync( d->model ) )
    //{
    //  return ERROR_UNKNOW;
    //}

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Redland::RedlandModel::add( const Statement &statement, const Node &context )
{
    Statement s( statement );
    s.setContext( context );
    return add( s );
}

QList<Soprano::Node> Soprano::Redland::RedlandModel::contexts() const
{
    QList<Node> contexts;

    QReadLocker lock( &d->readWriteLock );

    librdf_iterator *iter = librdf_model_get_contexts( d->model );
    if (!iter)
    {
        return contexts;
    }

    while ( librdf_iterator_end( iter ) == 0 )
    {
        librdf_node *ctx = (librdf_node *)librdf_iterator_get_context( iter );
        contexts.append( Util::createNode( ctx ) );
        librdf_iterator_next( iter );
    }

    return contexts;
}

bool Soprano::Redland::RedlandModel::contains( const Statement &statement ) const
{
    // FIXME: looks as if librdf_model_contains_statement does not support
    // empty nodes and also there is no context support for contains.
    return listStatements( statement ).hasNext();
}

bool Soprano::Redland::RedlandModel::contains( const Node &context ) const
{
    if ( !context.isValid() )
    {
        return false;
    }

    QReadLocker lock( &d->readWriteLock );

    librdf_node *ctx = Util::createNode( context );
    if ( !ctx )
    {
        return false;
    }

    int result = librdf_model_contains_context( d->model, ctx );
    Util::freeNode( ctx );

    return result != 0;
}

Soprano::ResultSet Soprano::Redland::RedlandModel::executeQuery( const Query &query ) const
{
    QReadLocker lock( &d->readWriteLock );

    librdf_query *q = librdf_new_query( d->world, Util::queryType( query ), 0L, (unsigned char *)query.query().toLatin1().data(), 0L );
    if ( !q )
    {
        return ResultSet();
    }

    librdf_query_set_limit( q , query.limit() );
    librdf_query_set_offset( q, query.offset() );

    librdf_query_results *res = librdf_model_query_execute( d->model, q );
    if ( !res )
    {
        return ResultSet();
    }

    librdf_free_query( q );

    RedlandQueryResult* result = new RedlandQueryResult( this, res );
    d->results.append( result );

    return ResultSet( result );
}

Soprano::StatementIterator Soprano::Redland::RedlandModel::listStatements( const Node &context ) const
{
    QReadLocker lock( &d->readWriteLock );

    if ( !context.isValid() )
    {
        return StatementIterator();
    }

    librdf_node *ctx = Util::createNode( context );

    librdf_stream *stream = librdf_model_context_as_stream( d->model, ctx );
    if ( !stream )
    {
        return StatementIterator();
    }
    Util::freeNode( ctx );

    // see listStatements( Statement ) for details on the context hack
    RedlandStatementIterator* it = new RedlandStatementIterator( this, stream, context );
    d->iterators.append( it );

    return StatementIterator( it );
}

Soprano::StatementIterator Soprano::Redland::RedlandModel::listStatements( const Statement &partial, const Node &context ) const
{
    Statement s( partial );
    s.setContext( context );
    return listStatements( s );
}

Soprano::StatementIterator Soprano::Redland::RedlandModel::listStatements( const Statement &partial ) const
{
    QReadLocker lock( &d->readWriteLock );

    librdf_statement *st = Util::createStatement( partial );
    if ( !st ) {
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
        return StatementIterator();
    }
    Util::freeNode( ctx );
    Util::freeStatement( st );

    RedlandStatementIterator* it = new RedlandStatementIterator( this, stream, partial.context() );
    d->iterators.append( it );

    return StatementIterator( it );
}

Soprano::ErrorCode Soprano::Redland::RedlandModel::remove( const Statement &statement, const Node &context )
{
    Statement s( statement );
    s.setContext( context );
    return remove( s );
}

Soprano::ErrorCode Soprano::Redland::RedlandModel::remove( const Statement &statement )
{
    QWriteLocker lock( &d->readWriteLock );

    if ( !statement.isValid() ) {
        return ERROR_INVALID_STATEMENT;
    }

    librdf_statement* redlandStatement = Util::createStatement( statement );
    if ( !redlandStatement ) {
        return ERROR_INVALID_STATEMENT;
    }

    if ( statement.context().isEmpty() ) {
        if ( librdf_model_remove_statement( d->model, redlandStatement ) ) {
            Util::freeStatement( redlandStatement );
            return ERROR_UNKNOW;
        }
    }
    else {
        librdf_node* redlandContext = Util::createNode( statement.context() );
        if ( librdf_model_context_remove_statement( d->model, redlandContext, redlandStatement ) ) {
            Util::freeNode( redlandContext );
            Util::freeStatement( redlandStatement );
            return ERROR_UNKNOW;
        }
        Util::freeNode( redlandContext );
    }

    Util::freeStatement( redlandStatement );

    // Sync the model
    //if ( librdf_model_sync( d->model ) )
    //{
    //  return ERROR_UNKNOW;
    //}

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Redland::RedlandModel::remove( const Node &context )
{
    QWriteLocker lock( &d->readWriteLock );

    if ( !context.isValid() )
    {
        return ERROR_UNKNOW;
    }

    librdf_node *ctx = Util::createNode( context );

    if (  librdf_model_context_remove_statements( d->model, ctx ) )
    {
        Util::freeNode( ctx );
        return ERROR_UNKNOW;
    }

    Util::freeNode( ctx );

    // Sync the model
    //if ( librdf_model_sync( d->model ) )
    //{
    //  return ERROR_UNKNOW;
    //}

    return ERROR_NONE;
}

int Soprano::Redland::RedlandModel::size() const
{
    QReadLocker lock( &d->readWriteLock );
    return librdf_model_size( d->model );
}

Soprano::ErrorCode Soprano::Redland::RedlandModel::write( QTextStream &os ) const
{
    QReadLocker lock( &d->readWriteLock );

    unsigned char *serialized = librdf_model_to_string( d->model, 0L, 0L, 0L, 0L  );
    QString tmp( (const char *) serialized );

    os << tmp;
    os.flush();

    free( serialized );

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Redland::RedlandModel::print() const
{
    QReadLocker lock( &d->readWriteLock );

    librdf_model_print( d->model, stdout );

    return ERROR_NONE;
}


void Soprano::Redland::RedlandModel::removeIterator( RedlandStatementIterator* it ) const
{
    d->iterators.removeAll( it );
}


void Soprano::Redland::RedlandModel::removeQueryResult( RedlandQueryResult* r ) const
{
    d->results.removeAll( r );
}
