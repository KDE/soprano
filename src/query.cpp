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
#include "query.h"

using namespace RDF;

struct Query::Private
{
  Private() : query(0L)
  {}
  librdf_query *query;
};

Query::Query()
{
  d = new Private;
  d->query = librdf_new_query();
  Q_ASSERT(d->query != NULL);
  librdf_query_open(d->query);
}

Query::~Query()
{
  librdf_free_query(d->query);
}





librdf_query* Query::queryPtr()
{
  return d->query;
}

