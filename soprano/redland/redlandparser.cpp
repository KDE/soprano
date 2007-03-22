/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
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


#include <redland.h>

#include "redlandworld.h"
#include "redlandmodel.h"
#include "redlandparser.h"

#include <QtCore>

namespace Soprano {
  namespace Redland {

RedlandParser::RedlandParser()
{
}

RedlandModel *RedlandParser::parse( const QUrl &toparse ) const
{
  QUrl tmp(toparse);
  if ( toparse.scheme().isEmpty() )
  {
    // we need to help the stupid librdf file url handling
    tmp.setScheme("file");
  }

  librdf_world *w = World::self()->worldPtr();

  // Create a memory model
  librdf_storage *storage = librdf_new_storage( w, "hashes", NULL, "hash-type='memory',contexts='yes'" );
  if ( !storage )
  {
    return 0L;
  }

  librdf_model *model = librdf_new_model( w, storage, 0L);
  if ( !model )
  {
    librdf_free_storage( storage );
    return 0L;
  }

  librdf_uri *uri = librdf_new_uri( w, (unsigned char *) tmp.toString().toLatin1().data() );
  if ( !uri )
  {
    librdf_free_model( model );
    librdf_free_storage( storage );
    return 0L;
  }

  librdf_parser *parser = librdf_new_parser( w, "rdfxml", "application/rdf+xml", NULL );
  if ( !parser )
  {
    librdf_free_model( model );
    librdf_free_storage( storage );
    librdf_free_uri( uri );
    return 0L;
  }

  if ( librdf_parser_parse_into_model( parser, uri, uri, model ) )
  {
    librdf_free_model( model );
    librdf_free_storage( storage );
    librdf_free_uri( uri );
    librdf_free_parser( parser );
    return 0L;
  }

  librdf_free_uri( uri );
  librdf_free_parser( parser );

  return new RedlandModel( model, storage );
}

}
}
