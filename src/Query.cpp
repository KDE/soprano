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
#include "Query.h"
using namespace RDF;

struct Query::Private
{
  Private() : limit(-1), offset(-1)
  {}
  int limit;
  int offset;
  QString query;
  QueryType type;
};

Query::Query( const QString &query, QueryType type )
{
  d = new Private;
  d->query = query;
  d->type = type;
}

Query::Query( const QString &query, QueryType type, int limit, int offset )
{
  d = new Private;
  d->query = query;
  d->type = type;
  d->limit = limit;
  d->offset = offset;
}

Query::Query( const Query &other )
{
  d = new Private;
  d->query = other.query();
  d->type = other.type();
  d->limit = other.limit();
  d->offset = other.offset();
}

Query::~Query()
{
  delete d;
}

Query::QueryType Query::type() const
{
  return d->type;
}

const QString &Query::query() const
{
  return d->query;
}

int Query::limit() const
{
  return d->limit;
}

void Query::setLimit(int limit)
{
  d->limit = limit;
}

int Query::offset() const
{
  return d->offset;
}

void Query::setOffset(int offset)
{
  d->offset = offset;
}

