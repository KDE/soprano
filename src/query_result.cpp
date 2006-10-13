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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QtGlobal>
#include "node.h"
#include "query_result.h"

using namespace RDF;

struct QueryResult::Private
{
  Private() : queryResult(0L)
  {}
  librdf_query_results *queryResult;
};

QueryResult::QueryResult(librdf_query_results *qr)
{
  d = new Private;
  d->queryResult = qr;
  Q_ASSERT( d->queryResult != NULL );
}

QueryResult::~QueryResult()
{
  librdf_free_query_results( d->queryResult );
  delete d;
}

int QueryResult::count()
{
  return librdf_query_results_get_count( d->queryResult );
}

bool QueryResult::hasNext()
{
  return librdf_query_results_next( d->queryResult ) != 0;
}

Node *QueryResult::getBinding(int offset)
{
  return new Node( librdf_query_results_get_binding_value( d->queryResult, offset ) );
}

bool QueryResult::isBoolean()
{
  return librdf_query_results_is_boolean( d->queryResult ) != 0;
}

bool QueryResult::getBoolean()
{
  return librdf_query_results_get_boolean( d->queryResult ) > 0;
}
