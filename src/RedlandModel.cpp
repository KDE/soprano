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
 * License
 * along with this library; see the file COPYING.LIB.  If not, write
 * to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "RedlandModel.h"
using namespace RDF;

struct RedlandModel::Private {
  Private(): world(0L), model(0L), storage(0L)
  {}

  librdf_world *world;
  librdf_model *model;
  librdf_storage *storage;
};

RedlandModel::RedlandModel( librdf_world *world, librdf_model *model, librdf_storage *storage )
{
  d = new Private;
  d->world = world;
  d->model = model;
  d->storage = storage;
}

RedlandModel::RedlandModel( const RedlandModel &other )
{
  d = new Private;
  d->world = other.worldPtr();
  d->model = librdf_new_model_from_model( other.modelPtr() );
  d->storage = librdf_new_storage_from_storage( other.storagePtr() );
}

RedlandModel::~RedlandModel()
{
  librdf_free_model( d->model );
  librdf_free_storage( d->storage );
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
  librdf_node *subject = librdf_new_node_from_uri_string( d->world, (unsigned char *) st.subject().uri().toString().toLatin1().data() );
  librdf_node *predicate = librdf_new_node_from_uri_string( d->world, (unsigned char *) st.predicate().uri().toString().toLatin1().data() );
  
  librdf_node *object;
  if ( st.object().isResource() )
  {
    object = librdf_new_node_from_uri_string( d->world, (unsigned char *) st.object().uri().toString().toLatin1().data() );
  } 
  else if ( st.object().isLiteral() )
  {
    object = librdf_new_node_from_literal( d->world, (const unsigned char *) st.object().literal().toLatin1().data(), 0L, 0L);
  } 
  else if ( st.object().isBlank() )
  {
    object = librdf_new_node_from_blank_identifier( d->world, (unsigned char *) st.object().literal().toLatin1().data() );
  }
  
  librdf_model_add( d->model, subject, predicate, object );
}

Node &RedlandModel::createProperty( const QString &ns, const QString &value )
{
}

Node &RedlandModel::createBlankNode( const QString &uri )
{
}

Node &RedlandModel::createResource( const QUrl &uri )
{
}

Node &RedlandModel::createLiteral( const QString &literal )
{
}

bool RedlandModel::isEmpty()
{
}

bool RedlandModel::contains( const Statement &partial )
{
}

void RedlandModel::remove( const Statement &st )
{
}

void RedlandModel::remove( const QList<Statement> &statements )
{
}

int RedlandModel::size()
{
}

void RedlandModel::write( FILE *fh )
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

