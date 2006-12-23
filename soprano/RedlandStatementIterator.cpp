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

#include <QtGlobal>

#include "RedlandUtil.h"
#include "RedlandStatementIterator.h"
#include "Statement.h"

using namespace Soprano::Backend::Redland;

RedlandStatementIterator::RedlandStatementIterator( librdf_model *model )
{
  m_model = model;
  m_stream = librdf_model_as_stream( model );
}

RedlandStatementIterator::~RedlandStatementIterator()
{
  librdf_storage *storage = librdf_model_get_storage( m_model );

  librdf_free_stream ( m_stream );
  librdf_free_model( m_model );
  librdf_free_storage( storage );
}

bool RedlandStatementIterator::hasNext() const
{
  return librdf_stream_end( m_stream ) == 0; 
}

const Soprano::Statement RedlandStatementIterator::next() const
{
  librdf_statement *st = librdf_stream_get_object( m_stream );
  if ( !st )
  {
    // Return a not valid Statement
    // as last value.
    return Soprano::Statement();
  }

  Statement copy = Util::createStatement( st );

  // Move to the next element
  librdf_stream_next( m_stream );

  return copy;
}

