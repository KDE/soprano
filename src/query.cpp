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
  Private() : query(0L), limit(-1), offset(0)
  {}
  int limit;
  int offset;
  QString *query;
};

Query::Query(QString *query)
{
  d = new Private;
  d->query = query;
  Q_ASSERT(d->query != NULL);
}

Query::Query(QString *query, int limit, int offset)
{
  d = new Private;
  d->query = query;
  d->limit = limit;
  d->offset = offset;
  Q_ASSERT(d->query != NULL);
}

Query::~Query()
{
  delete d->query;
  delete d;
}

int Query::limit()
{
  return d->limit;
}

void Query::setLimit(int limit)
{
  d->limit = limit;
}

int Query::offset()
{
  return d->offset;
}

void Query::setOffset(int offset)
{
  d->offset = offset;
}

librdf_query* Query::hook(World *world)
{
  librdf_query *q = librdf_new_query( world->hook(), NULL, NULL, (unsigned char *) d->query->toLatin1().constData(), NULL);
  librdf_query_set_limit( q, d->limit );
  librdf_query_set_offset( q, d->offset );

  return q;
}

