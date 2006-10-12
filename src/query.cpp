/* This file is part of QRDF
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
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
#include "world.h"
#include "query.h"

using namespace RDF;

struct Query::Private
{
  Private() : query(0L)
  {}
  librdf_query *query;
};

Query::Query(World *world, const QString &query)
{
  d = new Private;
  d->query = librdf_new_query( world->worldPtr(), "rdql", NULL, (unsigned char *) query.toLatin1().constData(), NULL);
  Q_ASSERT(d->query != NULL);
}

Query::~Query()
{
  librdf_free_query(d->query);
}

int Query::limit()
{
  return librdf_query_get_limit( d->query );
}

void Query::setLimit(int limit)
{
  librdf_query_set_limit( d->query, limit );
}

int Query::offset()
{
  return librdf_query_get_offset( d->query );
}

void Query::setOffset(int offset)
{
  librdf_query_set_offset( d->query, offset ); 
}

librdf_query* Query::hook()
{
  return d->query;
}

