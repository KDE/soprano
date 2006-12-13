/* 
 * This file is part of Soprano Project
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

#include "Node.h"
#include "QueryResultIterator.h"

using namespace Soprano;

struct QueryResultIterator::Private
{
  Private() : qr(0L)
  {}

  QueryResult *qr;
  QMap<QString, Node> row;
};

QueryResultIterator::QueryResultIterator( QueryResult *qr )
{
  d = new Private();
  d->qr = qr;
}

QueryResultIterator::~QueryResultIterator()
{
  delete d->qr;
  delete d;
}

bool QueryResultIterator::hasNext() const
{
  return d->qr->hasNext();
}

const QMap<QString, Node> &QueryResultIterator::next() const
{
  QStringList names = d->qr->bindingNames();
  QStringListIterator iter( names );
  while ( iter.hasNext() )
  {
    QString col = iter.next();
    d->row[ col ] = d->qr->binding( col );
  }

  d->qr->next();
  return d->row;
}
