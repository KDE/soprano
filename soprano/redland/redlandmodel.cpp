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

#include "query.h"
#include "resultset.h"
#include "statementiterator.h"
#include "redlandworld.h"
#include "redlandutil.h"
#include "redlandqueryresult.h"
#include "redlandstatementiterator.h"
#include "redlandmodel.h"

#include "redland_stream_adapter.h"

namespace Soprano {
  namespace Redland {

class RedlandModel::Private {
public:
  Private(): world(0L), model(0L), storage(0L)
  {}

  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;

  QList<stream_adapter *> streams;
};

RedlandModel::RedlandModel( librdf_model *model, librdf_storage *storage )
{
  d = new Private;
  d->world = World::self()->worldPtr();
  d->model = model;
  d->storage = storage;

  Q_ASSERT( model != 0L );
  Q_ASSERT( storage != 0L );
}

RedlandModel::~RedlandModel()
{
  QListIterator<stream_adapter *> iter( d->streams );
  while ( iter.hasNext() )
  {
    stream_adapter *stream = iter.next();
    if ( !stream->impl )
    {
      // Statement is dead!
      free_stream_adapter( stream );
    } else {
      free_stream_adapter_backend( stream);
    }
  }

  librdf_free_model( d->model );
  librdf_free_storage( d->storage );

  delete d;
}

librdf_model *RedlandModel::redlandModel() const
{
  return d->model;
}

ErrorCode RedlandModel::add( const Statement &statement )
{
  if ( !statement.isValid() )
  {
    return ERROR_INVALID_STATEMENT;
  }

  librdf_node *subject = Util::createNode( statement.subject() );
  if ( !subject )
  {
    return ERROR_UNKNOW;
  }

  librdf_node *predicate = Util::createNode( statement.predicate() );
  if ( !predicate )
  {
    Util::freeNode( subject );
    return ERROR_UNKNOW;
  }

  librdf_node *object = Util::createNode( statement.object() );
  if ( !object )
  {
    Util::freeNode( subject );
    Util::freeNode( predicate );
    return ERROR_UNKNOW;
  }

  if ( librdf_model_add( d->model, subject, predicate, object ) )
  {
    Util::freeNode( subject );
    Util::freeNode( predicate );
    Util::freeNode( object );
    return ERROR_UNKNOW;
  }

  //if ( librdf_model_sync( d->model ) )
  //{
  //  return ERROR_UNKNOW;
  //}

  return ERROR_NONE;
}

ErrorCode RedlandModel::add( const Statement &statement, const Node &context )
{
  if ( !statement.isValid() )
  {
    return ERROR_INVALID_STATEMENT;
  }

  librdf_node *ctx = Util::createNode( context );

  librdf_statement *st = Util::createStatement( statement );
  if ( !st )
  {
    Util::freeNode( ctx );
    return ERROR_UNKNOW;
  }

  if ( librdf_model_context_add_statement( d->model, ctx, st ) )
  {
    Util::freeNode( ctx );
    Util::freeStatement( st );
    return ERROR_UNKNOW;
  }

  Util::freeNode( ctx );
  Util::freeStatement( st );

  return ERROR_NONE;
}

QList<Node> RedlandModel::contexts() const
{
  QList<Node> contexts;

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

bool RedlandModel::contains( const Statement &statement ) const
{
  if ( !statement.isValid() )
  {
    return false;
  }

  librdf_statement *st = Util::createStatement( statement );
  if ( !st )
  {
    return false;
  }

  int result = librdf_model_contains_statement( d->model, st );
  Util::freeStatement( st );

  return result != 0;
}

bool RedlandModel::contains( const Node &context ) const
{
  if ( !context.isValid() )
  {
    return false;
  }

  librdf_node *ctx = Util::createNode( context );
  if ( !ctx )
  {
    return false;
  }

  int result = librdf_model_contains_context( d->model, ctx );
  Util::freeNode( ctx );

  return result != 0;
}

Soprano::ResultSet RedlandModel::executeQuery( const Query &query ) const
{
  librdf_query *q = librdf_new_query( d->world, Util::queryType( query ), 0L, (unsigned char *)query.query().toLatin1().data(), 0L );
  if ( !q )
  {
    return ResultSet( 0L );
  }

  librdf_query_set_limit( q , query.limit() );
  librdf_query_set_offset( q, query.offset() );

  librdf_query_results *res = librdf_model_query_execute( d->model, q );
  if ( !res )
  {
    return ResultSet( 0L );
  }

  librdf_free_query( q );

  return ResultSet( new RedlandQueryResult( res ) );
}

Soprano::StatementIterator RedlandModel::listStatements( const Node &context ) const
{
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

  stream_adapter *s = (stream_adapter *) malloc( sizeof( stream_adapter ) );
  s->impl = stream;

  d->streams.append( s );

  return StatementIterator( new RedlandStatementIterator( s ) );
}

Soprano::StatementIterator RedlandModel::listStatements( const Statement &partial, const Node &context ) const
{
  librdf_statement *st = Util::createStatement( partial );
  if ( !st )
  {
    return StatementIterator();
  }

  librdf_node *ctx = Util::createNode( context );

  // FIXME: context support does not work, redland API claims that librdf_model_find_statements_in_context
  // with a NULL context is the same as librdf_model_find_statements. Well, in practive it is not.
  // I did not test non-empty contexts yet but for now we use a hack here
  librdf_stream *stream = 0;
  if( context.isEmpty() )
    stream = librdf_model_find_statements( d->model, st );
  else
    stream = librdf_model_find_statements_in_context( d->model, st, ctx );

  if ( !stream )
  {
    Util::freeNode( ctx );
    Util::freeStatement( st );
    return StatementIterator();
  }
  Util::freeNode( ctx );
  Util::freeStatement( st );

  stream_adapter *s = (stream_adapter *) malloc( sizeof( stream_adapter ) );
  s->impl = stream;

  d->streams.append( s );

  return StatementIterator( new RedlandStatementIterator( s ) );;
}

Soprano::StatementIterator RedlandModel::listStatements( const Statement &partial ) const
{
  librdf_statement *st = Util::createStatement( partial );
  if ( !st )
  {
    return StatementIterator();
  }
  
  librdf_stream *stream = librdf_model_find_statements( d->model, st );
  if ( !stream )
  {
    Util::freeStatement( st );
    return StatementIterator();
  }
  Util::freeStatement( st );

  stream_adapter *s = (stream_adapter *) malloc( sizeof( stream_adapter ) );
  s->impl = stream;

  d->streams.append( s );

  return StatementIterator( new RedlandStatementIterator( s ) );;
}

ErrorCode RedlandModel::remove( const Statement &statement, const Node &context ) 
{
  if ( !statement.isValid() )
  {
    return ERROR_INVALID_STATEMENT;
  }

  librdf_node *ctx = Util::createNode( context );

  librdf_node *subject = Util::createNode( statement.subject() );
  librdf_node *predicate = Util::createNode( statement.predicate() );
  librdf_node *object = Util::createNode( statement.object() );

  librdf_statement *st = librdf_new_statement_from_nodes( d->world , subject, predicate, object );

  if ( librdf_model_context_remove_statement( d->model, ctx, st ) )
  {
    Util::freeNode( ctx );
    Util::freeStatement( st );
    return ERROR_UNKNOW;
  }

  Util::freeNode( ctx );
  Util::freeStatement( st );

  // Sync the model
  //if ( librdf_model_sync( d->model ) )
  //{
  //  return ERROR_UNKNOW;
  //}

  return ERROR_NONE;
}

ErrorCode RedlandModel::remove( const Statement &statement )
{
  if ( !statement.isValid() )
  {
    return ERROR_INVALID_STATEMENT;
  }

  librdf_node *subject = Util::createNode( statement.subject() );
  librdf_node *predicate = Util::createNode( statement.predicate() );
  librdf_node *object = Util::createNode( statement.object() );

  librdf_statement *st = librdf_new_statement_from_nodes( d->world , subject, predicate, object );
  if ( !st )
  {
    Util::freeNode( subject );
    Util::freeNode( predicate );
    Util::freeNode( object );
    return ERROR_UNKNOW;
  }

  if ( librdf_model_remove_statement( d->model, st ) )
  {
    Util::freeStatement( st );
    return ERROR_UNKNOW;
  }

  Util::freeStatement( st );

  // Sync the model
  //if ( librdf_model_sync( d->model ) )
  //{
  //  return ERROR_UNKNOW;
  //}

  return ERROR_NONE;
}

ErrorCode RedlandModel::remove( const Node &context )
{
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

int RedlandModel::size() const
{
  return librdf_model_size( d->model );
}

ErrorCode RedlandModel::write( QTextStream &os ) const
{
  unsigned char *serialized = librdf_model_to_string( d->model, 0L, 0L, 0L, 0L  );
  QString tmp( (const char *) serialized );

  os << tmp;
  os.flush();

  free( serialized );

  return ERROR_NONE;
}

ErrorCode RedlandModel::print() const
{
  librdf_model_print( d->model, stdout );

  return ERROR_NONE;
}

}
}
