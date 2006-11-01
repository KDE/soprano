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

#include <redland.h>

#include "RedlandUtil.h"
#include "RedlandStatementIterator.h"
using namespace RDF;

struct RedlandStatementIterator::Private
{
  Private(): stream(0L)
  {}

  librdf_stream *stream;
};

RedlandStatementIterator::RedlandStatementIterator( librdf_stream *stream )
{
  d = new Private;
  d->stream = stream;
}

RedlandStatementIterator::~RedlandStatementIterator()
{
  librdf_free_stream( d->stream );
  delete d;
}

bool RedlandStatementIterator::hasNext() const
{
  return librdf_stream_end( d->stream) != 0; 
}

const Statement RedlandStatementIterator::next() const
{
  librdf_stream_next( d->stream );

  librdf_statement *st = librdf_stream_get_object( d->stream );

  return Redland::createStatement( st );
}

