/* This file is part of Soprano
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

#include "RedlandUtil.h"
#include "RedlandQueryResult.h"
using namespace Soprano;

struct RedlandQueryResult::Private
{
  Private() : result(0L)
  {}
  librdf_query_results *result;
};

RedlandQueryResult::RedlandQueryResult( librdf_query_results *result )
{
  d = new Private;
  d->result = result;
  Q_ASSERT( d->result != 0L );
}

RedlandQueryResult::~RedlandQueryResult()
{
  librdf_free_query_results( d->result );
  delete d;
}

int RedlandQueryResult::count() const
{
  return librdf_query_results_get_count( d->result );
}

bool RedlandQueryResult::hasNext() const
{
  return librdf_query_results_finished( d->result ) == 0;
}

bool RedlandQueryResult::next() const
{
  return librdf_query_results_next( d->result ) != 0;
}

Node RedlandQueryResult::getBinding( const QString &name ) const
{
  librdf_node *node = librdf_query_results_get_binding_value_by_name( d->result, (const char *)name.toLatin1().data() );
  Q_ASSERT( node != 0L );

  Node tmp = Redland::createNode( node );
  librdf_free_node( node );
  
  return tmp;
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
