/* 
 * This file is part of Soprano Project.
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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "Model.h"
#include "Node.h"
#include "RedlandUtil.h"
#include "RedlandQueryResult.h"
#include "RedlandModelFactory.h"
#include "RedlandStatementIterator.h"

using namespace Soprano::Backend::Redland;

struct RedlandQueryResult::Private
{
  Private() : result(0L)
  {}
  librdf_query_results *result;

  QStringList names;
  bool first;
};

RedlandQueryResult::RedlandQueryResult( librdf_query_results *result )
{
  d = new Private;
  d->result = result;
  Q_ASSERT( d->result != 0L );

  for (int offset = 0; offset < bindingCount(); offset++) {
    d->names.append( QString( librdf_query_results_get_binding_name( result, offset ) ) );
  }
  d->first = true;
}

RedlandQueryResult::~RedlandQueryResult()
{
  librdf_free_query_results( d->result );
  delete d;
}

bool RedlandQueryResult::next() const
{
  bool hasNext = librdf_query_results_finished( d->result ) == 0;

  if ( !d->first )
  {
    hasNext = ( librdf_query_results_next( d->result ) == 0 );
  } 
  else 
  {
    d->first = false;
  }

  return hasNext;
}

Soprano::Node RedlandQueryResult::binding( const QString &name ) const
{
  librdf_node *node = librdf_query_results_get_binding_value_by_name( d->result, (const char *)name.toLatin1().data() );
  if ( !node )
  {
    // Return a not valid node (empty)
    return Soprano::Node();
  }

  Soprano::Node tmp = Util::createNode( node );
  librdf_free_node( node );
  
  return tmp;
}

Soprano::Node RedlandQueryResult::binding( int offset ) const
{
  librdf_node *node = librdf_query_results_get_binding_value( d->result, offset );
  if ( !node )
  {
    // Return a not valid node (empty)
    return Soprano::Node();
  }

  Soprano::Node tmp = Util::createNode( node );
  librdf_free_node( node );
  
  return tmp;
}

int RedlandQueryResult::bindingCount() const
{
  return librdf_query_results_get_count( d->result );
}

const QStringList &RedlandQueryResult::bindingNames() const
{
  return d->names;
}

bool RedlandQueryResult::isGraph() const
{
  return librdf_query_results_is_graph( d->result ) != 0;
}

bool RedlandQueryResult::isBinding() const
{
  return librdf_query_results_is_bindings( d->result ) != 0;
}

bool RedlandQueryResult::isBool() const
{
  return librdf_query_results_is_boolean( d->result ) != 0;
}

bool RedlandQueryResult::boolValue() const
{
  return librdf_query_results_get_boolean( d->result ) > 0;
}

Soprano::Model *RedlandQueryResult::model() const
{
  RedlandModelFactory factory;
  Model *model = factory.createMemoryModel( "query-result-model" );

  librdf_stream *stream = librdf_query_results_as_stream( d->result );
  if ( !stream )
  {
    return model;
  }
  
  model->add( RedlandStatementIterator( stream ) );

  return model;
} 
