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

#include <stdlib.h>
#include "redland_stream_adapter.h"

void free_stream_adapter( stream_adapter *s )
{
  free( s );
}

void free_stream_adapter_backend( stream_adapter *s )
{
  librdf_free_stream( s->impl );
  s->impl = 0;
}

librdf_statement *stream_adapter_get_object( stream_adapter *s )
{
  if ( !s->impl ) return 0;
  return librdf_stream_get_object( s->impl );
}

librdf_node *stream_adapter_get_context( stream_adapter *s )
{
    if ( !s->impl ) return 0;
    return (librdf_node*)librdf_stream_get_context( s->impl );
}

void stream_adapter_next( stream_adapter *s )
{
  if ( !s->impl ) return;

  librdf_stream_next( s->impl );
}

int stream_adapter_end( stream_adapter *s )
{
  return s->impl ? librdf_stream_end( s->impl ) : 1;
}
