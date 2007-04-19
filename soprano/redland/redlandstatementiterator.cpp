/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#include "redlandstatementiterator.h"

#include "statement.h"
#include "redlandutil.h"

#include <QtCore/QtGlobal>

#include "redland_stream_adapter.h"

namespace Soprano {
  namespace Redland {

RedlandStatementIterator::RedlandStatementIterator( stream_adapter *s ): m_stream( s )
{
}

RedlandStatementIterator::~RedlandStatementIterator()
{
  if ( !m_stream->impl )
  {
    // Model is dead!
    free_stream_adapter( m_stream );
  } else {
    free_stream_adapter_backend( m_stream);
  }
}

bool RedlandStatementIterator::hasNext() const
{
  return stream_adapter_end( m_stream ) == 0;
}

Soprano::Statement RedlandStatementIterator::next() const
{
  // FIXME: use librdf_stream_get_context to get the context of the statement

  librdf_statement *st = stream_adapter_get_object( m_stream );
  if ( !st )
  {
    // Return a not valid Statement
    // as last value.
    return Soprano::Statement();
  }

  Statement copy = Util::createStatement( st );

  // Move to the next element
  stream_adapter_next( m_stream );

  return copy;
}

}
}