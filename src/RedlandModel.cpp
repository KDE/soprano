/* This file is part of QRDF
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "RedlandUtil.h"
#include "RedlandQueryResult.h"
#include "RedlandModel.h"
using namespace RDF;


struct RedlandModel::Private {
  Private(): world(0L), model(0L)
  {}

  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;
};

RedlandModel::RedlandModel( librdf_world *world, librdf_model *model )
{
  d = new Private;
  d->world = world;
  d->model = model; 
  d->storage = librdf_model_get_storage( model );
}

RedlandModel::RedlandModel( const RedlandModel &other )
{
  d = new Private;
  d->world = other.worldPtr();
  d->model = librdf_new_model_from_model( other.modelPtr() );
  d->storage = librdf_new_storage_from_storage( other.storagePtr() );

  librdf_storage_open( d->storage, d->model );
}

RedlandModel::~RedlandModel()
{
  librdf_storage_close( d->storage );

  librdf_free_model( d->model );
  librdf_free_storage( storagePtr() );
  delete d;
}

void RedlandModel::add( const QList<Statement> &statements )
{

}

void RedlandModel::add( const Model &model )
{
  
}

void RedlandModel::add( const Statement &st )
{
  librdf_node *subject = Redland::createNode( d->world, st.subject() );
  librdf_node *predicate = Redland::createNode( d->world, st.predicate() );
  librdf_node *object = Redland::createNode( d->world, st.object() );
  
  librdf_model_add( d->model, subject, predicate, object );
}

bool RedlandModel::isEmpty() const
{
  return librdf_model_size( d->model ) == 0;
}

bool RedlandModel::contains( const Statement &statement ) const
{
  if ( !statement.isValid() ) return false;

  librdf_statement *st = Redland::createStatement( d->world, statement );

  int result = librdf_model_contains_statement( d->model, st );

  librdf_free_statement( st );

  return result != 0;
}

QueryResult *RedlandModel::execute( const Query &query ) const
{
  qDebug("build a query object..");
  librdf_query *q = librdf_new_query( d->world, Redland::queryType( query ), 0L, (unsigned char *)query.query().toLatin1().data(), 0L );
  Q_ASSERT( q != 0L );

  librdf_query_set_limit( q , query.limit() );
  librdf_query_set_offset( q, query.offset() );
  
  if (q)
      qDebug("done.");
  
  qDebug("limit: %d", librdf_query_get_limit( q ));
  qDebug("offset: %d", librdf_query_get_offset( q ));
  qDebug("executing query...");
 
  librdf_query_results *res = librdf_model_query_execute( d->model, q );
  librdf_free_query( q );

  return new RedlandQueryResult( res );
}

void RedlandModel::remove( const Statement &st )
{
  librdf_node *subject = Redland::createNode( d->world, st.subject() );
  librdf_node *predicate = Redland::createNode( d->world, st.predicate() );
  librdf_node *object = Redland::createNode( d->world, st.object() );

  librdf_statement *statement = librdf_new_statement_from_nodes( d->world , subject, predicate, object );
  librdf_model_remove_statement( d->model, statement );
  librdf_free_statement( statement );
}

void RedlandModel::remove( const QList<Statement> &statements )
{

}

int RedlandModel::size() const
{
  return librdf_model_size( d->model );
}

void RedlandModel::write( FILE *fh ) const
{
  librdf_model_print( d->model, fh ); 
}

librdf_world *RedlandModel::worldPtr() const
{
  return d->world;
}

librdf_model *RedlandModel::modelPtr() const
{
  return d->model;
}

librdf_storage *RedlandModel::storagePtr() const
{
  return d->storage;
}

